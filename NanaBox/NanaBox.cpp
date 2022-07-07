/*
 * PROJECT:   NanaBox
 * FILE:      NanaBox.cpp
 * PURPOSE:   Implementation for NanaBox
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "RdpClient.h"
#include "HostCompute.h"
#include "VirtualMachineConfiguration.h"

#include <cwchar>

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

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <winrt/Windows.Data.Json.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "NanaBoxResources.h"

HRESULT DwmWindowSetUseImmersiveDarkModeAttribute(
    _In_ HWND WindowHandle,
    _In_ BOOL Value)
{
    static bool IsWindows10Version20H1OrLater = ([]() -> bool
    {
        OSVERSIONINFOEXW OSVersionInfoEx = { 0 };
        OSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        OSVersionInfoEx.dwMajorVersion = 10;
        OSVersionInfoEx.dwMinorVersion = 0;
        OSVersionInfoEx.dwBuildNumber = 19041;
        return ::VerifyVersionInfoW(
            &OSVersionInfoEx,
            VER_BUILDNUMBER,
            ::VerSetConditionMask(
                ::VerSetConditionMask(
                    ::VerSetConditionMask(
                        0,
                        VER_MAJORVERSION,
                        VER_GREATER_EQUAL),
                    VER_MINORVERSION,
                    VER_GREATER_EQUAL),
                VER_BUILDNUMBER,
                VER_GREATER_EQUAL));
    }());

    const DWORD DwmWindowUseImmersiveDarkModeBefore20H1Attribute = 19;
    const DWORD DwmWindowUseImmersiveDarkModeAttribute = 20;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        (IsWindows10Version20H1OrLater
            ? DwmWindowUseImmersiveDarkModeAttribute
            : DwmWindowUseImmersiveDarkModeBefore20H1Attribute),
        &Value,
        sizeof(BOOL));
}

HRESULT DwmWindowSetCaptionColorAttribute(
    _In_ HWND WindowHandle,
    _In_ COLORREF Value)
{
    const DWORD DwmWindowCaptionColorAttribute = 35;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        DwmWindowCaptionColorAttribute,
        &Value,
        sizeof(COLORREF));
}

HRESULT DwmWindowDisableSystemBackdrop(
    _In_ HWND WindowHandle)
{
    const DWORD DwmWindowSystemBackdropTypeAttribute = 38;
    const DWORD DwmWindowSystemBackdropTypeNone = 1;
    DWORD Value = DwmWindowSystemBackdropTypeNone;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        DwmWindowSystemBackdropTypeAttribute,
        &Value,
        sizeof(DWORD));
}

bool ShouldAppsUseImmersiveDarkMode()
{
    DWORD Type = REG_DWORD;
    DWORD Value = 0;
    DWORD ValueLength = sizeof(DWORD);
    ::RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD | RRF_SUBKEY_WOW6464KEY,
        &Type,
        &Value,
        &ValueLength);
    return (Type == REG_DWORD && ValueLength == sizeof(DWORD) && Value == 0);
}

namespace winrt
{
    using Windows::Data::Json::IJsonValue;
    using Windows::Data::Json::JsonArray;
    using Windows::Data::Json::JsonObject;
    using Windows::Data::Json::JsonValue;
    using Windows::UI::Xaml::ElementTheme;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::UIElement;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
    using Windows::UI::Xaml::Media::VisualTreeHelper;
}

namespace
{
    winrt::hstring g_VMID = L"48781dff-90cc-4650-89c3-fe12e6210b19";
    bool volatile g_VirtualMachineRunning = false;
    bool volatile g_RdpClientEnhancedMode = false;
}

namespace NanaBox
{
    class MainWindow : public ATL::CWindowImpl<MainWindow>
    {
    public:

        DECLARE_WND_CLASS(L"NanaBoxMainWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DPICHANGED(OnDpiChanged)
            MSG_WM_MENUCHAR(OnMenuChar)
            MSG_WM_SETTINGCHANGE(OnSettingChange)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnSize(
            UINT nType,
            CSize size);

        void OnDpiChanged(
            UINT nDpiX,
            UINT nDpiY,
            PRECT pRect);

        LRESULT OnMenuChar(
            UINT nChar,
            UINT nFlags,
            WTL::CMenuHandle menu);

        void OnSettingChange(
            UINT uFlags,
            LPCTSTR lpszSection);

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;
        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaBox::MainWindowControl m_MainWindowControl;
    };

}

namespace NanaBox
{
    struct GpuConfiguration
    {
        GpuAssignmentMode AssignmentMode;
        std::vector<winrt::hstring> SelectedDevices;
    };

    struct ScsiDeviceConfiguration
    {
        bool Enabled;
        ScsiDeviceType Type;
        winrt::hstring Path;
    };

    struct NetworkAdapterConfiguration
    {
        bool Enabled;
        bool Connected;
        winrt::hstring MacAddress;
    };

    struct SharedFolderConfiguration
    {
        bool Enabled;
        bool ReadOnly;
        winrt::hstring HostPath;
        winrt::hstring GuestName;
    };

    struct VirtualMachineConfiguration
    {      
        std::uint32_t Version;
        GuestType GuestType;
        winrt::hstring Name;
        std::uint32_t ProcessorCount;
        std::uint64_t MemorySize;
        std::vector<winrt::hstring> ComPorts;
        GpuConfiguration Gpu;  
        std::vector<NetworkAdapterConfiguration> NetworkAdapters;
        std::vector<ScsiDeviceConfiguration> ScsiDevices;
        std::vector<SharedFolderConfiguration> SharedFolders;
    };

    VirtualMachineConfiguration ParseVirtualMachineConfiguration(
        winrt::hstring const& Configuration);

    winrt::hstring GenerateComputeSystemConfiguration(
        VirtualMachineConfiguration const& Configuration);
}

NanaBox::VirtualMachineConfiguration NanaBox::ParseVirtualMachineConfiguration(
    winrt::hstring const& Configuration)
{
    winrt::JsonObject ParsedJsonObject = nullptr;
    if (!winrt::JsonObject::TryParse(
        Configuration,
        ParsedJsonObject))
    {
        throw winrt::hresult_invalid_argument(
            L"Invalid JSON");
    }

    winrt::JsonObject RootJsonObject =
        ParsedJsonObject.GetNamedObject(
            L"NanaBox",
            nullptr);
    if (!RootJsonObject)
    {
        throw winrt::hresult_invalid_argument(
            L"Invalid Configuration");
    }

    if (L"VirtualMachine" != RootJsonObject.GetNamedString(
        L"Type",
        winrt::hstring()))
    {
        throw winrt::hresult_invalid_argument(
            L"Invalid Virtual Machine Configuration");
    }

    NanaBox::VirtualMachineConfiguration Result;

    Result.Version = static_cast<std::uint32_t>(
        RootJsonObject.GetNamedNumber(
            L"Version",
            static_cast<double>(1)));
    if (Result.Version < 1 || Result.Version > 1)
    {
        throw winrt::hresult_invalid_argument(
            L"Invalid Version");
    }

    winrt::hstring GuestType = RootJsonObject.GetNamedString(
        L"GuestType",
        winrt::hstring());
    if (0 == std::wcscmp(GuestType.c_str(), L"Windows"))
    {
        Result.GuestType = NanaBox::GuestType::Windows;
    }
    else if (0 == std::wcscmp(GuestType.c_str(), L"Linux"))
    {
        Result.GuestType = NanaBox::GuestType::Linux;
    }
    else
    {
        Result.GuestType = NanaBox::GuestType::Unknown;
    }

    Result.Name = RootJsonObject.GetNamedString(
        L"Name",
        winrt::hstring());

    Result.ProcessorCount = static_cast<std::uint32_t>(
        RootJsonObject.GetNamedNumber(
            L"ProcessorCount",
            0.0));
    if (!Result.ProcessorCount)
    {
        throw winrt::hresult_out_of_bounds(
            L"Invalid Processor Count");
    }

    Result.MemorySize = static_cast<std::uint32_t>(
        RootJsonObject.GetNamedNumber(
            L"MemorySize",
            0.0));
    if (!Result.MemorySize)
    {
        throw winrt::hresult_out_of_bounds(
            L"Invalid Memory Size");
    }

    winrt::JsonArray ComPorts = RootJsonObject.GetNamedArray(
        L"ComPorts",
        nullptr);
    if (ComPorts)
    {
        for (winrt::IJsonValue const& RawComPort : ComPorts)
        {
            winrt::hstring Current = RawComPort.GetString();
            if (Current.empty())
            {
                continue;
            }

            Result.ComPorts.push_back(Current);
        }
    }
    
    winrt::JsonObject Gpu = RootJsonObject.GetNamedObject(
        L"Gpu",
        nullptr);
    if (Gpu)
    {
        winrt::hstring AssignmentMode = Gpu.GetNamedString(
            L"AssignmentMode",
            winrt::hstring());
        if (0 == std::wcscmp(AssignmentMode.c_str(), L"Default"))
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Default;
        }
        else if (0 == std::wcscmp(AssignmentMode.c_str(), L"List"))
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::List;
        }
        else if (0 == std::wcscmp(AssignmentMode.c_str(), L"Mirror"))
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Mirror;
        }
        else
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
        }

        winrt::JsonArray SelectedDevices = RootJsonObject.GetNamedArray(
            L"SelectedDevices",
            nullptr);
        if (SelectedDevices)
        {
            for (winrt::IJsonValue const& RawSelectedDevice : SelectedDevices)
            {
                winrt::hstring Current = RawSelectedDevice.GetString();
                if (Current.empty())
                {
                    continue;
                }

                Result.Gpu.SelectedDevices.push_back(Current);
            }
        }
        
        if (Result.Gpu.SelectedDevices.empty() &&
            Result.Gpu.AssignmentMode == NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
        }

        if (Result.Gpu.AssignmentMode != NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.SelectedDevices.clear();
        }
    }

    winrt::JsonArray NetworkAdapters = RootJsonObject.GetNamedArray(
        L"NetworkAdapters",
        nullptr);
    if (NetworkAdapters)
    {
        for (winrt::IJsonValue const& RawNetworkAdapter : NetworkAdapters)
        {
            winrt::JsonObject NetworkAdapter =
                RawNetworkAdapter.GetObject();

            NanaBox::NetworkAdapterConfiguration Current;

            Current.Enabled = NetworkAdapter.GetNamedBoolean(
                L"Enabled",
                true);

            Current.Connected = NetworkAdapter.GetNamedBoolean(
                L"Connected",
                true);

            Current.MacAddress = NetworkAdapter.GetNamedString(
                L"MacAddress",
                winrt::hstring());
            if (Current.MacAddress.empty())
            {
                continue;
            }

            Result.NetworkAdapters.push_back(Current);
        }
    }

    winrt::JsonArray ScsiDevices = RootJsonObject.GetNamedArray(
        L"ScsiDevices",
        nullptr);
    if (ScsiDevices)
    {
        for (winrt::IJsonValue const& RawScsiDevice : ScsiDevices)
        {
            winrt::JsonObject ScsiDevice =
                RawScsiDevice.GetObject();

            NanaBox::ScsiDeviceConfiguration Current;

            Current.Enabled = ScsiDevice.GetNamedBoolean(
                L"Enabled",
                true);

            winrt::hstring Type = ScsiDevice.GetNamedString(
                L"Type",
                winrt::hstring());
            if (0 == std::wcscmp(Type.c_str(), L"VirtualDisk"))
            {
                Current.Type = NanaBox::ScsiDeviceType::VirtualDisk;
            }
            else if (0 == std::wcscmp(Type.c_str(), L"VirtualImage"))
            {
                Current.Type = NanaBox::ScsiDeviceType::VirtualImage;
            }
            else if (0 == std::wcscmp(Type.c_str(), L"PhysicalDevice"))
            {
                Current.Type = NanaBox::ScsiDeviceType::PhysicalDevice;
            }
            else
            {
                continue;
            }

            Current.Path = ScsiDevice.GetNamedString(
                L"Path",
                winrt::hstring());
            if (Current.Path.empty() &&
                Current.Type != NanaBox::ScsiDeviceType::VirtualImage)
            {
                continue;
            }

            Result.ScsiDevices.push_back(Current);
        }
    }

    winrt::JsonArray SharedFolders = RootJsonObject.GetNamedArray(
        L"SharedFolders",
        nullptr);
    if (SharedFolders)
    {
        for (winrt::IJsonValue const& RawSharedFolder : SharedFolders)
        {
            winrt::JsonObject SharedFolder =
                RawSharedFolder.GetObject();

            NanaBox::SharedFolderConfiguration Current;

            Current.Enabled = SharedFolder.GetNamedBoolean(
                L"Enabled",
                true);

            Current.ReadOnly = SharedFolder.GetNamedBoolean(
                L"ReadOnly",
                true);

            Current.HostPath = SharedFolder.GetNamedString(
                L"HostPath",
                winrt::hstring());
            if (Current.HostPath.empty())
            {
                continue;
            }

            Current.GuestName = SharedFolder.GetNamedString(
                L"GuestName",
                winrt::hstring());
            if (Current.GuestName.empty())
            {
                continue;
            }

            Result.SharedFolders.push_back(Current);
        }
    }

    return Result;
}

namespace
{
    winrt::JsonObject MakeSchemaVersion(
        std::uint32_t const& Major,
        std::uint32_t const& Minor)
    {
        winrt::JsonObject Result;
        Result.Insert(
            L"Major",
            winrt::JsonValue::CreateNumberValue(
                static_cast<double>(Major)));
        Result.Insert(
            L"Minor",
            winrt::JsonValue::CreateNumberValue(
                static_cast<double>(Minor)));
        return Result; 
    }

    winrt::JsonObject MakeUefiBootEntry(
        std::uint32_t const& DiskNumber)
    {
        winrt::JsonObject Result;
        Result.Insert(
            L"DeviceType",
            winrt::JsonValue::CreateStringValue(
                L"ScsiDrive"));
        Result.Insert(
            L"DevicePath",
            winrt::JsonValue::CreateStringValue(
                L"NanaBox Scsi Controller"));
        Result.Insert(
            L"DiskNumber",
            winrt::JsonValue::CreateNumberValue(
                static_cast<double>(DiskNumber)));
        return Result;
    }

    winrt::JsonObject MakeComputeTopology(
        std::uint32_t ProcessorCount,
        std::uint64_t MemorySize)
    {
        winrt::JsonObject Result;
        {
            winrt::JsonObject Memory;
            Memory.Insert(
                L"SizeInMB",
                winrt::JsonValue::CreateNumberValue(
                    static_cast<double>(MemorySize)));
            Result.Insert(
                L"Memory",
                Memory);

            winrt::JsonObject Processor;
            Processor.Insert(
                L"Count",
                winrt::JsonValue::CreateNumberValue(
                    static_cast<double>(ProcessorCount)));
            Processor.Insert(
                L"ExposeVirtualizationExtensions",
                winrt::JsonValue::CreateBooleanValue(true));
            Result.Insert(
                L"Processor",
                Processor);
        }
        return Result;
    }

    winrt::JsonObject MakeVideoMonitor(
        std::uint16_t const& HorizontalResolution,
        std::uint16_t const& VerticalResolution)
    {
        winrt::JsonObject Result;
        Result.Insert(
            L"HorizontalResolution",
            winrt::JsonValue::CreateNumberValue(
                static_cast<double>(HorizontalResolution)));
        Result.Insert(
            L"VerticalResolution",
            winrt::JsonValue::CreateNumberValue(
                static_cast<double>(VerticalResolution)));
        return Result;
    }

    winrt::JsonObject MakeComPorts(
        std::vector<winrt::hstring> const& ComPorts)
    {
        winrt::JsonObject Result;
        std::uint32_t Count = 0;
        for (winrt::hstring const& ComPort : ComPorts)
        {
            winrt::JsonObject Current;

            Current.Insert(
                L"NamedPipe",
                winrt::JsonValue::CreateStringValue(ComPort));

            Result.Insert(
                winrt::to_hstring(Count++),
                Current);
        }
        return Result;
    }

    winrt::JsonObject MakeScsi(
        std::vector<NanaBox::ScsiDeviceConfiguration> const& Devices)
    {
        winrt::JsonObject Attachments;
        std::uint32_t Count = 0;
        for (NanaBox::ScsiDeviceConfiguration const& Device : Devices)
        {
            if (!Device.Enabled)
            {
                continue;
            }

            winrt::JsonObject Current;

            switch (Device.Type)
            {
            case NanaBox::ScsiDeviceType::VirtualDisk:
            {
                Current.Insert(
                    L"Type",
                    winrt::JsonValue::CreateStringValue(L"VirtualDisk"));
                break;
            }
            case NanaBox::ScsiDeviceType::VirtualImage:
            {
                Current.Insert(
                    L"Type",
                    winrt::JsonValue::CreateStringValue(L"Iso"));
                break;
            }
            case NanaBox::ScsiDeviceType::PhysicalDevice:
            {
                Current.Insert(
                    L"Type",
                    winrt::JsonValue::CreateStringValue(L"PassThru"));
                break;
            }
            default:
                throw winrt::hresult_invalid_argument(
                    L"Invalid Scsi Device Type");
            }

            Current.Insert(
                L"Path",
                winrt::JsonValue::CreateStringValue(Device.Path));

            Attachments.Insert(
                winrt::to_hstring(Count++),
                Current);
        }

        winrt::JsonObject ScsiController;
        ScsiController.Insert(
            L"Attachments",
            Attachments);

        winrt::JsonObject Result;
        Result.Insert(
            L"NanaBox Scsi Controller",
            ScsiController);
        return Result;
    }
}

winrt::hstring NanaBox::GenerateComputeSystemConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    winrt::JsonObject Result;

    Result.Insert(
        L"SchemaVersion",
        ::MakeSchemaVersion(
            2,
            1));

    Result.Insert(
        L"Owner",
        winrt::JsonValue::CreateStringValue(
            Configuration.Name));

    Result.Insert(
        L"ShouldTerminateOnLastHandleClosed",
        winrt::JsonValue::CreateBooleanValue(true));
    
    winrt::JsonObject VirtualMachine;
    {
        winrt::JsonObject Chipset;
        {
            winrt::JsonObject Uefi;
            {
                Uefi.Insert(
                    L"BootThis",
                    ::MakeUefiBootEntry(
                        0));
            }
            Chipset.Insert(
                L"Uefi",
                Uefi);
        }
        VirtualMachine.Insert(
            L"Chipset",
            Chipset);

        VirtualMachine.Insert(
            L"ComputeTopology",
            ::MakeComputeTopology(
                Configuration.ProcessorCount,
                Configuration.MemorySize));

        winrt::JsonObject Devices;
        {
            Devices.Insert(
                L"VideoMonitor",
                ::MakeVideoMonitor(
                    1024,
                    768));

            Devices.Insert(
                L"EnhancedModeVideo",
                winrt::JsonObject());

            Devices.Insert(
                L"Keyboard",
                winrt::JsonObject());

            Devices.Insert(
                L"Mouse",
                winrt::JsonObject());

            Devices.Insert(
                L"ComPorts",
                ::MakeComPorts(
                    Configuration.ComPorts));
            
            //std::vector<NetworkAdapterConfiguration> NetworkAdapters;

            Devices.Insert(
                L"Scsi",
                ::MakeScsi(
                    Configuration.ScsiDevices));
        }
        VirtualMachine.Insert(
            L"Devices",
            Devices);
    }
    Result.Insert(
        L"VirtualMachine",
        VirtualMachine);

    return Result.Stringify();
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

    this->m_MainWindowControl =
        winrt::make<winrt::NanaBox::implementation::MainWindowControl>();
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

    ::DwmWindowDisableSystemBackdrop(this->m_hWnd);

    winrt::FrameworkElement Content =
        this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();

    ::DwmWindowSetUseImmersiveDarkModeAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? TRUE
            : FALSE));

    ::DwmWindowSetCaptionColorAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? RGB(0, 0, 0)
            : RGB(255, 255, 255)));



    this->m_RdpClient->EnableAutoReconnect(false);
    this->m_RdpClient->RelativeMouseMode(true);
    this->m_RdpClient->AuthenticationServiceClass(L"Microsoft Virtual Console Service");

    this->m_RdpClient->AuthenticationLevel(0);
    this->m_RdpClient->EnableCredSspSupport(true);
    this->m_RdpClient->NegotiateSecurityLayer(false);
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

    this->m_RdpClient->GrabFocusOnConnect(false);




    this->m_RdpClient->Server(L"localhost");
    this->m_RdpClient->RDPPort(2179);
    this->m_RdpClient->MinInputSendInterval(20);



    this->m_RdpClient->PCB(g_VMID.c_str()/* + winrt::hstring(L";" L"EnhancedMode=1")*/);

    this->m_RdpClient->Connect();

    /*g_RdpClient->OnLoginComplete([hWnd]()
    {
        ::Sleep(200);

        RECT ClientRect;
        winrt::check_bool(::GetClientRect(hWnd, &ClientRect));
        ClientRect.top += 100;
        ClientRect.bottom -= 50;

        ULONG RawWidth = ClientRect.right - ClientRect.left;
        ULONG RawHeight = ClientRect.bottom - ClientRect.top;

        UINT WindowDpi = ::GetDpiForWindow(hWnd);

        this->m_RdpClient->UpdateSessionDisplaySettings(
            RawWidth,
            RawHeight,
            (ULONG)(RawWidth * 100 * 25.4),
            (ULONG)(RawHeight * 100 * 25.4),
            0,
            (ULONG)(WindowDpi * 100.0 / 96.0),
            100);
    });*/

    this->m_RdpClient->OnDisconnected([this](
        _In_ LONG DisconnectReason)
    {
        UNREFERENCED_PARAMETER(DisconnectReason);

        if (g_VirtualMachineRunning)
        {
            this->m_RdpClient->Connect();
        }
    });

    return 0;
}

void NanaBox::MainWindow::OnSize(
    UINT nType,
    CSize size)
{
    UNREFERENCED_PARAMETER(nType);
    UNREFERENCED_PARAMETER(size);

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    int MainWindowControlHeight =
        ::MulDiv(40, DpiValue, USER_DEFAULT_SCREEN_DPI);

    RECT ClientRect;
    winrt::check_bool(this->GetClientRect(&ClientRect));
    ClientRect.top += MainWindowControlHeight;

    this->m_RdpClientWindow.SetWindowPos(
        nullptr,
        &ClientRect,
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
        ClientRect.right - ClientRect.left,
        MainWindowControlHeight,
        SWP_SHOWWINDOW);


    //g_RdpClient->SyncSessionDisplaySettings();

    /*if (this->m_RdpClient->Connected() == 1)
    {
        ULONG Width = ClientRect.right - ClientRect.left;
        ULONG Height = ClientRect.bottom - ClientRect.top;

        UINT WindowDpi = ::GetDpiForWindow(this->m_hWnd);

        this->m_RdpClient->UpdateSessionDisplaySettings(
            Width,
            Height,
            Width,
            Height,
            0,
            static_cast<ULONG>(WindowDpi * 100.0 / 96.0),
            100);
    }*/
}

void NanaBox::MainWindow::OnDpiChanged(
    UINT nDpiX,
    UINT nDpiY,
    PRECT pRect)
{
    UNREFERENCED_PARAMETER(nDpiX);
    UNREFERENCED_PARAMETER(nDpiY);

    this->SetWindowPos(
        nullptr,
        pRect,
        SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT NanaBox::MainWindow::OnMenuChar(
    UINT nChar,
    UINT nFlags,
    WTL::CMenuHandle menu)
{
    UNREFERENCED_PARAMETER(nChar);
    UNREFERENCED_PARAMETER(nFlags);
    UNREFERENCED_PARAMETER(menu);

    // Reference: https://github.com/microsoft/terminal
    //            /blob/756fd444b1d443320cf2ed6887d4b65aa67a9a03
    //            /scratch/ScratchIslandApp
    //            /WindowExe/SampleIslandWindow.cpp#L155
    // Return this LRESULT here to prevent the app from making a bell
    // when alt+key is pressed. A menu is active and the user presses a
    // key that does not correspond to any mnemonic or accelerator key.

    return MAKELRESULT(0, MNC_CLOSE);
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

            ::DwmWindowSetUseImmersiveDarkModeAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? TRUE
                    : FALSE));

            ::DwmWindowSetCaptionColorAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? RGB(0, 0, 0)
                    : RGB(255, 255, 255)));
        }
    }
}

void NanaBox::MainWindow::OnDestroy()
{
    this->m_RdpClient->DisableEventsDispatcher();
    this->m_RdpClientWindow.DestroyWindow();
    this->m_RdpClient = nullptr;

    this->m_XamlSource.Close();

    ::PostQuitMessage(0);
}

namespace
{
    WTL::CAppModule g_Module;
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

    winrt::NanaBox::App app =
        winrt::make<winrt::NanaBox::implementation::App>();

    /*winrt::check_hresult(::HcsCreateEmptyGuestStateFile(L"D:\\Test\\Test.vmgs"));
    winrt::check_hresult(::HcsCreateEmptyRuntimeStateFile(L"D:\\Test\\Test.vmrs"));
    winrt::check_hresult(::HcsGrantVmAccess(L"Sample", L"D:\\Test\\Test.vmgs"));
    winrt::check_hresult(::HcsGrantVmAccess(L"Sample", L"D:\\Test\\Test.vmrs"));*/

    /*static constexpr wchar_t c_VmConfiguration[] = LR"(
    {
        "VirtualMachine": {
            "Chipset": {
                "Uefi": {
                    "ApplySecureBootTemplate": "Apply",
                    "SecureBootTemplateId": "1734c6e8-3154-4dda-ba5f-a874cc483422"
                }
            },
            "GuestState": {
                "GuestStateFilePath": "D:\\NanaBox VM\\B5BAB8AD-5A00-4EB7-BD48-966E3D382307.vmgs",
                "GuestStateFileType": "FileMode",
			    "ForceTransientState": true,
                "RuntimeStateFilePath": "D:\\NanaBox VM\\B5BAB8AD-5A00-4EB7-BD48-966E3D382307.vmrs"
            },
            "SecuritySettings": {
                "EnableTpm": true
            }
        }
    })";*/

    NanaBox::VirtualMachineConfiguration Configuration;

    Configuration.Version = 1;
    Configuration.GuestType = NanaBox::GuestType::Windows;
    Configuration.Name = L"DemoVM";
    Configuration.ProcessorCount = 2;
    Configuration.MemorySize = 2048;
    {
        NanaBox::ScsiDeviceConfiguration Device;
        Device.Enabled = true;
        Device.Type = NanaBox::ScsiDeviceType::VirtualDisk;
        Device.Path = L"D:\\Hyper-V\\DemoVM\\Virtual Hard Disks\\DemoVM.vhdx";
        Configuration.ScsiDevices.push_back(Device);
    }
    {
        NanaBox::ScsiDeviceConfiguration Device;
        Device.Enabled = true;
        Device.Type = NanaBox::ScsiDeviceType::VirtualImage;
        Device.Path = L"";
        Configuration.ScsiDevices.push_back(Device);
    }

    NanaBox::ComputeSystem test(
        Configuration.Name,
        NanaBox::GenerateComputeSystemConfiguration(Configuration));

    test.SystemExited([](
        winrt::hstring const& EventData)
    {
        UNREFERENCED_PARAMETER(EventData);

        g_VirtualMachineRunning = false;
    });

    test.SystemRdpEnhancedModeStateChanged([]()
    {
        g_RdpClientEnhancedMode = !g_RdpClientEnhancedMode;
    });

    test.Start();

    g_VirtualMachineRunning = true;

    auto fuck = test.GetProperties();

    winrt::JsonObject testobj = winrt::JsonObject::Parse(fuck);
    g_VMID = testobj.GetNamedString(L"RuntimeId");

    //::MessageBoxW(nullptr, fuck.c_str(), L"NanaBox", 0);


    WTL::CMessageLoop MessageLoop;

    ATL::AtlAxWinInit();

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    NanaBox::MainWindow Window;
    if (!Window.Create(
        nullptr,
        Window.rcDefault,
        L"NanaBox",
        WS_OVERLAPPEDWINDOW))
    {
        return -1;
    }
    Window.ShowWindow(nShowCmd);
    Window.UpdateWindow();

    int Result = MessageLoop.Run();

    g_Module.RemoveMessageLoop();
    g_Module.Term();

    app.Close();

    return Result;
}
