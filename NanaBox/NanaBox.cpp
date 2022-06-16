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

#include <cwchar>

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlhost.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>
#include <atltypes.h>

#include <winrt/Windows.Data.Json.h>


namespace winrt
{
    using Windows::Data::Json::JsonObject;
}

namespace
{
    winrt::hstring g_VMID = L"48781dff-90cc-4650-89c3-fe12e6210b19";
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

        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;
    };

}

int NanaBox::MainWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    this->m_RdpClient = winrt::make_self<NanaBox::RdpClient>();

    this->m_RdpClient->EnableEventsDispatcher();

    RECT ClientRect;
    winrt::check_bool(this->GetClientRect(&ClientRect));
    ClientRect.top += 100;

    if (!this->m_RdpClientWindow.Create(
        this->m_hWnd,
        ClientRect,
        nullptr,
        WS_CHILD | WS_VISIBLE))
    {
        return -1;
    }

    winrt::check_hresult(this->m_RdpClientWindow.AttachControl(
        this->m_RdpClient->RawControl().get(),
        nullptr));

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

        DisconnectReason = DisconnectReason;

        //this->m_RdpClient->Connect();  
    });

    return 0;
}

void NanaBox::MainWindow::OnSize(
    UINT nType,
    CSize size)
{
    UNREFERENCED_PARAMETER(nType);
    UNREFERENCED_PARAMETER(size);

    RECT ClientRect;
    winrt::check_bool(this->GetClientRect(&ClientRect));
    ClientRect.top += 100;

    this->m_RdpClientWindow.SetWindowPos(
        nullptr,
        &ClientRect,
        SWP_NOZORDER | SWP_NOACTIVATE);

    //g_RdpClient->SyncSessionDisplaySettings();

    //if (g_RdpClient->Connected() == 1)
    //{
    //    ULONG Width = ClientRect.right - ClientRect.left;
    //    ULONG Height = ClientRect.bottom - ClientRect.top;

    //    //UINT WindowDpi = ::GetDpiForWindow(hWnd);

    //    g_RdpClient->UpdateSessionDisplaySettings(
    //        Width,
    //        Height,
    //        Width,
    //        Height,
    //        0,
    //        150, //(ULONG)(WindowDpi * 100.0 / 96.0),
    //        100);
    //}
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

    }
}

void NanaBox::MainWindow::OnDestroy()
{
    this->m_RdpClient->DisableEventsDispatcher();
    this->m_RdpClientWindow.DestroyWindow();
    this->m_RdpClient = nullptr;
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

    /*static constexpr wchar_t c_VmConfigurationNew[] = LR"(
    {
        "SchemaVersion": {
            "Major": 2,
            "Minor": 1
        },
        "Owner": "Sample",
        "ShouldTerminateOnLastHandleClosed": true,
        "VirtualMachine": {
            "Chipset": {
                "Uefi": {
                    "ApplySecureBootTemplate": "Apply",
                    "SecureBootTemplateId": "1734c6e8-3154-4dda-ba5f-a874cc483422"
                }
            },
            "ComputeTopology": {
                "Memory": {
                    "Backing": "Virtual",
                    "SizeInMB": 2048
                },
                "Processor": {
                    "Count": 2,
                    "ExposeVirtualizationExtensions": true
                }
            },
            "Devices": {
                "VideoMonitor": {},
                "EnhancedModeVideo": {},
                "Keyboard": {},
                "Mouse": {},
                "Scsi": {
                    "Primary disk": {
                        "Attachments": {
                            "0": {
                                "Type": "Iso",
                                "Path": "D:\\Projects\\MouriNaruto\\AluImageWorkspace\\Assets\\WindowsImages\\22000.493\\en-us_windows_11_consumer_editions_updated_feb_2022_x64_dvd_aa300219.iso"
                            },
                            "1": {
                                "Type": "VirtualDisk",
                                "Path": "D:\\Hyper-V\\DemoVM\\Virtual Hard Disks\\DemoVM.vhdx"
                            }
                        }
                    }
                }
            },
            "GuestState": {
                "GuestStateFilePath": "D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmgs",
                "RuntimeStateFilePath": "D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmrs"
            },
            "SecuritySettings": {
                "EnableTpm": true
            }
        }
    })";*/

    static constexpr wchar_t c_VmConfiguration[] = LR"(
    {
        "SchemaVersion": {
            "Major": 2,
            "Minor": 1
        },
        "Owner": "Sample",
        "ShouldTerminateOnLastHandleClosed": true,
        "VirtualMachine": {
            "Chipset": {
                "Uefi": {
                    "SecureBootTemplateId": "1734c6e8-3154-4dda-ba5f-a874cc483422"
                }
            },
            "ComputeTopology": {
                "Memory": {
                    "Backing": "Virtual",
                    "SizeInMB": 2048
                },
                "Processor": {
                    "Count": 2,
                    "ExposeVirtualizationExtensions": true
                }
            },
            "Devices": {
                "VideoMonitor": {
                    "HorizontalResolution" : 1024,
                    "VerticalResolution": 768
                },
                "EnhancedModeVideo": {},
                "Keyboard": {},
                "Mouse": {},
                "Scsi": {
                    "Primary disk": {
                        "Attachments": {
                            "0": {
                                "Type": "VirtualDisk",
                                "Path": "D:\\Hyper-V\\DemoVM\\Virtual Hard Disks\\DemoVM.vhdx"
                            }
                        }
                    }
                }
            },
            "GuestState": {
                "GuestStateFilePath": "D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmgs",
                "RuntimeStateFilePath": "D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmrs"
            }
        }
    })";

    //ATL::AtlAxAttachControl

    /*winrt::check_hresult(::HcsGrantVmAccess(
        L"Sample",
        L"D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmgs"));
    winrt::check_hresult(::HcsGrantVmAccess(
        L"Sample",
        L"D:\\Hyper-V\\DemoVM\\Virtual Machines\\48781DFF-90CC-4650-89C3-FE12E6210B19.vmrs"));*/

    NanaBox::ComputeSystem test(L"Sample", c_VmConfiguration);

    test.SystemExited([](
        winrt::hstring const& EventData)
    {
        UNREFERENCED_PARAMETER(EventData);

        //g_RdpClient->OnDisconnected(g_OnDisconnectedToken);
        //g_RdpClient->Disconnect();
    });

    test.SystemRdpEnhancedModeStateChanged([]()
    {
        ::MessageBoxW(nullptr, L"fuck", L"NanaZip", 0);
        /*try
        {
            g_RdpClient->Disconnect();
            while (g_RdpClient->Connected());

            winrt::com_ptr<IOleInPlaceObject> OleInPlaceObject =
                g_RdpClient->RawControl().as<IOleInPlaceObject>();

            winrt::check_hresult(OleInPlaceObject->InPlaceDeactivate());

            winrt::com_ptr<IOleObject> OleObject =
                g_RdpClient->RawControl().as<IOleObject>();

            winrt::check_hresult(OleObject->Close(OLECLOSE_NOSAVE));
            winrt::check_hresult(OleObject->SetClientSite(nullptr));

            g_RdpClient = nullptr;
        }
        catch (...)
        {

        }

        for (size_t i = 0; i < 5; ++i)
        {
            try
            {
                ::Sleep(50);

                HWND hWnd = g_WindowHandle;

                g_RdpClient = winrt::make_self<NanaBox::RdpClient>();

                RECT ClientRect;
                winrt::check_bool(::GetClientRect(hWnd, &ClientRect));
                ClientRect.top += 100;
                ClientRect.bottom -= 50;

                winrt::com_ptr<IOleObject> OleObject =
                    g_RdpClient->RawControl().as<IOleObject>();

                winrt::com_ptr<IOleInPlaceActiveObject> OleInPlaceActiveObject =
                    g_RdpClient->RawControl().as<IOleInPlaceActiveObject>();

                winrt::com_ptr<IOleInPlaceObject> OleInPlaceObject =
                    g_RdpClient->RawControl().as<IOleInPlaceObject>();

                winrt::check_hresult(OleObject->SetClientSite(
                    g_OleClientSite.get()));

                winrt::check_hresult(OleObject->DoVerb(
                    OLEIVERB_INPLACEACTIVATE,
                    nullptr,
                    g_OleClientSite.get(),
                    0,
                    hWnd,
                    &ClientRect));

                g_RdpClient->EnableAutoReconnect(false);
                g_RdpClient->RelativeMouseMode(true);
                g_RdpClient->AuthenticationServiceClass(L"Microsoft Virtual Console Service");

                g_RdpClient->AuthenticationLevel(0);
                g_RdpClient->EnableCredSspSupport(true);
                g_RdpClient->NegotiateSecurityLayer(false);
                try
                {
                    VARIANT Value;
                    Value.vt = VT_BOOL;
                    Value.boolVal = VARIANT_TRUE;
                    g_RdpClient->Property(L"DisableCredentialsDelegation", Value);
                }
                catch (...)
                {

                }

                g_RdpClient->GrabFocusOnConnect(false);




                g_RdpClient->Server(L"localhost");
                g_RdpClient->RDPPort(2179);
                g_RdpClient->MinInputSendInterval(20);

                g_RdpClient->PCB(g_VMID.c_str() + winrt::hstring(L";" L"EnhancedMode=1"));

                g_RdpClient->Connect();
            }
            catch (...)
            {

            }
        }*/

        

        
    });

    test.Start();

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

    return Result;
}
