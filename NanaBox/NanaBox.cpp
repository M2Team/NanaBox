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
#include "ConfigurationManager.h"
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
#include "ExitConfirmationPage.h"

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <json.hpp>

#include <Mile.Project.Version.h>

#include "Utils.h"
#include "Environment.h"
#include "NanaBoxResources.h"

#include <Mile.Helpers.h>
#include <Mile.Xaml.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace winrt
{
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
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
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_SIZE(OnSize)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_ACTIVATE(OnActivate)
            MSG_WM_CLOSE(OnClose)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnCommand(
            UINT uNotifyCode,
            int nID,
            ATL::CWindow wndCtl);

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

        void OnClose();

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;
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

        HWND CreateXamlDialog();

        int ShowXamlDialog(
            _In_ HWND WindowHandle,
            _In_ int Width,
            _In_ int Height,
            _In_ LPVOID Content);
    };

}

int NanaBox::MainWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    if (this->DefWindowProcW() != 0)
    {
        return -1;
    }

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
    catch (...)
    {
        winrt::hresult_error Exception = Mile::WinRT::ToHResultError();
        std::thread([Exception]()
        {
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                Exception.message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
        }).join();
        return -1;
    }

    this->m_MainWindowControl =
        winrt::make<winrt::NanaBox::implementation::MainWindowControl>(
            this->m_hWnd);

    if (FAILED(::MileXamlSetXamlContentForContentWindow(
        this->m_hWnd,
        winrt::get_abi(this->m_MainWindowControl))))
    {
        return -1;
    }

    this->m_RdpClient->EnableAutoReconnect(false);
    this->m_RdpClient->RelativeMouseMode(true);
    this->m_RdpClient->AuthenticationServiceClass(
        L"Microsoft Virtual Console Service");

    this->m_RdpClient->AuthenticationLevel(0);
    this->m_RdpClient->EnableCredSspSupport(true);
    this->m_RdpClient->NegotiateSecurityLayer(false);

    NanaBox::RemoteDesktopUpdateKeyboardConfiguration(
        this->m_RdpClient,
        m_Configuration.Keyboard);

    NanaBox::RemoteDesktopUpdateEnhancedSessionConfiguration(
        this->m_RdpClient,
        m_Configuration.EnhancedSession);

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

void NanaBox::MainWindow::OnCommand(
    UINT uNotifyCode,
    int nID,
    ATL::CWindow wndCtl)
{
    UNREFERENCED_PARAMETER(uNotifyCode);
    UNREFERENCED_PARAMETER(wndCtl);

    switch (nID)
    {
    case NanaBox::MainWindowCommands::EnterBasicSession:
    {
        this->m_RdpClientMode = RdpClientMode::BasicSession;

        this->m_RdpClient->Disconnect();

        break;
    }
    case NanaBox::MainWindowCommands::EnterEnhancedSession:
    {
        this->m_RdpClientMode = RdpClientMode::EnhancedSession;

        this->m_RdpClient->Disconnect();

        break;
    }
    case NanaBox::MainWindowCommands::EnterFullScreen:
    {
        // If ZoomLevel doesn't equal 100, Basic Session Mode can't enter full
        // screen.
        if (this->m_RdpClientMode == NanaBox::RdpClientMode::BasicSession)
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

        break;
    }
    case NanaBox::MainWindowCommands::PauseVirtualMachine:
    {
        this->m_VirtualMachine->Pause();

        break;
    }
    case NanaBox::MainWindowCommands::ResumeVirtualMachine:
    {
        this->m_VirtualMachine->Resume();

        break;
    }
    case NanaBox::MainWindowCommands::RestartVirtualMachine:
    {
        this->m_VirtualMachineRestarting = true;
        this->m_VirtualMachine->Terminate();
        this->m_VirtualMachine = nullptr;

        this->InitializeVirtualMachine();
        this->m_RdpClient->Disconnect();

        break;
    }
    default:
        break;
    }
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

    winrt::DesktopWindowXamlSource XamlSource = nullptr;
    winrt::copy_from_abi(
        XamlSource,
        ::GetPropW(this->m_hWnd, L"XamlWindowSource"));
    if (!XamlSource)
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
        XamlSource.as<IDesktopWindowXamlSourceNative>();

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

void NanaBox::MainWindow::OnClose()
{
    if (!this->m_VirtualMachineRunning)
    {
        this->DestroyWindow();
        return;
    }

    HWND WindowHandle = this->CreateXamlDialog();
    if (!WindowHandle)
    {
        return;
    }

    winrt::NanaBox::ExitConfirmationPage Window =
        winrt::make< winrt::NanaBox::implementation::ExitConfirmationPage>(
            WindowHandle);
    this->ShowXamlDialog(WindowHandle, 480, 320, winrt::get_abi(Window));


    switch (Window.Status())
    {
    case winrt::NanaBox::ExitConfirmationStatus::Suspend:
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
            try
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
            catch (...)
            {

            }

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

        break;
    }
    case winrt::NanaBox::ExitConfirmationStatus::PowerOff:
    {
        this->m_VirtualMachine->Pause();
        this->m_VirtualMachine->Terminate();

        break;
    }
    default:
        break;
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

    NanaBox::ComputeSystemPrepareResourcesForNetworkAdapters(
        this->m_Configuration.Name,
        this->m_Configuration.NetworkAdapters);

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

    NanaBox::ComputeSystemUpdateGpu(
        this->m_VirtualMachine,
        this->m_Configuration.Gpu);

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

HWND NanaBox::MainWindow::CreateXamlDialog()
{
    return ::CreateWindowExW(
        WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME,
        L"Mile.Xaml.ContentWindow",
        nullptr,
        WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        this->m_hWnd,
        nullptr,
        nullptr,
        nullptr);
}

int NanaBox::MainWindow::ShowXamlDialog(
    _In_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
    _In_ LPVOID Content)
{
    if (!WindowHandle)
    {
        return -1;
    }

    if (FAILED(::MileAllowNonClientDefaultDrawingForWindow(
        WindowHandle,
        FALSE)))
    {
        return -1;
    }

    if (FAILED(::MileXamlSetXamlContentForContentWindow(
        WindowHandle,
        Content)))
    {
        return -1;
    }

    HMENU MenuHandle = ::GetSystemMenu(WindowHandle, FALSE);
    if (MenuHandle)
    {
        ::RemoveMenu(MenuHandle, 0, MF_SEPARATOR);
        ::RemoveMenu(MenuHandle, SC_RESTORE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_SIZE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_MINIMIZE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_MAXIMIZE, MF_BYCOMMAND);
    }

    UINT DpiValue = ::GetDpiForWindow(WindowHandle);

    int ScaledWidth = ::MulDiv(Width, DpiValue, USER_DEFAULT_SCREEN_DPI);
    int ScaledHeight = ::MulDiv(Height, DpiValue, USER_DEFAULT_SCREEN_DPI);

    RECT ParentWindowRect;
    ::GetWindowRect(this->m_hWnd, &ParentWindowRect);

    int ParentWidth = ParentWindowRect.right - ParentWindowRect.left;
    int ParentHeight = ParentWindowRect.bottom - ParentWindowRect.top;

    ::SetWindowPos(
        WindowHandle,
        nullptr,
        ParentWindowRect.left + ((ParentWidth - ScaledWidth) / 2),
        ParentWindowRect.top + ((ParentHeight - ScaledHeight) / 2),
        ScaledWidth,
        ScaledHeight,
        SWP_NOZORDER | SWP_NOACTIVATE);

    ::ShowWindow(WindowHandle, SW_SHOW);
    ::UpdateWindow(WindowHandle);

    ::EnableWindow(this->m_hWnd, FALSE);

    MSG Message;
    while (::GetMessageW(&Message, nullptr, 0, 0))
    {
        // Workaround for capturing Alt+F4 in applications with XAML Islands.
        // Reference: https://github.com/microsoft/microsoft-ui-xaml/issues/2408
        if (Message.message == WM_SYSKEYDOWN && Message.wParam == VK_F4)
        {
            ::SendMessageW(
                ::GetAncestor(Message.hwnd, GA_ROOT),
                Message.message,
                Message.wParam,
                Message.lParam);

            continue;
        }

        ::TranslateMessage(&Message);
        ::DispatchMessageW(&Message);
    }

    ::EnableWindow(this->m_hWnd, TRUE);
    ::SetActiveWindow(this->m_hWnd);

    return static_cast<int>(Message.wParam);
}

void PrerequisiteCheck()
{
    try
    {
        NanaBox::HcsGetServiceProperties();
    }
    catch (winrt::hresult_error const& ex)
    {
        winrt::hstring InstructionText;
        winrt::hstring ContentText;

        if (ex.code() == HCS_E_ACCESS_DENIED)
        {
            InstructionText = Mile::WinRT::GetLocalizedString(
                L"Messages/AccessDeniedInstructionText");
            ContentText = Mile::WinRT::GetLocalizedString(
                L"Messages/AccessDeniedContentText");
        }
        else if (ex.code() == HCS_E_SERVICE_NOT_AVAILABLE)
        {
            InstructionText = Mile::WinRT::GetLocalizedString(
                L"Messages/HyperVNotAvailableInstructionText");
            ContentText = Mile::WinRT::GetLocalizedString(
                L"Messages/HyperVNotAvailableContentText");
        }
        else
        {
            InstructionText = ex.message();
        }

        ::TaskDialog(
            nullptr,
            nullptr,
            g_WindowTitle.data(),
            InstructionText.c_str(),
            ContentText.c_str(),
            TDCBF_OK_BUTTON,
            TD_ERROR_ICON,
            nullptr);

        ::ExitProcess(ex.code());
    }

    SERVICE_STATUS_PROCESS ServiceStatus = { 0 };
    if (!::MileStartService(L"vmms", &ServiceStatus))
    {
        ::TaskDialog(
            nullptr,
            nullptr,
            g_WindowTitle.data(),
            Mile::WinRT::GetLocalizedString(
                L"Messages/VMMSNotAvailableInstructionText").c_str(),
            Mile::WinRT::GetLocalizedString(
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

    winrt::com_ptr<winrt::NanaBox::implementation::App> App =
        winrt::make_self<winrt::NanaBox::implementation::App>();
    auto ExitHandler = Mile::ScopeExitTaskHandler([&]()
    {
        App->Close();
    });

    std::wstring ApplicationName;
    std::map<std::wstring, std::wstring> OptionsAndParameters;
    std::wstring UnresolvedCommandLine;

    ::SplitCommandLineEx(
        std::wstring(GetCommandLineW()),
        std::vector<std::wstring>{ L"-", L"/", L"--" },
        std::vector<std::wstring>{ L"=", L":" },
        ApplicationName,
        OptionsAndParameters,
        UnresolvedCommandLine);

    bool PackagedMode = Mile::WinRT::IsPackagedMode();
    std::filesystem::path TargetBinaryPath;

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
        catch (...)
        {
            winrt::hresult_error Exception = Mile::WinRT::ToHResultError();
            ::TaskDialog(
                nullptr,
                nullptr,
                g_WindowTitle.data(),
                Exception.message().c_str(),
                nullptr,
                TDCBF_OK_BUTTON,
                TD_ERROR_ICON,
                nullptr);
            ::ExitProcess(Exception.code());
        }
    }

    if (!::MileIsCurrentProcessElevated() || PackagedMode)
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

            SHELLEXECUTEINFOW Information = { 0 };
            Information.cbSize = sizeof(SHELLEXECUTEINFOW);
            Information.fMask = SEE_MASK_NOCLOSEPROCESS;
            Information.lpVerb = L"runas";
            Information.nShow = nShowCmd;
            Information.lpFile = ApplicationName.c_str();
            Information.lpParameters = UnresolvedCommandLine.c_str();
            winrt::check_bool(::ShellExecuteExW(&Information));
            ::WaitForSingleObjectEx(Information.hProcess, INFINITE, FALSE);
            ::CloseHandle(Information.hProcess);

            if (PackagedMode && !TargetBinaryPath.empty())
            {
                std::filesystem::remove_all(TargetBinaryPath);
            }
        }
        catch (...)
        {
            winrt::hresult_error Exception = Mile::WinRT::ToHResultError();
            if (Exception.code() != ::HRESULT_FROM_WIN32(ERROR_CANCELLED))
            {
                ::TaskDialog(
                    nullptr,
                    nullptr,
                    g_WindowTitle.data(),
                    Exception.message().c_str(),
                    nullptr,
                    TDCBF_OK_BUTTON,
                    TD_ERROR_ICON,
                    nullptr);
            }
            ::ExitProcess(Exception.code());
        }

        ::ExitProcess(0);
    }

    ::PrerequisiteCheck();

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

    WTL::CMessageLoop MessageLoop;

    ATL::AtlAxWinInit();

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    NanaBox::MainWindow MainWindow;
    if (!MainWindow.Create(
        nullptr,
        MainWindow.rcDefault,
        g_WindowTitle.data(),
        WS_OVERLAPPEDWINDOW))
    {
        return -1;
    }

    MainWindow.ShowWindow(nShowCmd);
    MainWindow.UpdateWindow();

    int Result = MessageLoop.Run();

    g_Module.RemoveMessageLoop();
    g_Module.Term();

    return Result;
}
