#include "pch.h"

#include "MainWindow.h"
#include "ExitConfirmationPage.h"

#include <Mile.Xaml.h>

#include <ShlObj.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include "VirtualMachineConfiguration.h"
#include "Utils.h"

#include "NanaBoxResources.h"

namespace winrt
{
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
}

NanaBox::MainWindow::MainWindow(
    std::wstring const& ConfigurationFilePath) :
    m_ConfigurationFilePath(ConfigurationFilePath)
{

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
        ::ShowErrorMessageDialog(Mile::WinRT::ToHResultError());
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

    try
    {
        VARIANT Value;
        Value.vt = VT_BOOL;
        Value.boolVal = VARIANT_TRUE;
        this->m_RdpClient->Property(L"AllowAxToContainerEvents", Value);
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

        winrt::check_bool(this->GetWindowRect(
            &this->m_RememberedMainWindowRect));
        this->m_RememberedMainWindowStyle =
            this->GetWindowLongPtrW(GWL_STYLE);

        RECT FullScreenRect;
        FullScreenRect.left = 0;
        FullScreenRect.top = 0;
        FullScreenRect.right = dm.dmPelsWidth;
        FullScreenRect.bottom = dm.dmPelsHeight;

        // Resize RdpClientWindow and hidden XamlIslands by OnSize method
        // automatically calculate.
        m_RecommendedMainWindowControlHeight = 0;

        this->SetWindowLongPtrW(
            GWL_STYLE,
            WS_VISIBLE | WS_POPUP);
        // SetWindowPos will send WM_SIZE, RDP Zoom will be re-enabled in Basic
        // Session Mode.
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
    case NanaBox::MainWindowCommands::VirtualMachineSettings:
    {
        OPENASINFO OpenAsInfo = { 0 };
        OpenAsInfo.pcszFile = this->m_ConfigurationFilePath.c_str();
        OpenAsInfo.oaifInFlags = OAIF_EXEC;
        ::SHOpenWithDialog(this->m_hWnd, &OpenAsInfo);

        break;
    }
    case NanaBox::MainWindowCommands::ReloadVirtualMachineSettings:
    {
        break;
    }
    case NanaBox::MainWindowCommands::CreateVirtualHardDisk:
    {
        ::ShowNewVirtualHardDiskDialog(this->m_hWnd);

        break;
    }
    case NanaBox::MainWindowCommands::Documentation:
    {
        ::LaunchDocumentation();

        break;
    }
    case NanaBox::MainWindowCommands::About:
    {
        ::ShowAboutDialog(this->m_hWnd);

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

    HWND WindowHandle = ::CreateXamlDialog(this->m_hWnd);
    if (!WindowHandle)
    {
        return;
    }

    winrt::NanaBox::ExitConfirmationPage Window =
        winrt::make<winrt::NanaBox::implementation::ExitConfirmationPage>(
            WindowHandle);
    ::ShowXamlDialog(
        WindowHandle,
        480,
        320,
        winrt::get_abi(Window),
        this->m_hWnd);


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

        std::wstring SaveStateFile = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, this->m_Configuration.SaveStateFile));

        ::MileDeleteFileIgnoreReadonlyAttribute(SaveStateFile.c_str());

        nlohmann::json Options;
        Options["SaveType"] = "ToFile";
        Options["SaveStateFilePath"] = winrt::to_string(SaveStateFile.c_str());

        try
        {
            this->m_VirtualMachine->Save(winrt::to_hstring(Options.dump()));
        }
        catch (winrt::hresult_error const& ex)
        {
            ::MileDeleteFileIgnoreReadonlyAttribute(SaveStateFile.c_str());

            this->m_Configuration.SaveStateFile.clear();

            ::ShowErrorMessageDialog(ex);
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
                this->m_ConfigurationFilePath,
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
        this->m_ConfigurationFilePath);

    this->m_Configuration = NanaBox::DeserializeConfiguration(
        ConfigurationFileContent);

    for (NanaBox::ScsiDeviceConfiguration& ScsiDevice
        : this->m_Configuration.ScsiDevices)
    {
        if (ScsiDevice.Type == NanaBox::ScsiDeviceType::PhysicalDevice)
        {
            break;
        }

        std::wstring Path = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, ScsiDevice.Path));
        if (::PathFileExistsW(Path.c_str()))
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
        std::wstring GuestStateFile = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, this->m_Configuration.GuestStateFile));
        if (!::PathFileExistsW(GuestStateFile.c_str()))
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
        std::wstring RuntimeStateFile = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, this->m_Configuration.RuntimeStateFile));
        if (!::PathFileExistsW(RuntimeStateFile.c_str()))
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
        std::wstring SaveStateFile = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, this->m_Configuration.SaveStateFile));
        if (::PathFileExistsW(SaveStateFile.c_str()))
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
        std::wstring SaveStateFile = ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8, this->m_Configuration.SaveStateFile));

        ::MileDeleteFileIgnoreReadonlyAttribute(SaveStateFile.c_str());

        this->m_Configuration.SaveStateFile.clear();
    }

    ConfigurationFileContent =
        NanaBox::SerializeConfiguration(this->m_Configuration);
    ::WriteAllTextToUtf8TextFile(
        this->m_ConfigurationFilePath,
        ConfigurationFileContent);

    nlohmann::json Properties = nlohmann::json::parse(
        winrt::to_string(this->m_VirtualMachine->GetProperties()));
    this->m_VirtualMachineGuid = Properties["RuntimeId"];

    std::wstring WindowTitle = Mile::FormatWideString(
        L"%s - NanaBox",
        Mile::ToWideString(CP_UTF8, this->m_Configuration.Name).c_str());
    this->SetWindowTextW(WindowTitle.c_str());
    this->m_RdpClient->ConnectionBarText(WindowTitle.c_str());
}
