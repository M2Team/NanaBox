#include "pch.h"

#include "MainWindow.h"
#include "ExitConfirmationPage.h"
#include "ReloadConfirmationPage.h"

#include <Mile.Xaml.h>

#include <ShlObj.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include "Utils.h"

#include <map>

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

    this->m_MainWindowControl =
        winrt::make<winrt::NanaBox::implementation::MainWindowControl>(
            this->m_hWnd);

    if (FAILED(::MileXamlSetXamlContentForContentWindow(
        this->m_hWnd,
        winrt::get_abi(this->m_MainWindowControl))))
    {
        return -1;
    }

    try
    {
        this->InitializeVirtualMachine();

        winrt::check_hresult(::RDPBASE_CreateInstance(
            nullptr,
            CLSID_RDPRuntimeSTAContext,
            IID_IRDPENCPlatformContext,
            this->m_PlatformContext.put_void()));
        winrt::check_hresult(this->m_PlatformContext->InitializeInstance());
        this->m_RdpNamedPipeCallbacks.attach(
            new NanaBox::RdpNamedPipeCallbacks(this));

        this->RdpClientInitialize();
    }
    catch (...)
    {
        ::ShowErrorMessageDialog(Mile::WinRT::ToHResultError());
        return -1;
    }

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
        this->m_NeedRdpClientModeChange = true;

        this->m_RdpClient->Disconnect();

        break;
    }
    case NanaBox::MainWindowCommands::EnterEnhancedSession:
    {
        this->m_RdpClientMode = RdpClientMode::EnhancedSession;
        this->m_NeedRdpClientModeChange = true;

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

        break;
    }
    case NanaBox::MainWindowCommands::VirtualMachineSettings:
    {
        winrt::handle(Mile::CreateThread([=]()
        {
            winrt::hstring SettingsInstructionText =
                Mile::WinRT::GetLocalizedString(
                    L"MainWindow/SettingsInstructionText");
            winrt::hstring SettingsContentText =
                Mile::WinRT::GetLocalizedString(
                    L"MainWindow/SettingsContentText");

            ::ShowMessageDialog(
                this->m_hWnd,
                SettingsInstructionText.c_str(),
                SettingsContentText.c_str());

            OPENASINFO OpenAsInfo = { 0 };
            OpenAsInfo.pcszFile = this->m_ConfigurationFilePath.c_str();
            OpenAsInfo.oaifInFlags = OAIF_EXEC;
            ::SHOpenWithDialog(this->m_hWnd, &OpenAsInfo);
        }));

        break;
    }
    case NanaBox::MainWindowCommands::ReloadVirtualMachineSettings:
    {
        HWND WindowHandle = ::CreateXamlDialog(this->m_hWnd);
        if (!WindowHandle)
        {
            return;
        }

        winrt::NanaBox::ReloadConfirmationPage Window =
            winrt::make<winrt::NanaBox::implementation::ReloadConfirmationPage>(
                WindowHandle);
        ::ShowXamlDialog(
            WindowHandle,
            480,
            320,
            winrt::get_abi(Window),
            this->m_hWnd);

        if (winrt::NanaBox::ReloadConfirmationStatus::Reload == Window.Status())
        {
            try
            {
                this->TryReloadVirtualMachine();
            }
            catch (...)
            {

            }
        }

        break;
    }
    case NanaBox::MainWindowCommands::CreateVirtualHardDisk:
    {
        ::ShowNewVirtualHardDiskDialog(this->m_hWnd);

        break;
    }
    case NanaBox::MainWindowCommands::ResizeVirtualHardDisk:
    {
        ::ShowResizeVirtualHardDiskDialog(this->m_hWnd);

        break;
    }
    case NanaBox::MainWindowCommands::CompactVirtualHardDisk:
    {
        ::ShowCompactVirtualHardDiskDialog(this->m_hWnd);

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

    if (this->m_RdpClient->Connected() &&
        RdpClientMode::BasicSession == this->m_RdpClientMode)
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
        // Temporarily disable the GPU-PV settings because virtual machine
        // doen't support save the state when GPU-PV is enabled.
        NanaBox::GpuConfiguration Gpu;
        Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
        NanaBox::ComputeSystemUpdateGpu(this->m_VirtualMachine, Gpu);

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

            NanaBox::ComputeSystemUpdateGpu(
                this->m_VirtualMachine,
                this->m_Configuration.Gpu);
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
    // Just kill the process directly for reducing the Windows error reports.
    ::ExitProcess(0);
}

BOOL NanaBox::MainWindow::OnQueryEndSession(
    UINT nSource,
    UINT uLogOff)
{
    UNREFERENCED_PARAMETER(nSource);
    UNREFERENCED_PARAMETER(uLogOff);

    ::ShutdownBlockReasonCreate(
        this->m_hWnd,
        Mile::WinRT::GetLocalizedString(
            L"ExitConfirmationPage/GridTitleTextBlock/Text").c_str());

    // Notify the OS to prevent shut down because shut down automatically
    // without exiting NanaBox may corrupt the user's virtual machine.
    return FALSE;
}

void NanaBox::MainWindow::InitializeVirtualMachine()
{
    std::string ConfigurationFileContent = ::ReadAllTextFromUtf8TextFile(
        this->m_ConfigurationFilePath);

    this->m_Configuration = NanaBox::DeserializeConfiguration(
        ConfigurationFileContent);

    {
        bool VirtualMachineExisted = true;
        try
        {
            winrt::make_self<NanaBox::ComputeSystem>(
                winrt::to_hstring(
                    this->m_Configuration.Name));
        }
        catch (...)
        {
            VirtualMachineExisted = false;
        }

        if (VirtualMachineExisted)
        {
            winrt::throw_hresult(HCS_E_SYSTEM_ALREADY_EXISTS);
        }
    }

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

    if (!this->m_Configuration.NetworkAdapters.empty())
    {
        for (NanaBox::NetworkAdapterConfiguration& NetworkAdapter
            : this->m_Configuration.NetworkAdapters)
        {
            NanaBox::ComputeNetworkDeleteEndpoint(NetworkAdapter);
            if (NetworkAdapter.Connected)
            {
                try
                {
                    NanaBox::ComputeNetworkCreateEndpoint(
                        this->m_Configuration.Name,
                        NetworkAdapter);
                }
                catch (...)
                {

                }
            }
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
            this->m_RdpClient->Disconnect();
            return;
        }

        this->m_VirtualMachineRunning = false;
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

    this->m_WindowTitle = Mile::FormatWideString(
        L"%s - NanaBox",
        Mile::ToWideString(CP_UTF8, this->m_Configuration.Name).c_str());
    this->SetWindowTextW(this->m_WindowTitle.c_str());
}

void NanaBox::MainWindow::TryReloadVirtualMachine()
{
    std::string ConfigurationFileContent =
        ::ReadAllTextFromUtf8TextFile(this->m_ConfigurationFilePath);

    NanaBox::VirtualMachineConfiguration Configuration =
        NanaBox::DeserializeConfiguration(ConfigurationFileContent);

    if (this->m_Configuration.MemorySize != Configuration.MemorySize)
    {
        try
        {
            NanaBox::ComputeSystemUpdateMemorySize(
                this->m_VirtualMachine,
                Configuration.MemorySize);
            this->m_Configuration.MemorySize =
                Configuration.MemorySize;
        }
        catch (...)
        {

        }
    }

    if (0 != _stricmp(
        this->m_Configuration.ComPorts.ComPort1.c_str(),
        Configuration.ComPorts.ComPort1.c_str()))
    {
        try
        {
            if (this->m_Configuration.ComPorts.ComPort1.empty())
            {
                NanaBox::ComputeSystemAddComPort(
                    this->m_VirtualMachine,
                    0,
                    Configuration.ComPorts.ComPort1);
            }
            else if (Configuration.ComPorts.ComPort1.empty())
            {
                NanaBox::ComputeSystemRemoveComPort(
                    this->m_VirtualMachine,
                    0,
                    Configuration.ComPorts.ComPort1);
            }
            else
            {
                NanaBox::ComputeSystemUpdateComPort(
                    this->m_VirtualMachine,
                    0,
                    Configuration.ComPorts.ComPort1);
            }

            this->m_Configuration.ComPorts.ComPort1 =
                Configuration.ComPorts.ComPort1;
        }
        catch (...)
        {

        }
    }

    if (0 != _stricmp(
        this->m_Configuration.ComPorts.ComPort2.c_str(),
        Configuration.ComPorts.ComPort2.c_str()))
    {
        try
        {
            if (this->m_Configuration.ComPorts.ComPort2.empty())
            {
                NanaBox::ComputeSystemAddComPort(
                    this->m_VirtualMachine,
                    1,
                    Configuration.ComPorts.ComPort2);
            }
            else if (Configuration.ComPorts.ComPort2.empty())
            {
                NanaBox::ComputeSystemRemoveComPort(
                    this->m_VirtualMachine,
                    1,
                    Configuration.ComPorts.ComPort2);
            }
            else
            {
                NanaBox::ComputeSystemUpdateComPort(
                    this->m_VirtualMachine,
                    1,
                    Configuration.ComPorts.ComPort2);
            }

            this->m_Configuration.ComPorts.ComPort2 =
                Configuration.ComPorts.ComPort2;
        }
        catch (...)
        {

        }
    }

    try
    {
        NanaBox::ComputeSystemUpdateGpu(
            this->m_VirtualMachine,
            Configuration.Gpu);
        this->m_Configuration.Gpu = Configuration.Gpu;
    }
    catch (...)
    {

    }

    {
        using NetworkAdapterMapType =
            std::map<std::string, NanaBox::NetworkAdapterConfiguration>;

        NetworkAdapterMapType PreviousList;
        NetworkAdapterMapType CurrentList;
        std::vector<NanaBox::NetworkAdapterConfiguration> KeepList;
        std::vector<NanaBox::NetworkAdapterConfiguration> AddList;
        std::vector<NanaBox::NetworkAdapterConfiguration> RemoveList;
        std::vector<NanaBox::NetworkAdapterConfiguration> FinalList;

        for (NanaBox::NetworkAdapterConfiguration& Previous
            : this->m_Configuration.NetworkAdapters)
        {
            PreviousList.insert(std::pair(Previous.EndpointId, Previous));
        }

        for (NanaBox::NetworkAdapterConfiguration& Current
            : Configuration.NetworkAdapters)
        {
            CurrentList.insert(std::pair(Current.EndpointId, Current));
        }

        for (NanaBox::NetworkAdapterConfiguration& Previous
            : this->m_Configuration.NetworkAdapters)
        {
            NetworkAdapterMapType::iterator Current =
                CurrentList.find(Previous.EndpointId);

            if (CurrentList.end() == Current)
            {
                RemoveList.push_back(Previous);
            }
            else
            {
                if (Previous.Connected != Current->second.Connected ||
                    0 != _stricmp(
                        Previous.MacAddress.c_str(),
                        Current->second.MacAddress.c_str()))
                {
                    if (Previous.Connected)
                    {
                        RemoveList.push_back(Previous);
                    }
                    AddList.push_back(Current->second);
                }
                else
                {
                    KeepList.push_back(Previous);
                }
            }
        }

        for (NanaBox::NetworkAdapterConfiguration& Current
            : Configuration.NetworkAdapters)
        {
            if (PreviousList.end() == PreviousList.find(Current.EndpointId))
            {
                AddList.push_back(Current);
            }
        }

        for (NanaBox::NetworkAdapterConfiguration& Current : KeepList)
        {
            FinalList.push_back(Current);
        }

        for (NanaBox::NetworkAdapterConfiguration& Current : RemoveList)
        {
            try
            {
                NanaBox::ComputeSystemRemoveNetworkAdapter(
                    this->m_VirtualMachine,
                    Current);
                NanaBox::ComputeNetworkDeleteEndpoint(Current);
            }
            catch (...)
            {
                FinalList.push_back(Current);
            }
        }

        for (NanaBox::NetworkAdapterConfiguration& Current : AddList)
        {
            try
            {
                NanaBox::ComputeNetworkDeleteEndpoint(Current);
                if (Current.Connected)
                {
                    NanaBox::ComputeNetworkCreateEndpoint(
                        this->m_Configuration.Name,
                        Current);
                    NanaBox::ComputeSystemAddNetworkAdapter(
                        this->m_VirtualMachine,
                        Current);
                }
                FinalList.push_back(Current);
            }
            catch (...)
            {

            }
        }

        this->m_Configuration.NetworkAdapters = FinalList;
    }

    {
        size_t PreviousCount = this->m_Configuration.ScsiDevices.size();
        size_t NewCount = Configuration.ScsiDevices.size();
        if (PreviousCount <= NewCount)
        {
            for (std::uint32_t i = 0; i < NewCount; ++i)
            {
                NanaBox::ScsiDeviceConfiguration& Current =
                    Configuration.ScsiDevices[i];

                if (i < PreviousCount)
                {
                    NanaBox::ScsiDeviceConfiguration& Previous =
                        this->m_Configuration.ScsiDevices[i];

                    if (Previous.Type == Current.Type)
                    {
                        if (0 != _stricmp(
                            Previous.Path.c_str(),
                            Current.Path.c_str()))
                        {
                            try
                            {
                                NanaBox::ComputeSystemUpdateScsiDevice(
                                    this->m_VirtualMachine,
                                    i,
                                    Current);
                                Previous.Path = Current.Path;
                            }
                            catch (...)
                            {

                            }
                        }
                    }
                }
                else
                {
                    try
                    {
                        if (Current.Type !=
                            NanaBox::ScsiDeviceType::PhysicalDevice)
                        {
                            std::wstring Path = ::GetAbsolutePath(
                                Mile::ToWideString(CP_UTF8, Current.Path));
                            if (!::PathFileExistsW(Path.c_str()))
                            {
                                continue;
                            }
                            winrt::check_hresult(::HcsGrantVmAccess(
                                winrt::to_hstring(
                                    this->m_Configuration.Name).c_str(),
                                Path.c_str()));
                        }

                        NanaBox::ComputeSystemAddScsiDevice(
                            this->m_VirtualMachine,
                            i,
                            Current);
                        this->m_Configuration.ScsiDevices.push_back(Current);
                    }
                    catch (...)
                    {

                    }
                }
            }
        }
    }

    try
    {
        NanaBox::RemoteDesktopUpdateKeyboardConfiguration(
            this->m_RdpClient,
            Configuration.Keyboard);
        this->m_Configuration.Keyboard = Configuration.Keyboard;
    }
    catch (...)
    {

    }

    try
    {
        NanaBox::RemoteDesktopUpdateEnhancedSessionConfiguration(
            this->m_RdpClient,
            Configuration.EnhancedSession);
        this->m_Configuration.EnhancedSession = Configuration.EnhancedSession;
    }
    catch (...)
    {

    }

    ConfigurationFileContent =
        NanaBox::SerializeConfiguration(this->m_Configuration);
    ::WriteAllTextToUtf8TextFile(
        this->m_ConfigurationFilePath,
        ConfigurationFileContent);

    this->m_NeedRdpClientModeChange = true;
    this->m_RdpClient->Disconnect();
}

void NanaBox::MainWindow::RdpClientOnRemoteDesktopSizeChange(
    _In_ LONG Width,
    _In_ LONG Height)
{
    if (this->m_RdpClientMode == RdpClientMode::BasicSession)
    {
        this->m_DisplayResolution = CSize(Width, Height);

        if (this->IsZoomed())
        {
            VARIANT RawZoomLevel;
            RawZoomLevel.vt = VT_UI4;
            RawZoomLevel.uintVal = this->m_RecommendedZoomLevel;
            this->m_RdpClient->Property(
                L"ZoomLevel",
                RawZoomLevel);
            return;
        }

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
}

void NanaBox::MainWindow::RdpClientOnLoginComplete()
{
    this->m_connectionOnceSuccessful = true;
    if (this->m_RdpClientMode == RdpClientMode::EnhancedSession)
    {
        this->m_RdpClientMode = RdpClientMode::EnhancedVideoSyncedSession;
        this->m_DisplayResolution = CSize();
    }
}

void NanaBox::MainWindow::RdpClientOnDisconnected(
    _In_ LONG DisconnectReason)
{
    this->m_MouseCaptureMode = false;
    ::ClipCursor(nullptr);

    this->m_RdpNamedPipe->TerminateInstance();
    this->m_RdpNamedPipe = nullptr;

    if (this->m_RdpClientMode == RdpClientMode::EnhancedVideoSyncedSession)
    {
        this->m_RdpClientMode = RdpClientMode::EnhancedSession;
    }

    if (this->m_VirtualMachineRunning)
    {
        if (DisconnectReason == 4 && !this->m_connectionOnceSuccessful)
        {
            ::ShowMessageDialog(
                nullptr,
                Mile::WinRT::GetLocalizedString(
                    L"Messages/VirtualMachineConnectionFailedInstructionText"),
                Mile::WinRT::GetLocalizedString(
                    L"Messages/VirtualMachineConnectionFailedContentText"));

            this->m_VirtualMachine->Terminate();
        }
        else if (this->m_NeedRdpClientModeChange)
        {
            try
            {
                this->RdpClientUninitialize();
                this->RdpClientInitialize();

                // Ensure the UI layout will be refreshed via sending WM_SIZE
                // message directly. It's OK because we don't use wParam and
                // lParam in WM_SIZE message.
                this->SendMessageW(WM_SIZE);
            }
            catch (...)
            {

            }

            this->m_NeedRdpClientModeChange = false;
        }
        else
        {
            try
            {
                this->RdpClientConnect();
            }
            catch (...)
            {

            }
        }
    }
}

void NanaBox::MainWindow::RdpClientOnRequestGoFullScreen()
{
    HMONITOR MonitorHandle = ::MonitorFromWindow(
        this->m_hWnd,
        MONITOR_DEFAULTTONULL);
    winrt::check_pointer(MonitorHandle);
    MONITORINFOEX MonitorInfo;
    MonitorInfo.cbSize = sizeof(MONITORINFOEX);
    winrt::check_bool(::GetMonitorInfoW(
        MonitorHandle,
        &MonitorInfo));
    DEVMODE DevMode;
    DevMode.dmSize = sizeof(DEVMODE);
    winrt::check_bool(::EnumDisplaySettingsW(
        MonitorInfo.szDevice,
        ENUM_CURRENT_SETTINGS,
        &DevMode));

    winrt::check_bool(this->GetWindowRect(
        &this->m_RememberedMainWindowRect));
    this->m_RememberedMainWindowStyle =
        this->GetWindowLongPtrW(GWL_STYLE);

    RECT FullScreenRect;
    FullScreenRect.left = 0;
    FullScreenRect.top = 0;
    FullScreenRect.right = DevMode.dmPelsWidth;
    FullScreenRect.bottom = DevMode.dmPelsHeight;

    // Resize RDP client window and hide the XAML control window by calculating
    // in OnSize method automatically.
    this->m_RecommendedMainWindowControlHeight = 0;

    this->SetWindowLongPtrW(
        GWL_STYLE,
        WS_VISIBLE | WS_POPUP);
    // SetWindowPos will send WM_SIZE, RDP Zooming will be re-enabled in Basic
    // Session Mode.
    this->SetWindowPos(
        HWND_TOP,
        &FullScreenRect,
        SWP_FRAMECHANGED | SWP_NOACTIVATE);
}

void NanaBox::MainWindow::RdpClientOnRequestLeaveFullScreen()
{
    this->m_RecommendedMainWindowControlHeight =
        this->m_MainWindowControlHeight;
    this->SetWindowLongPtrW(
        GWL_STYLE,
        this->m_RememberedMainWindowStyle);
    this->SetWindowPos(
        nullptr,
        &this->m_RememberedMainWindowRect,
        SWP_NOACTIVATE);
}

void NanaBox::MainWindow::RdpClientOnRequestContainerMinimize()
{
    this->ShowWindow(SW_MINIMIZE);
}

void NanaBox::MainWindow::RdpClientOnConfirmClose(
    _Out_ VARIANT_BOOL* pfAllowClose)
{
    // Set it FALSE because we don't want to close the connection before making
    // decisions in the close confirmation dialog.
    *pfAllowClose = VARIANT_FALSE;
    this->PostMessageW(WM_CLOSE);
}

void NanaBox::MainWindow::RdpClientOnFocusReleased(
    _In_ INT Direction)
{
    UNREFERENCED_PARAMETER(Direction);

    this->m_MouseCaptureMode = !this->m_MouseCaptureMode;

    if (this->m_MouseCaptureMode)
    {
        RECT WindowRect;
        if (this->m_RdpClientWindow.GetWindowRect(&WindowRect))
        {
            ::ClipCursor(&WindowRect);
        }
    }
    else
    {
        ::ClipCursor(nullptr);
    }
}

void NanaBox::MainWindow::RdpClientInitialize()
{
    this->m_RdpClient = winrt::make_self<NanaBox::RdpClient>();

    this->m_RdpClient->EnableEventsDispatcher();

    winrt::check_pointer(this->m_RdpClientWindow.Create(
        this->m_hWnd,
        this->m_RdpClientWindow.rcDefault,
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN));

    winrt::check_hresult(this->m_RdpClientWindow.AttachControl(
        this->m_RdpClient->RawControl().get(),
        nullptr));

    this->m_RdpClient->ConnectionBarText(this->m_WindowTitle.c_str());

    this->m_RdpClient->EnableAutoReconnect(false);
    this->m_RdpClient->RelativeMouseMode(true);

    this->m_RdpClient->AuthenticationLevel(0);
    this->m_RdpClient->EnableCredSspSupport(false);
    this->m_RdpClient->NegotiateSecurityLayer(true);

    NanaBox::RemoteDesktopUpdateKeyboardConfiguration(
        this->m_RdpClient,
        this->m_Configuration.Keyboard);

    NanaBox::RemoteDesktopUpdateEnhancedSessionConfiguration(
        this->m_RdpClient,
        this->m_Configuration.EnhancedSession);

    this->m_RdpClient->ContainerHandledFullScreen(true);

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

    try
    {
        VARIANT Value;
        Value.vt = VT_BOOL;
        Value.boolVal = VARIANT_TRUE;
        this->m_RdpClient->Property(L"AllowRelativeMouseMode", Value);
    }
    catch (...)
    {

    }

    this->m_RdpClient->GrabFocusOnConnect(false);

    this->m_RdpClient->AllowPromptingForCredentials(false);

    this->m_RdpClient->OnRemoteDesktopSizeChange.add(
        { this, &NanaBox::MainWindow::RdpClientOnRemoteDesktopSizeChange });
    this->m_RdpClient->OnLoginComplete.add(
        { this, &NanaBox::MainWindow::RdpClientOnLoginComplete });
    this->m_RdpClient->OnDisconnected.add(
        { this, &NanaBox::MainWindow::RdpClientOnDisconnected });
    this->m_RdpClient->OnRequestGoFullScreen.add(
        { this, &NanaBox::MainWindow::RdpClientOnRequestGoFullScreen });
    this->m_RdpClient->OnRequestLeaveFullScreen.add(
        { this, &NanaBox::MainWindow::RdpClientOnRequestLeaveFullScreen });
    this->m_RdpClient->OnRequestContainerMinimize.add(
        { this, &NanaBox::MainWindow::RdpClientOnRequestContainerMinimize });
    this->m_RdpClient->OnConfirmClose.add(
        { this, &NanaBox::MainWindow::RdpClientOnConfirmClose });
    this->m_RdpClient->OnFocusReleased.add(
        { this, &NanaBox::MainWindow::RdpClientOnFocusReleased });

    this->SetTimer(
        NanaBox::MainWindowTimerEvents::SyncDisplaySettings,
        200);

    this->m_RdpClient->MinInputSendInterval(20);

    if (this->m_RdpClientMode == RdpClientMode::EnhancedSession)
    {
        this->m_RdpClient->DesktopWidth(
            this->m_RecommendedDisplayResolution.cx);
        this->m_RdpClient->DesktopHeight(
            this->m_RecommendedDisplayResolution.cy);

        try
        {
            VARIANT Value;
            Value.vt = VT_UI4;
            Value.uintVal = ::MulDiv(
                100,
                ::GetDpiForWindow(this->m_hWnd),
                USER_DEFAULT_SCREEN_DPI);
            this->m_RdpClient->Property(L"DesktopScaleFactor", Value);
        }
        catch (...)
        {

        }

        try
        {
            VARIANT Value;
            Value.vt = VT_UI4;
            Value.uintVal = 100;
            this->m_RdpClient->Property(L"DeviceScaleFactor", Value);
        }
        catch (...)
        {

        }
    }

    this->m_RdpClient->MarkRdpSettingsSecure(true);

    this->RdpClientConnect();
}

void NanaBox::MainWindow::RdpClientUninitialize()
{
    this->KillTimer(
        NanaBox::MainWindowTimerEvents::SyncDisplaySettings);
    this->m_RdpClient->DisableEventsDispatcher();
    this->m_RdpClientWindow.DestroyWindow();
    this->m_RdpClient = nullptr;
}

void NanaBox::MainWindow::RdpClientConnect()
{
    winrt::check_hresult(::RDPBASE_CreateInstance(
        this->m_PlatformContext.get(),
        CLSID_RDPENCNamedPipeDirectConnector,
        IID_IRDPENCNamedPipeDirectConnector,
        this->m_RdpNamedPipe.put_void()));
    winrt::check_hresult(this->m_RdpNamedPipe->InitializeInstance(
        this->m_RdpNamedPipeCallbacks.get()));
    winrt::check_hresult(this->m_RdpNamedPipe->StartConnect(
        Mile::ToWideString(
            CP_UTF8,
            Mile::FormatString(
                this->m_RdpClientMode == RdpClientMode::EnhancedSession
                ? "\\\\.\\pipe\\%s.EnhancedSession"
                : "\\\\.\\pipe\\%s.BasicSession",
                this->m_Configuration.Name.c_str())).c_str()));
}

NanaBox::RdpNamedPipeCallbacks::RdpNamedPipeCallbacks(
    _In_ MainWindow* Instance)
    : m_Instance(Instance)
{

}

void STDMETHODCALLTYPE NanaBox::RdpNamedPipeCallbacks::OnConnectionCompleted(
    _In_ IUnknown* pNetStream)
{
    this->m_Instance->m_RdpClient->ConnectWithEndpoint(pNetStream);

    this->m_Instance->m_RdpClient->Connect();

    this->m_Instance->m_RdpClientWindow.SetFocus();
}

void STDMETHODCALLTYPE NanaBox::RdpNamedPipeCallbacks::OnConnectorError(
    _In_ HRESULT hrError)
{
    UNREFERENCED_PARAMETER(hrError);

    this->m_Instance->RdpClientOnDisconnected(
        exDiscReasonServerDeniedConnection);
}
