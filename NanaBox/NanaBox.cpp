/*
 * PROJECT:   NanaBox
 * FILE:      NanaBox.cpp
 * PURPOSE:   Implementation for NanaBox
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#include <Windows.h>

#include <ShlObj.h>

#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")

#include "RdpClient.h"
#include "HostCompute.h"
#include "VirtualMachineConfiguration.h"

#include <cwchar>

#include <filesystem>

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlhost.h>
#include <atltypes.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>

#include "pch.h"

#include "App.h"
#include "MainWindowControl.h"
#include "MainWindowExitNoticeControl.h"

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <json.hpp>

#include <Mile.Project.Version.h>

#include "Utils.h"
#include "Environment.h"
#include "NanaBoxResources.h"
#include "ConfigurationWindow.h"

#include <Mile.Helpers.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace winrt
{
    using Windows::UI::Xaml::ElementTheme;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::FocusState;
    using Windows::UI::Xaml::UIElement;
    using Windows::UI::Xaml::Controls::Control;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs;
    using Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason;
    using Windows::UI::Xaml::Media::VisualTreeHelper;
}

namespace
{
    const std::wstring_view g_AppUserModelID =
        L"M2Team.NanaBox"
        L"_" MILE_PROJECT_DOT_VERSION_STRING
        L"_" MILE_PROJECT_BUILD_DATE_STRING;

    const std::wstring_view g_WindowTitle =
        L"NanaBox " MILE_PROJECT_VERSION_STRING
        L" (" MILE_PROJECT_DOT_VERSION_STRING L")";

    WTL::CAppModule g_Module;
    std::filesystem::path g_ConfigurationFilePath;
}

namespace NanaBox
{
    using winrt::NanaBox::MainWindowExitNoticeStatus;

    class MainWindowExitNoticeWindow :
        public ATL::CWindowImpl<MainWindowExitNoticeWindow>,
        public WTL::CMessageFilter
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaBox.MainWindowExitNoticeWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(MainWindowExitNoticeWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        virtual BOOL PreTranslateMessage(
            MSG* pMsg);

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnDestroy();
    };
}

BOOL NanaBox::MainWindowExitNoticeWindow::PreTranslateMessage(
    MSG* pMsg)
{
    // Workaround for capturing Alt+F4 in applications with XAML Islands.
    // Reference: https://github.com/microsoft/microsoft-ui-xaml/issues/2408
    if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
    {
        ::SendMessageW(
            ::GetAncestor(pMsg->hwnd, GA_ROOT),
            pMsg->message,
            pMsg->wParam,
            pMsg->lParam);

        return TRUE;
    }

    return FALSE;
}

int NanaBox::MainWindowExitNoticeWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    if (this->DefWindowProcW() != 0)
    {
        return -1;
    }

    {
        HMENU MenuHandle = this->GetSystemMenu(FALSE);
        if (MenuHandle)
        {
            ::RemoveMenu(MenuHandle, 0, MF_SEPARATOR);
            ::RemoveMenu(MenuHandle, SC_RESTORE, MF_BYCOMMAND);
            ::RemoveMenu(MenuHandle, SC_SIZE, MF_BYCOMMAND);
            ::RemoveMenu(MenuHandle, SC_MINIMIZE, MF_BYCOMMAND);
            ::RemoveMenu(MenuHandle, SC_MAXIMIZE, MF_BYCOMMAND);
            ::RemoveMenu(MenuHandle, SC_TASKLIST, MF_BYCOMMAND);
        }
    }

    return 0;
}

void NanaBox::MainWindowExitNoticeWindow::OnDestroy()
{
    ::PostQuitMessage(0);
}

namespace NanaBox
{
    namespace MainWindowTimerEvents
    {
        enum
        {
            SyncDisplaySettings = 1,
        };
    }

    enum class RdpClientMode : std::uint32_t
    {
        BasicSession = 0,
        EnhancedSession = 1,
        EnhancedVideoSyncedSession = 2
    };

    class MainWindow
        : public ATL::CWindowImpl<MainWindow>
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaBoxMainWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_SIZE(OnSize)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_ACTIVATE(OnActivate)
            MSG_WM_SETTINGCHANGE(OnSettingChange)
            MSG_WM_CLOSE(OnClose)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnTimer(
            UINT_PTR nIDEvent);

        void OnSize(
            UINT nType,
            CSize size);

        void OnSetFocus(
            ATL::CWindow wndOld);

        void OnActivate(
            UINT nState,
            BOOL bMinimized,
            ATL::CWindow wndOther);

        void OnSettingChange(
            UINT uFlags,
            LPCTSTR lpszSection);

        void OnClose();

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;
        winrt::DesktopWindowXamlSource m_XamlSource;
        const int m_MainWindowControlHeight = 48;
        int m_RecommendedMainWindowControlHeight = m_MainWindowControlHeight;
        winrt::NanaBox::MainWindowControl m_MainWindowControl;
        NanaBox::VirtualMachineConfiguration m_Configuration;
        winrt::com_ptr<NanaBox::ComputeSystem> m_VirtualMachine;
        std::string m_VirtualMachineGuid;
        bool m_VirtualMachineRunning = false;
        bool m_VirtualMachineRestarting = false;
        RdpClientMode m_RdpClientMode = RdpClientMode::BasicSession;
        CSize m_RecommendedDisplayResolution = CSize(1024, 768);
        std::uint32_t m_RecommendedZoomLevel = 100;
        CSize m_DisplayResolution = CSize(1024, 768);
        UINT64 m_SyncDisplaySettingsCheckPoint = 0;
        RECT m_RememberedMainWindowRect;
        LONG_PTR m_RememberedMainWindowStyle;

        void InitializeVirtualMachine();
    };

}

int NanaBox::MainWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    this->m_ApplicationIcon.LoadIconW(
        MAKEINTRESOURCE(IDI_NANABOX),
        256,
        256,
        LR_SHARED);

    this->SetIcon(this->m_ApplicationIcon, TRUE);
    this->SetIcon(this->m_ApplicationIcon, FALSE);

    this->m_RdpClient = winrt::make_self<NanaBox::RdpClient>();

    this->m_RdpClient->EnableEventsDispatcher();

    if (!this->m_RdpClientWindow.Create(
        this->m_hWnd,
        this->m_RdpClientWindow.rcDefault,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN))
    {
        return -1;
    }

    winrt::check_hresult(this->m_RdpClientWindow.AttachControl(
        this->m_RdpClient->RawControl().get(),
        nullptr));

    try
    {
        this->InitializeVirtualMachine();
    }
    catch (winrt::hresult_error const& ex)
    {
        std::thread([ex]()
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                ex.message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }).join();
        return -1;
    }
    catch (std::exception const& ex)
    {
        std::thread([ex]()
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                winrt::to_hstring(ex.what()).c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }).join();
        return -1;
    }

    this->m_MainWindowControl =
        winrt::make<winrt::NanaBox::implementation::MainWindowControl>();
    this->m_MainWindowControl.RequestEnhancedSession([this](
        bool const& RequestState)
    {
        this->m_RdpClientMode =
            RequestState
            ? RdpClientMode::EnhancedSession
            : RdpClientMode::BasicSession;
        this->m_RdpClient->Disconnect();
    });
    this->m_MainWindowControl.RequestFullScreen([this]()
    {
        // If ZoomLevel doesn't equal 100, Basic Session Mode can't enter full screen.
        if (this->m_RdpClientMode == RdpClientMode::BasicSession)
        {
            this->m_RecommendedZoomLevel = 100;
            VARIANT RawZoomLevel;
            RawZoomLevel.vt = VT_UI4;
            RawZoomLevel.uintVal = this->m_RecommendedZoomLevel;
            this->m_RdpClient->Property(
                L"ZoomLevel",
                RawZoomLevel);
        }
        this->m_RdpClient->FullScreen(true);
    });
    this->m_MainWindowControl.RequestPauseVirtualMachine([this](
        bool const& RequestState)
    {
        if (RequestState)
        {
            this->m_VirtualMachine->Pause();
        }
        else
        {
            this->m_VirtualMachine->Resume();
        }
    });
    this->m_MainWindowControl.RequestRestartVirtualMachine([this]()
    {
        this->m_VirtualMachineRestarting = true;
        this->m_VirtualMachine->Terminate();
        this->m_VirtualMachine = nullptr;

        this->InitializeVirtualMachine();
        this->m_RdpClient->Disconnect();
    });

    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();
    winrt::check_hresult(
        XamlSourceNative->AttachToWindow(this->m_hWnd));
    this->m_XamlSource.Content(this->m_MainWindowControl);

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));

    // Focus on XAML Island host window for Acrylic brush support.
    ::SetFocus(XamlWindowHandle);

    winrt::FrameworkElement Content =
        this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();

    MARGINS Margins = { -1 };
    ::DwmExtendFrameIntoClientArea(this->m_hWnd, &Margins);

    ::MileSetWindowSystemBackdropTypeAttribute(
        this->m_hWnd,
        MILE_WINDOW_SYSTEM_BACKDROP_TYPE_MICA);

    ::MileEnableImmersiveDarkModeForWindow(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? TRUE
            : FALSE));

    this->m_RdpClient->EnableAutoReconnect(false);
    this->m_RdpClient->RelativeMouseMode(true);
    this->m_RdpClient->AuthenticationServiceClass(
        L"Microsoft Virtual Console Service");

    this->m_RdpClient->AuthenticationLevel(0);
    this->m_RdpClient->EnableCredSspSupport(true);
    this->m_RdpClient->NegotiateSecurityLayer(false);

    // Set KeyboardHookMode to remote for fixing keyboard navigation issues in
    // window mode.
    // TODO: Connect with m_Configuration.Keyboard.RedirectKeyCombinations
    this->m_RdpClient->KeyboardHookMode(1);

    this->m_RdpClient->ContainerHandledFullScreen(true);

    try
    {
        VARIANT Value;
        Value.vt = VT_BOOL;
        Value.boolVal = VARIANT_TRUE;
        this->m_RdpClient->Property(L"DisableCredentialsDelegation", Value);
    }
    catch (...)
    {

    }

    this->m_RdpClient->PerformanceFlags(
        TS_PERF_ENABLE_ENHANCED_GRAPHICS |
        TS_PERF_ENABLE_FONT_SMOOTHING |
        TS_PERF_ENABLE_DESKTOP_COMPOSITION);

    try
    {
        VARIANT Value;
        Value.vt = VT_BOOL;
        Value.boolVal = VARIANT_TRUE;
        this->m_RdpClient->Property(L"EnableHardwareMode", Value);
    }
    catch (...)
    {

    }

    try
    {
        VARIANT Value;
        Value.vt = VT_BOOL;
        Value.boolVal = VARIANT_TRUE;
        this->m_RdpClient->Property(L"EnableFrameBufferRedirection", Value);
    }
    catch (...)
    {

    }

    this->m_RdpClient->GrabFocusOnConnect(false);

    this->m_RdpClient->OnRemoteDesktopSizeChange.add([this](
        _In_ LONG Width,
        _In_ LONG Height)
    {
        if (this->m_RdpClientMode == RdpClientMode::BasicSession)
        {
            if (this->IsZoomed())
            {
                return;
            }

            this->m_DisplayResolution = CSize(Width, Height);

            UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

            RECT WindowRect;
            winrt::check_bool(this->GetWindowRect(&WindowRect));
            {
                int WindowWidth = 0;
                WindowWidth += this->m_DisplayResolution.cx;
                int WindowHeight = this->m_RecommendedMainWindowControlHeight;
                WindowHeight += this->m_DisplayResolution.cy;

                RECT ClientRect;
                winrt::check_bool(this->GetClientRect(&ClientRect));

                WindowRect.right -= ClientRect.right;
                WindowRect.bottom -= ClientRect.bottom;
                WindowRect.right += ::MulDiv(
                    WindowWidth,
                    DpiValue,
                    USER_DEFAULT_SCREEN_DPI);
                WindowRect.bottom += ::MulDiv(
                    WindowHeight,
                    DpiValue,
                    USER_DEFAULT_SCREEN_DPI);
            }

            this->SetWindowPos(
                nullptr,
                &WindowRect,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
    });
    this->m_RdpClient->OnLoginComplete.add([this]()
    {
        if (this->m_RdpClientMode == RdpClientMode::EnhancedSession)
        {
            this->m_RdpClientMode = RdpClientMode::EnhancedVideoSyncedSession;
            this->m_DisplayResolution = CSize();
        }
    });
    this->m_RdpClient->OnDisconnected.add([this](
        _In_ LONG DisconnectReason)
    {
        UNREFERENCED_PARAMETER(DisconnectReason);

        if (this->m_RdpClientMode == RdpClientMode::EnhancedVideoSyncedSession)
        {
            this->m_RdpClientMode = RdpClientMode::EnhancedSession;
        }

        if (this->m_VirtualMachineRunning)
        {
            try
            {
                std::string PCB = this->m_VirtualMachineGuid;
                if (this->m_RdpClientMode == RdpClientMode::EnhancedSession)
                {
                    PCB += ";EnhancedMode=1";

                    this->m_RdpClient->DesktopWidth(
                        this->m_RecommendedDisplayResolution.cx);
                    this->m_RdpClient->DesktopHeight(
                        this->m_RecommendedDisplayResolution.cy);

                    VARIANT RawZoomLevel;
                    RawZoomLevel.vt = VT_UI4;
                    RawZoomLevel.uintVal = 100;
                    this->m_RdpClient->Property(
                        L"ZoomLevel",
                        RawZoomLevel);
                }
                else if (this->m_RdpClientMode == RdpClientMode::BasicSession)
                {
                    VARIANT RawZoomLevel;
                    RawZoomLevel.vt = VT_UI4;
                    RawZoomLevel.uintVal = this->m_RecommendedZoomLevel;
                    this->m_RdpClient->Property(
                        L"ZoomLevel",
                        RawZoomLevel);
                }
                this->m_RdpClient->PCB(winrt::to_hstring(PCB));
                this->m_RdpClient->Connect();
            }
            catch (...)
            {

            }
        }
    });
    this->m_RdpClient->OnRequestGoFullScreen.add([this]()
    {
        HMONITOR m_hMonitor = ::MonitorFromWindow(
            m_hWnd,
            MONITOR_DEFAULTTONULL);
        winrt::check_pointer(m_hMonitor);
        MONITORINFOEX mi;
        mi.cbSize = sizeof(MONITORINFOEX);
        winrt::check_bool(::GetMonitorInfoW(
            m_hMonitor,
            &mi));
        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        winrt::check_bool(::EnumDisplaySettingsW(
            mi.szDevice,
            ENUM_CURRENT_SETTINGS,
            &dm));

        winrt::check_bool(this->GetWindowRect(&this->m_RememberedMainWindowRect));
        this->m_RememberedMainWindowStyle = this->GetWindowLongPtrW(GWL_STYLE);

        RECT FullScreenRect;
        FullScreenRect.left = 0;
        FullScreenRect.top = 0;
        FullScreenRect.right = dm.dmPelsWidth;
        FullScreenRect.bottom = dm.dmPelsHeight;

        // Resize RdpClientWindow and hidden XamlIslands by OnSize method automatically calculate.
        m_RecommendedMainWindowControlHeight = 0;

        this->SetWindowLongPtrW(
            GWL_STYLE,
            WS_VISIBLE | WS_POPUP);
        // SetWindowPos will send WM_SIZE, RDP Zoom will be re-enabled in Basic Session Mode.
        this->SetWindowPos(
            HWND_TOP,
            &FullScreenRect,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
    });
    this->m_RdpClient->OnRequestLeaveFullScreen.add([this]()
    {
        m_RecommendedMainWindowControlHeight = m_MainWindowControlHeight;
        this->SetWindowLongPtrW(
            GWL_STYLE,
            this->m_RememberedMainWindowStyle);
        this->SetWindowPos(
            nullptr,
            &this->m_RememberedMainWindowRect,
            SWP_NOACTIVATE);
    });
    this->m_RdpClient->OnRequestContainerMinimize.add([this]()
    {
        this->ShowWindow(SW_MINIMIZE);
    });
    this->m_RdpClient->OnConfirmClose.add([this](
        VARIANT_BOOL* pfAllowClose)
    {
        // Set it FALSE because we don't want to close the connection before
        // making decisions in the close confirmation dialog.
        *pfAllowClose = VARIANT_FALSE;
        this->PostMessageW(WM_CLOSE);
    });

    this->SetTimer(
        NanaBox::MainWindowTimerEvents::SyncDisplaySettings,
        200);

    this->m_RdpClient->Server(L"localhost");
    this->m_RdpClient->RDPPort(2179);
    this->m_RdpClient->MinInputSendInterval(20);

    this->m_RdpClient->Connect();

    this->m_RdpClientWindow.SetFocus();

    return 0;
}

void NanaBox::MainWindow::OnTimer(
    UINT_PTR nIDEvent)
{
    if (nIDEvent == NanaBox::MainWindowTimerEvents::SyncDisplaySettings)
    {
        if (this->m_RdpClientMode != RdpClientMode::EnhancedVideoSyncedSession)
        {
            return;
        }

        UINT64 CurrentCheckPoint = ::GetTickCount64();
        if (this->m_SyncDisplaySettingsCheckPoint > CurrentCheckPoint)
        {
            return;
        }
        else
        {
            this->m_SyncDisplaySettingsCheckPoint = CurrentCheckPoint + 200;
        }

        CSize DisplayResolution = this->m_RecommendedDisplayResolution;
        if (this->m_DisplayResolution == DisplayResolution)
        {
            return;
        }

        try
        {
            this->m_RdpClient->UpdateSessionDisplaySettings(
                DisplayResolution.cx,
                DisplayResolution.cy,
                DisplayResolution.cx,
                DisplayResolution.cy,
                0,
                this->m_RecommendedZoomLevel,
                100);

            this->m_DisplayResolution = DisplayResolution;
        }
        catch (...)
        {

        }
    }
}

void NanaBox::MainWindow::OnSize(
    UINT nType,
    CSize size)
{
    UNREFERENCED_PARAMETER(nType);
    UNREFERENCED_PARAMETER(size);

    if (nType == SIZE_MINIMIZED)
    {
        return;
    }

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    this->m_RecommendedZoomLevel = ::MulDiv(
        100,
        DpiValue,
        USER_DEFAULT_SCREEN_DPI);

    int MainWindowControlScaledHeight = ::MulDiv(
        this->m_RecommendedMainWindowControlHeight,
        DpiValue,
        USER_DEFAULT_SCREEN_DPI);

    MARGINS Margins = { 0 };
    Margins.cyTopHeight = MainWindowControlScaledHeight;
    ::DwmExtendFrameIntoClientArea(this->m_hWnd, &Margins);

    RECT RdpClientRect;
    winrt::check_bool(this->GetClientRect(&RdpClientRect));
    RdpClientRect.top += MainWindowControlScaledHeight;

    this->m_RdpClientWindow.SetWindowPos(
        nullptr,
        &RdpClientRect,
        SWP_NOZORDER | SWP_NOACTIVATE);

    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));
    ::SetWindowPos(
        XamlWindowHandle,
        nullptr,
        0,
        0,
        RdpClientRect.right - RdpClientRect.left,
        MainWindowControlScaledHeight,
        SWP_SHOWWINDOW);

    this->m_RecommendedDisplayResolution = CSize(
        RdpClientRect.right - RdpClientRect.left,
        RdpClientRect.bottom - RdpClientRect.top);

    if (this->m_RdpClientMode == RdpClientMode::BasicSession)
    {
        VARIANT RawZoomLevel;
        RawZoomLevel.vt = VT_UI4;
        RawZoomLevel.uintVal = this->m_RecommendedZoomLevel;
        this->m_RdpClient->Property(
            L"ZoomLevel",
            RawZoomLevel);
    }
}

void NanaBox::MainWindow::OnSetFocus(
    ATL::CWindow wndOld)
{
    UNREFERENCED_PARAMETER(wndOld);

    this->m_RdpClientWindow.SetFocus();
}

void NanaBox::MainWindow::OnActivate(
    UINT nState,
    BOOL bMinimized,
    ATL::CWindow wndOther)
{
    UNREFERENCED_PARAMETER(wndOther);

    if (bMinimized || nState == WA_INACTIVE)
    {
        return;
    }

    this->m_RdpClientWindow.SetFocus();
}

void NanaBox::MainWindow::OnSettingChange(
    UINT uFlags,
    LPCTSTR lpszSection)
{
    UNREFERENCED_PARAMETER(uFlags);

    if (lpszSection && 0 == std::wcscmp(
        lpszSection,
        L"ImmersiveColorSet"))
    {
        winrt::FrameworkElement Content =
            this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();
        if (Content &&
            winrt::VisualTreeHelper::GetParent(Content))
        {
            Content.RequestedTheme(winrt::ElementTheme::Default);

            ::MileEnableImmersiveDarkModeForWindow(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? TRUE
                    : FALSE));
        }
    }
}

void NanaBox::MainWindow::OnClose()
{
    if (!this->m_VirtualMachineRunning)
    {
        this->DestroyWindow();
        return;
    }

    const int Width = 400;
    const int Height = 200;

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    using MainWindowExitNoticeControlInstance =
        winrt::NanaBox::implementation::MainWindowExitNoticeControl;

    winrt::NanaBox::MainWindowExitNoticeControl Control =
        winrt::make<MainWindowExitNoticeControlInstance>();

    NanaBox::MainWindowExitNoticeWindow Window;
    if (!Window.Create(
        this->m_hWnd,
        Window.rcDefault,
        nullptr,
        WS_CAPTION | WS_SYSMENU,
        WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME,
        nullptr,
        winrt::get_abi(Control)))
    {
        return;
    }
    Control.RequestCloseDialog([&]()
    {
        Window.DestroyWindow();
    });
    Window.SetWindowPos(
        nullptr,
        0,
        0,
        ::MulDiv(Width, DpiValue, USER_DEFAULT_SCREEN_DPI),
        ::MulDiv(Height, DpiValue, USER_DEFAULT_SCREEN_DPI),
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    Window.CenterWindow(this->m_hWnd);
    Window.ShowWindow(SW_SHOW);
    Window.UpdateWindow();

    WTL::CMessageLoop MessageLoop;
    MessageLoop.AddMessageFilter(&Window);
    this->EnableWindow(FALSE);
    MessageLoop.Run();
    this->EnableWindow(TRUE);
    this->SetActiveWindow();

    if (Control.Status() == NanaBox::MainWindowExitNoticeStatus::Suspend)
    {
        this->m_VirtualMachine->Pause();

        if (this->m_Configuration.SaveStateFile.empty())
        {
            this->m_Configuration.SaveStateFile =
                this->m_Configuration.Name + ".SaveState.vmrs";
        }

        std::filesystem::path SaveStateFile = std::filesystem::absolute(
            winrt::to_hstring(this->m_Configuration.SaveStateFile).c_str());
        if (std::filesystem::exists(SaveStateFile))
        {
            std::filesystem::perms perms = std::filesystem::perms::none;
            perms |= std::filesystem::perms::owner_write;
            perms |= std::filesystem::perms::group_write;
            perms |= std::filesystem::perms::others_write;

            std::filesystem::permissions(
                SaveStateFile,
                perms,
                std::filesystem::perm_options::remove);

            std::filesystem::remove(SaveStateFile);
        }

        nlohmann::json Options;
        Options["SaveType"] = "ToFile";
        Options["SaveStateFilePath"] = winrt::to_string(SaveStateFile.c_str());

        try
        {
            this->m_VirtualMachine->Save(winrt::to_hstring(Options.dump()));
        }
        catch (winrt::hresult_error const& ex)
        {
            std::filesystem::perms perms = std::filesystem::perms::none;
            perms |= std::filesystem::perms::owner_write;
            perms |= std::filesystem::perms::group_write;
            perms |= std::filesystem::perms::others_write;

            std::filesystem::permissions(
                SaveStateFile,
                perms,
                std::filesystem::perm_options::remove);

            std::filesystem::remove(SaveStateFile);

            this->m_Configuration.SaveStateFile.clear();

            std::thread([ex]()
            {
                ::TaskDialog(
                    nullptr,
                    nullptr,
                    g_WindowTitle.data(),
                    ex.message().c_str(),
                    nullptr,
                    TDCBF_OK_BUTTON,
                    TD_ERROR_ICON,
                    nullptr);
            }).join();
        }

        if (this->m_Configuration.SaveStateFile.empty())
        {
            this->m_VirtualMachine->Resume();
        }
        else
        {
            this->m_VirtualMachine->Terminate();

            std::string ConfigurationFileContent =
                NanaBox::SerializeConfiguration(this->m_Configuration);
            ::WriteAllTextToUtf8TextFile(
                g_ConfigurationFilePath,
                ConfigurationFileContent);
        }
    }
    else if (Control.Status() == NanaBox::MainWindowExitNoticeStatus::PowerOff)
    {
        this->m_VirtualMachine->Pause();
        this->m_VirtualMachine->Terminate();
    }
}

void NanaBox::MainWindow::OnDestroy()
{
    this->KillTimer(
        NanaBox::MainWindowTimerEvents::SyncDisplaySettings);

    this->m_RdpClientWindow.DestroyWindow();
    this->m_RdpClient = nullptr;

    ::PostQuitMessage(0);
}

void NanaBox::MainWindow::InitializeVirtualMachine()
{
    std::string ConfigurationFileContent = ::ReadAllTextFromUtf8TextFile(
        g_ConfigurationFilePath);

    this->m_Configuration = NanaBox::DeserializeConfiguration(
        ConfigurationFileContent);

    for (NanaBox::ScsiDeviceConfiguration& ScsiDevice
        : this->m_Configuration.ScsiDevices)
    {
        if (ScsiDevice.Type == NanaBox::ScsiDeviceType::PhysicalDevice)
        {
            break;
        }

        std::filesystem::path Path = std::filesystem::absolute(
            winrt::to_hstring(ScsiDevice.Path).c_str());
        if (std::filesystem::exists(Path))
        {
            winrt::check_hresult(::HcsGrantVmAccess(
                winrt::to_hstring(this->m_Configuration.Name).c_str(),
                Path.c_str()));
        }
    }

    if (this->m_Configuration.GuestStateFile.empty())
    {
        this->m_Configuration.GuestStateFile =
            this->m_Configuration.Name + ".vmgs";
    }
    {
        std::filesystem::path GuestStateFile = std::filesystem::absolute(
            winrt::to_hstring(this->m_Configuration.GuestStateFile).c_str());
        if (!std::filesystem::exists(GuestStateFile))
        {
            winrt::check_hresult(::HcsCreateEmptyGuestStateFile(
                GuestStateFile.c_str()));
        }

        winrt::check_hresult(::HcsGrantVmAccess(
            winrt::to_hstring(this->m_Configuration.Name).c_str(),
            GuestStateFile.c_str()));
    }

    if (this->m_Configuration.RuntimeStateFile.empty())
    {
        this->m_Configuration.RuntimeStateFile =
            this->m_Configuration.Name + ".vmrs";
    }
    {
        std::filesystem::path RuntimeStateFile = std::filesystem::absolute(
            winrt::to_hstring(this->m_Configuration.RuntimeStateFile).c_str());
        if (!std::filesystem::exists(RuntimeStateFile))
        {
            winrt::check_hresult(::HcsCreateEmptyRuntimeStateFile(
                RuntimeStateFile.c_str()));
        }

        winrt::check_hresult(::HcsGrantVmAccess(
            winrt::to_hstring(this->m_Configuration.Name).c_str(),
            RuntimeStateFile.c_str()));
    }

    if (!this->m_Configuration.SaveStateFile.empty())
    {
        std::filesystem::path SaveStateFile = std::filesystem::absolute(
            winrt::to_hstring(this->m_Configuration.SaveStateFile).c_str());
        if (std::filesystem::exists(SaveStateFile))
        {
            winrt::check_hresult(::HcsGrantVmAccess(
                winrt::to_hstring(this->m_Configuration.Name).c_str(),
                SaveStateFile.c_str()));
        }
    }

    this->m_VirtualMachine = winrt::make_self<NanaBox::ComputeSystem>(
        winrt::to_hstring(
            this->m_Configuration.Name),
        winrt::to_hstring(
            NanaBox::MakeHcsConfiguration(this->m_Configuration)));

    this->m_VirtualMachine->SystemExited.add([this](
        winrt::hstring const& EventData)
    {
        UNREFERENCED_PARAMETER(EventData);

        if (this->m_VirtualMachineRestarting)
        {
            this->m_VirtualMachineRestarting = false;
            return;
        }

        this->m_VirtualMachineRunning = false;
        this->m_RdpClient->DisableEventsDispatcher();
        ::SleepEx(200, FALSE);
        this->PostMessageW(WM_CLOSE);
    });

    /*this->m_VirtualMachine->SystemRdpEnhancedModeStateChanged.add([this]()
    {
        this->m_EnableEnhancedMode = !this->m_EnableEnhancedMode;
    });*/

    this->m_VirtualMachine->Start();

    NanaBox::HcnNetwork NetworkHandle = NanaBox::HcnOpenNetwork(
        NanaBox::DefaultSwitchId);

    std::string DefaultSwitchIdString = winrt::to_string(
        ::FromGuid(NanaBox::DefaultSwitchId));

    if (!this->m_Configuration.NetworkAdapters.empty())
    {
        for (NanaBox::NetworkAdapterConfiguration& NetworkAdapter
            : this->m_Configuration.NetworkAdapters)
        {
            if (!NetworkAdapter.Enabled)
            {
                continue;
            }

            GUID EndpointId;
            try
            {
                if (!NetworkAdapter.EndpointId.empty())
                {
                    EndpointId = winrt::guid(NetworkAdapter.EndpointId);
                }
            }
            catch (...)
            {

            }
            if (NetworkAdapter.EndpointId.empty())
            {
                winrt::check_hresult(::CoCreateGuid(&EndpointId));
                NetworkAdapter.EndpointId =
                    winrt::to_string(::FromGuid(EndpointId));
            }

            try
            {
                NanaBox::HcnDeleteEndpoint(EndpointId);
            }
            catch (...)
            {

            }

            NanaBox::HcnEndpoint EndpointHandle;
            nlohmann::json Settings;
            Settings["SchemaVersion"]["Major"] = 2;
            Settings["SchemaVersion"]["Minor"] = 0;
            Settings["Owner"] = this->m_Configuration.Name;
            Settings["HostComputeNetwork"] = DefaultSwitchIdString;
            if (!NetworkAdapter.MacAddress.empty())
            {
                Settings["MacAddress"] = NetworkAdapter.MacAddress;
            }

            EndpointHandle = NanaBox::HcnCreateEndpoint(
                NetworkHandle,
                EndpointId,
                winrt::to_hstring(Settings.dump()));

            nlohmann::json Properties = nlohmann::json::parse(winrt::to_string(
                NanaBox::HcnQueryEndpointProperties(EndpointHandle)));
            NetworkAdapter.MacAddress = Properties["MacAddress"];

            this->m_VirtualMachine->Modify(winrt::to_hstring(
                NanaBox::MakeHcsAddNetworkAdapterRequest(NetworkAdapter)));
        }
    }

    this->m_VirtualMachine->Modify(winrt::to_hstring(
        NanaBox::MakeHcsUpdateGpuRequest(this->m_Configuration.Gpu)));

    this->m_VirtualMachineRunning = true;

    if (!this->m_Configuration.SaveStateFile.empty())
    {
        std::filesystem::path SaveStateFile = std::filesystem::absolute(
            winrt::to_hstring(this->m_Configuration.SaveStateFile).c_str());
        if (std::filesystem::exists(SaveStateFile))
        {
            std::filesystem::perms perms = std::filesystem::perms::none;
            perms |= std::filesystem::perms::owner_write;
            perms |= std::filesystem::perms::group_write;
            perms |= std::filesystem::perms::others_write;

            std::filesystem::permissions(
                SaveStateFile,
                perms,
                std::filesystem::perm_options::remove);

            std::filesystem::remove(SaveStateFile);
        }

        this->m_Configuration.SaveStateFile.clear();
    }

    ConfigurationFileContent =
        NanaBox::SerializeConfiguration(this->m_Configuration);
    ::WriteAllTextToUtf8TextFile(
        g_ConfigurationFilePath,
        ConfigurationFileContent);

    nlohmann::json Properties = nlohmann::json::parse(
        winrt::to_string(this->m_VirtualMachine->GetProperties()));
    this->m_VirtualMachineGuid = Properties["RuntimeId"];

    std::wstring WindowTitle =
        winrt::to_hstring(this->m_Configuration.Name).c_str();
    WindowTitle += L" - ";
    WindowTitle += g_WindowTitle;
    this->SetWindowTextW(WindowTitle.c_str());
    this->m_RdpClient->ConnectionBarText(WindowTitle.c_str());
}

void PrerequisiteCheck()
{
    try
    {
        NanaBox::HcsGetServiceProperties();
    }
    catch (winrt::hresult_error const& ex)
    {
        if (ex.code() == HCS_E_ACCESS_DENIED)
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                ::GetLocalizedString(
                    L"Messages/AccessDeniedInstructionText").c_str(),
                ::GetLocalizedString(
                    L"Messages/AccessDeniedContentText").c_str(),
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }
        else if (ex.code() == HCS_E_SERVICE_NOT_AVAILABLE)
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                ::GetLocalizedString(
                    L"Messages/HyperVNotAvailableInstructionText").c_str(),
                ::GetLocalizedString(
                    L"Messages/HyperVNotAvailableContentText").c_str(),
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }
        else
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                ex.message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }

        ::ExitProcess(ex.code());
    }

    bool IsHyperVManagementServiceRunning = false;
    {
        SC_HANDLE ManagerHandle = ::OpenSCManagerW(
            nullptr,
            nullptr,
            SC_MANAGER_CONNECT);
        if (ManagerHandle)
        {
            SC_HANDLE ServiceHandle = ::OpenServiceW(
                ManagerHandle,
                L"vmms",
                SERVICE_QUERY_STATUS);
            if (ServiceHandle)
            {
                SERVICE_STATUS_PROCESS ServiceStatus = { 0 };
                DWORD BytesNeeded = 0;
                if (::QueryServiceStatusEx(
                    ServiceHandle,
                    SC_STATUS_PROCESS_INFO,
                    reinterpret_cast<LPBYTE>(&ServiceStatus),
                    sizeof(SERVICE_STATUS_PROCESS),
                    &BytesNeeded))
                {
                    IsHyperVManagementServiceRunning =
                        (SERVICE_RUNNING == ServiceStatus.dwCurrentState);
                }

                ::CloseServiceHandle(ServiceHandle);
            }

            ::CloseServiceHandle(ManagerHandle);
        }
    }
    if (!IsHyperVManagementServiceRunning)
    {
        ::TaskDialog(
            nullptr,
            nullptr,
            g_WindowTitle.data(),
            ::GetLocalizedString(
                L"Messages/VMMSNotAvailableInstructionText").c_str(),
            ::GetLocalizedString(
                L"Messages/VMMSNotAvailableContentText").c_str(),
            TDCBF_OK_BUTTON,
            TD_ERROR_ICON,
            nullptr);
        ::ExitProcess(winrt::hresult_no_interface().code());
    }

    return;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);

    winrt::check_hresult(::SetCurrentProcessExplicitAppUserModelID(
        g_AppUserModelID.data()));

    std::wstring ApplicationName;
    std::map<std::wstring, std::wstring> OptionsAndParameters;
    std::wstring UnresolvedCommandLine;

    ::SpiltCommandLineEx(
        std::wstring(GetCommandLineW()),
        std::vector<std::wstring>{ L"-", L"/", L"--" },
        std::vector<std::wstring>{ L"=", L":" },
        ApplicationName,
        OptionsAndParameters,
        UnresolvedCommandLine);

    bool PackagedMode = ::IsPackagedMode();
    std::filesystem::path TargetBinaryPath;

    bool ShowVirtualMachineConfig = false;
    if (!OptionsAndParameters.empty())
    {
        auto iter = OptionsAndParameters.find(L"edit");
        if (iter != OptionsAndParameters.end())
        {
            ShowVirtualMachineConfig = true;
        }
    }

    if (!ShowVirtualMachineConfig)
    {
        if (PackagedMode)
        {
            try
            {
                std::filesystem::path AppBinaryPath;
                {
                    std::wstring RawPath = ::GetCurrentProcessModulePath();
                    std::wcsrchr(&RawPath[0], L'\\')[0] = L'\0';
                    RawPath.resize(std::wcslen(RawPath.c_str()));
                    AppBinaryPath = std::filesystem::absolute(RawPath);
                }

                std::filesystem::path TempBinaryPath;
                {
                    TempBinaryPath = ::GetLocalStateFolderPath();
                    GUID TempFolderGuid;
                    winrt::check_hresult(::CoCreateGuid(&TempFolderGuid));
                    TempBinaryPath /= ::FromGuid(TempFolderGuid).c_str();
                }

                if (std::filesystem::create_directory(TempBinaryPath))
                {
                    std::filesystem::copy_file(
                        AppBinaryPath / L"NanaBox.exe",
                        TempBinaryPath / L"NanaBox.exe");

                    std::filesystem::copy_file(
                        AppBinaryPath / L"resources.pri",
                        TempBinaryPath / L"resources.pri");

                std::filesystem::copy_file(
                    AppBinaryPath / L"Mile.Xaml.Styles.SunValley.xbf",
                    TempBinaryPath / L"Mile.Xaml.Styles.SunValley.xbf");

                TargetBinaryPath = TempBinaryPath;
            }
        }
        catch (winrt::hresult_error const& ex)
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                ex.message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
            ::ExitProcess(ex.code());
        }
        catch (std::exception const& ex)
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                winrt::to_hstring(ex.what()).c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
            ::ExitProcess(static_cast<UINT>(-1));
        }
    }

        if (!::IsCurrentProcessElevated() || PackagedMode)
        {
            try
            {
                if (PackagedMode && !TargetBinaryPath.empty())
                {
                    ApplicationName = TargetBinaryPath / L"NanaBox.exe";
                }
                else
                {
                    ApplicationName = ::GetCurrentProcessModulePath();
                }

                std::wstring parameters;
                for (auto&& item : OptionsAndParameters)
                {
                    if (!item.second.empty())
                    {
                        parameters.append(std::format(L"--{}={} ", item.first, item.second));
                    }
                    else
                    {
                        parameters.append(std::format(L"--{} ", item.first));
                    }
                }
                parameters.append(UnresolvedCommandLine);

                SHELLEXECUTEINFOW Information = { 0 };
                Information.cbSize = sizeof(SHELLEXECUTEINFOW);
                Information.fMask = SEE_MASK_NOCLOSEPROCESS;
                Information.lpVerb = L"runas";
                Information.nShow = nShowCmd;
                Information.lpFile = ApplicationName.c_str();

                Information.lpParameters = parameters.c_str();
                winrt::check_bool(::ShellExecuteExW(&Information));
                ::WaitForSingleObjectEx(Information.hProcess, INFINITE, FALSE);
                ::CloseHandle(Information.hProcess);

                if (PackagedMode && !TargetBinaryPath.empty())
                {
                    std::filesystem::remove_all(TargetBinaryPath);
                }
            }
            catch (winrt::hresult_error const& ex)
            {
                if (ex.code() != ::HRESULT_FROM_WIN32(ERROR_CANCELLED))
                {
                    ::TaskDialog(
                        nullptr,
                        nullptr,
                        g_WindowTitle.data(),
                        ex.message().c_str(),
                        nullptr,
                        TDCBF_OK_BUTTON,
                        TD_ERROR_ICON,
                        nullptr);
                }

                ::ExitProcess(ex.code());
            }
            catch (std::exception const& ex)
            {
                ::TaskDialog(
                    nullptr,
                    nullptr,
                    g_WindowTitle.data(),
                    winrt::to_hstring(ex.what()).c_str(),
                    nullptr,
                    TDCBF_OK_BUTTON,
                    TD_ERROR_ICON,
                    nullptr);
                ::ExitProcess(static_cast<UINT>(-1));
            }

            ::ExitProcess(0);
        }

        ::PrerequisiteCheck();
    }

    if (!UnresolvedCommandLine.empty())
    {
        g_ConfigurationFilePath = std::filesystem::absolute(
            UnresolvedCommandLine);
    }
    else
    {
        try
        {
            winrt::com_ptr<IFileDialog> FileDialog =
                winrt::create_instance<IFileDialog>(CLSID_FileOpenDialog);

            DWORD Flags = 0;
            winrt::check_hresult(FileDialog->GetOptions(&Flags));
            Flags |= FOS_FORCEFILESYSTEM;
            Flags |= FOS_NOCHANGEDIR;
            Flags |= FOS_DONTADDTORECENT;
            winrt::check_hresult(FileDialog->SetOptions(Flags));

            static constexpr COMDLG_FILTERSPEC SupportedFileTypes[] =
            {
                {
                    L"NanaBox Configuration File (*.7b)",
                    L"*.7b"
                }
            };
            winrt::check_hresult(FileDialog->SetFileTypes(
                ARRAYSIZE(SupportedFileTypes),
                SupportedFileTypes));
            winrt::check_hresult(FileDialog->SetFileTypeIndex(1));
            winrt::check_hresult(FileDialog->SetDefaultExtension(L"7b"));

            winrt::check_hresult(FileDialog->Show(nullptr));

            winrt::com_ptr<IShellItem> Result;
            winrt::check_hresult(FileDialog->GetResult(Result.put()));

            PWSTR FilePath = nullptr;
            winrt::check_hresult(Result->GetDisplayName(
                SIGDN_FILESYSPATH,
                &FilePath));
            g_ConfigurationFilePath = FilePath;
            ::CoTaskMemFree(FilePath);
        }
        catch (winrt::hresult_error const& ex)
        {
            if (ex.code() != ::HRESULT_FROM_WIN32(ERROR_CANCELLED))
            {
                ::TaskDialog(
                    nullptr,
                    nullptr,
                    g_WindowTitle.data(),
                    ex.message().c_str(),
                    nullptr,
                    TDCBF_OK_BUTTON,
                    TD_ERROR_ICON,
                    nullptr);
            }

            ::ExitProcess(ex.code());
        }
    }

    std::filesystem::current_path(
        g_ConfigurationFilePath.parent_path());

    winrt::com_ptr<winrt::NanaBox::implementation::App> app =
        winrt::make_self<winrt::NanaBox::implementation::App>();

    WTL::CMessageLoop MessageLoop;

    ATL::AtlAxWinInit();

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    std::unique_ptr<NanaBox::ConfigurationWindow> ConfigurationWindow = nullptr;
    std::unique_ptr<NanaBox::MainWindow> MainWindow = nullptr;

    if (ShowVirtualMachineConfig)
    {
        ConfigurationWindow = std::make_unique<NanaBox::ConfigurationWindow>();
        if (!ConfigurationWindow->Initialize(g_ConfigurationFilePath))
        {
            return -1;
        }

        //ConfigurationWindow->LoadConfiguration(g_ConfigurationFilePath);
        ConfigurationWindow->ShowWindow(nShowCmd);
        ConfigurationWindow->UpdateWindow();
    }
    else
    {
        MainWindow = std::make_unique<NanaBox::MainWindow>();
        if (!MainWindow->Create(
            nullptr,
            MainWindow->rcDefault,
            g_WindowTitle.data(),
            WS_OVERLAPPEDWINDOW))
        {
            return -1;
        }

        MainWindow->ShowWindow(nShowCmd);
        MainWindow->UpdateWindow();
    }

    int Result = MessageLoop.Run();

    g_Module.RemoveMessageLoop();
    g_Module.Term();

    app->Close();

    return Result;
}
