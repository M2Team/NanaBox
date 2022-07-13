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
#include "MainWindowExitNoticeControl.h"

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <json.hpp>

#include "NanaBoxResources.h"

#include <Mile.Windows.DwmHelpers.h>

namespace winrt
{
    using Windows::UI::Xaml::ElementTheme;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::UIElement;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
    using Windows::UI::Xaml::Media::VisualTreeHelper;
}

namespace NanaBox
{
    class MainWindowExitNoticeWindow : public ATL::CWindowImpl<
        MainWindowExitNoticeWindow>
    {
    public:

        DECLARE_WND_CLASS(L"MainWindowExitNoticeWindow")

        BEGIN_MSG_MAP(MainWindowExitNoticeWindow)
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

        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaBox::MainWindowExitNoticeControl m_XamlControl;
    };
}

int NanaBox::MainWindowExitNoticeWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    using MainWindowExitNoticeControlInstance =
        winrt::NanaBox::implementation::MainWindowExitNoticeControl;

    this->m_XamlControl = winrt::make<MainWindowExitNoticeControlInstance>();

    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();
    winrt::check_hresult(
        XamlSourceNative->AttachToWindow(this->m_hWnd));
    this->m_XamlSource.Content(this->m_XamlControl);

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));

    // Focus on XAML Island host window for Acrylic brush support.
    ::SetFocus(XamlWindowHandle);

    ::MileDisableSystemBackdrop(this->m_hWnd);

    winrt::FrameworkElement Content =
        this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();

    ::MileSetUseImmersiveDarkModeAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? TRUE
            : FALSE));

    ::MileSetCaptionColorAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? RGB(0, 0, 0)
            : RGB(255, 255, 255)));

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

void NanaBox::MainWindowExitNoticeWindow::OnSize(
    UINT nType,
    CSize size)
{
    UNREFERENCED_PARAMETER(nType);
    UNREFERENCED_PARAMETER(size);

    RECT ClientRect;
    winrt::check_bool(this->GetClientRect(&ClientRect));

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
        ClientRect.bottom - ClientRect.top,
        SWP_SHOWWINDOW);
}

void NanaBox::MainWindowExitNoticeWindow::OnDpiChanged(
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

LRESULT NanaBox::MainWindowExitNoticeWindow::OnMenuChar(
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

void NanaBox::MainWindowExitNoticeWindow::OnSettingChange(
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

            ::MileSetUseImmersiveDarkModeAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? TRUE
                    : FALSE));

            ::MileSetCaptionColorAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? RGB(0, 0, 0)
                    : RGB(255, 255, 255)));
        }
    }
}

void NanaBox::MainWindowExitNoticeWindow::OnDestroy()
{
    this->m_XamlSource.Close();

    this->ShowWindow(SW_HIDE);

    ::PostQuitMessage(0);
}

namespace NanaBox
{
    class MainWindow : public ATL::CWindowImpl<
        MainWindow>
    {
    public:

        DECLARE_WND_CLASS(L"NanaBoxMainWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DPICHANGED(OnDpiChanged)
            MSG_WM_MENUCHAR(OnMenuChar)
            MSG_WM_SETTINGCHANGE(OnSettingChange)
            MSG_WM_CLOSE(OnClose)
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

        void OnClose();

        void OnDestroy();

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::slim_mutex m_RdpClientOperationMutex;
        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;
        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaBox::MainWindowControl m_MainWindowControl;
        winrt::com_ptr<NanaBox::ComputeSystem> m_VirtualMachine;
        winrt::hstring m_VMID;
        bool m_VirtualMachineRunning = false;
        bool m_EnableEnhancedMode = false;

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

    this->InitializeVirtualMachine();

    this->m_MainWindowControl =
        winrt::make<winrt::NanaBox::implementation::MainWindowControl>();
    this->m_MainWindowControl.RequestEnhancedSession([this](
        bool const& RequestState)
    {
        this->m_EnableEnhancedMode = RequestState;
        this->m_RdpClient->Disconnect();
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
    this->m_MainWindowControl.RequestShutdownVirtualMachine([this]()
    {
        this->m_VirtualMachine->Shutdown();
    });

    this->m_MainWindowControl.RequestRestartVirtualMachine([this]()
    {
        this->m_VirtualMachine->Terminate();
        this->m_VirtualMachine->Start();
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

    ::MileDisableSystemBackdrop(this->m_hWnd);

    winrt::FrameworkElement Content =
        this->m_XamlSource.Content().try_as<winrt::FrameworkElement>();

    ::MileSetUseImmersiveDarkModeAttribute(
        this->m_hWnd,
        (Content.ActualTheme() == winrt::ElementTheme::Dark
            ? TRUE
            : FALSE));

    ::MileSetCaptionColorAttribute(
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



    this->m_RdpClient->PCB(this->m_VMID.c_str()/* + winrt::hstring(L";" L"EnhancedMode=1")*/);

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

        winrt::slim_lock_guard Guard(this->m_RdpClientOperationMutex);

        if (this->m_VirtualMachineRunning)
        {
            winrt::hstring PCB = this->m_VMID;

            if (this->m_EnableEnhancedMode)
            {
                PCB = this->m_VMID + L";" + L"EnhancedMode=1";
            }

            this->m_RdpClient->PCB(PCB);

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

            ::MileSetUseImmersiveDarkModeAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? TRUE
                    : FALSE));

            ::MileSetCaptionColorAttribute(
                this->m_hWnd,
                (Content.ActualTheme() == winrt::ElementTheme::Dark
                    ? RGB(0, 0, 0)
                    : RGB(255, 255, 255)));
        }
    }
}

void NanaBox::MainWindow::OnClose()
{
    const int Width = 400;
    const int Height = 200;

    UINT DpiValue = ::GetDpiForWindow(this->m_hWnd);

    NanaBox::MainWindowExitNoticeWindow Window;
    if (!Window.Create(
        this->m_hWnd,
        Window.rcDefault,
        nullptr,
        WS_CAPTION | WS_SYSMENU,
        WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME))
    {
        return;
    }
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
    this->EnableWindow(FALSE);
    MessageLoop.Run();
    this->EnableWindow(TRUE);
    this->SetActiveWindow();
}

void NanaBox::MainWindow::OnDestroy()
{
    this->m_RdpClient->DisableEventsDispatcher();
    this->m_RdpClientWindow.DestroyWindow();
    this->m_RdpClient = nullptr;

    this->m_XamlSource.Close();

    ::PostQuitMessage(0);
}

void NanaBox::MainWindow::InitializeVirtualMachine()
{
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

    NanaBox::HcnNetwork NetworkHandle = NanaBox::HcnOpenNetwork(
        NanaBox::DefaultSwitchId);

    /*GUID EndpointId;
    winrt::check_hresult(::CoCreateGuid(&EndpointId));*/

    /*static constexpr wchar_t c_EndpointConfiguration[] = LR"(
    {
      "ID": "b628ccf8-cb1f-405c-9c1a-3ca76526e4e0",
      "Name": "Ethernet",
      "Version": 64424509440,
      "AdditionalParams": {},
      "Resources": {
        "AdditionalParams": {},
        "AllocationOrder": 0,
        "CompartmentOperationTime": 0,
        "Flags": 0,
        "Health": {
          "LastErrorCode": 0,
          "LastUpdateTime": 133019448371526766
        },
        "ID": "E9A1B213-E312-4033-9ED3-8292DFB20E62",
        "PortOperationTime": 0,
        "State": 1,
        "SwitchOperationTime": 0,
        "VfpOperationTime": 0,
        "parentId": "BD080206-CABC-4D72-8AEC-DEA141D519EB"
      },
      "State": 1,
      "VirtualNetwork": "c08cb7b8-9b3c-408e-8e30-5e16a3aeb444",
      "VirtualNetworkName": "Default Switch",
      "MacAddress": "00-15-5D-64-2F-8B",
      "Flags": 32,
      "SharedContainers": []
    })";*/

    winrt::guid EndpointId = winrt::guid(
        "B628CCF8-CB1F-405C-9C1A-3CA76526E4E0");

    nlohmann::json Settings;
    Settings["VirtualNetwork"] = "C08CB7B8-9B3C-408E-8E30-5E16A3AEB444";
    //Settings["ID"] = "B628CCF8-CB1F-405C-9C1A-3CA76526E4E0";
    Settings["MacAddress"] = "00-15-5D-64-2F-AB";

    //Settings["SchemaVersion"]["Major"] = 2;
    //Settings["SchemaVersion"]["Minor"] = 0;
    //Settings["ID"] = "B628CCF8-CB1F-405C-9C1A-3CA76526E4E0"; //
    //Settings["Name"] = "B628CCF8-CB1F-405C-9C1A-3CA76526E4E0"; //
    //Settings["HostComputeNetwork"] = "B628CCF8-CB1F-405C-9C1A-3CA76526E4E0"; //
    //Settings["HostComputeNamespace"] =  "";
    /*Settings["Policies"] = nullptr;
    Settings["IpConfigurations"] = nullptr;
    Settings["Dns"] = nullptr;
    Settings["Routes"] = nullptr;
    Settings["MacAddress"] = "00-15-5D-64-2F-8B";
    Settings["Flags"] = 0;
    Settings["Health"] = nullptr;*/


    //
    // // winrt::to_string(winrt::to_hstring(EndpointId));
    //
    //Settings["Flags"] = 0;

    try
    {
        NanaBox::HcnDeleteEndpoint(EndpointId);
    }
    catch (...)
    {

    }

    NanaBox::HcnEndpoint EndpointHandle = NanaBox::HcnCreateEndpoint(
        NetworkHandle,
        EndpointId,
        winrt::to_hstring(Settings.dump()));

    winrt::hstring EndpointProperties = NanaBox::HcnQueryEndpointProperties(
        EndpointHandle);

    /*::MessageBoxW(
        nullptr,
        EndpointProperties.c_str(),
        L"NanaBox",
        MB_ICONINFORMATION);

    try
    {
        NanaBox::HcnDeleteEndpoint(EndpointId);
    }
    catch (...)
    {

    }*/

    NanaBox::VirtualMachineConfiguration Configuration;

    Configuration.Version = 1;
    Configuration.GuestType = NanaBox::GuestType::Windows;
    Configuration.Name = "DemoVM";
    Configuration.ProcessorCount = 2;
    Configuration.MemorySize = 4096;
    Configuration.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Mirror;
    {
        NanaBox::ScsiDeviceConfiguration Device;
        Device.Enabled = true;
        Device.Type = NanaBox::ScsiDeviceType::VirtualDisk;
        Device.Path = "D:\\Hyper-V\\Windows 11\\Windows 11\\Virtual Hard Disks\\Windows 11.vhdx";
        Configuration.ScsiDevices.push_back(Device);
    }
    {
        NanaBox::ScsiDeviceConfiguration Device;
        Device.Enabled = true;
        Device.Type = NanaBox::ScsiDeviceType::VirtualImage;
        Device.Path = "";
        Configuration.ScsiDevices.push_back(Device);
    }
    {
        NanaBox::NetworkAdapterConfiguration Device;
        Device.Enabled = true;
        Device.Connected = true;
        Device.MacAddress = "00-15-5D-64-2F-AB";
        Device.EndpointId = "b628ccf8-cb1f-405c-9c1a-3ca76526e4e0";
        Configuration.NetworkAdapters.push_back(Device);
    }
    {
        NanaBox::SharedFolderConfiguration Folder;
        Folder.Enabled = true;
        Folder.ReadOnly = true;
        Folder.HostPath = "D:\\TempState";
        Folder.GuestName = "TempState";
        Configuration.SharedFolders.push_back(Folder);
    }

    /*std::string Test = NanaBox::SerializeConfiguration(Configuration);

    Test = Test;

    auto Fuck = NanaBox::DeserializeConfiguration(Test);

    Fuck = Fuck;*/


    this->m_VirtualMachine = winrt::make_self<NanaBox::ComputeSystem>(
        winrt::to_hstring(Configuration.Name),
        winrt::to_hstring(NanaBox::MakeHcsConfiguration(Configuration)));

    this->m_VirtualMachine->SystemExited([this](
        winrt::hstring const& EventData)
    {
        UNREFERENCED_PARAMETER(EventData);

        this->m_VirtualMachineRunning = false;
    });

    /*this->m_VirtualMachine->SystemRdpEnhancedModeStateChanged([this]()
    {
        this->m_EnableEnhancedMode = !this->m_EnableEnhancedMode;
    });*/

    this->m_VirtualMachine->Start();

    //test.Pause();

    /*{
        nlohmann::json Request;
        Request["ResourcePath"] = "VirtualMachine/Devices/NetworkAdapters/B628CCF8";
        Request["RequestType"] = "Add";
        Request["Settings"]["EndpointId"] = "b628ccf8-cb1f-405c-9c1a-3ca76526e4e0";
        Request["Settings"]["MacAddress"] = "00-15-5D-64-2F-AB";
        VirtualMachine.Modify(winrt::to_hstring(Request.dump()));
    }*/

    this->m_VirtualMachine->Modify(winrt::to_hstring(
        NanaBox::MakeHcsUpdateGpuRequest(Configuration.Gpu)));

    //VirtualMachine.Resume();

    this->m_VirtualMachineRunning = true;

    nlohmann::json Properties = nlohmann::json::parse(
        winrt::to_string(this->m_VirtualMachine->GetProperties()));

    this->m_VMID = winrt::to_hstring(Properties["RuntimeId"]);

    /*::MessageBoxW(
        nullptr,
        winrt::to_hstring(Properties.dump(2)).c_str(),
        L"NanaBox",
        0);*/
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
