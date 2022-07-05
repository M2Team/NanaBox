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
    using Windows::Data::Json::JsonObject;
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



    this->m_RdpClient->PCB(g_VMID.c_str() + winrt::hstring(L";" L"EnhancedMode=1"));

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

    if (this->m_RdpClient->Connected() == 1)
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
    }
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

    /*winrt::check_hresult(::HcsCreateEmptyGuestStateFile(L"D:\\Test\\Test.vmgs"));
    winrt::check_hresult(::HcsCreateEmptyRuntimeStateFile(L"D:\\Test\\Test.vmrs"));
    winrt::check_hresult(::HcsGrantVmAccess(L"Sample", L"D:\\Test\\Test.vmgs"));
    winrt::check_hresult(::HcsGrantVmAccess(L"Sample", L"D:\\Test\\Test.vmrs"));*/

    /*static constexpr wchar_t c_VmConfiguration[] = LR"(
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
            },
            "SecuritySettings": {
                "EnableTpm": true
            }
        }
    })";*/

    /*static constexpr wchar_t c_VmConfiguration[] = LR"(
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
                    "BootThis": {
                        "DevicePath": "Primary disk",
                        "DiskNumber": 0,
                        "DeviceType": "ScsiDrive"
                    }
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
                    "BootThis": {
                        "DevicePath": "Primary disk",
                        "DiskNumber": 0,
                        "DeviceType": "ScsiDrive"
                    }
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
                            },
                            "1": {
                                "Type": "Iso"
                            }
                        }
                    }
                }
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
