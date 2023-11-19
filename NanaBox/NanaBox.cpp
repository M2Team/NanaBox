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

#include "VirtualMachineConfiguration.h"

#include <cwchar>

#include "pch.h"

#include "App.h"
#include "MainWindow.h"
#include "ExitConfirmationPage.h"
#include "QuickStartPage.h"

#include <json.hpp>

#include <Mile.Project.Version.h>

#include "Utils.h"
#include "NanaBoxResources.h"

#include <Mile.Helpers.h>
#include <Mile.Xaml.h>

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace
{
    const std::wstring_view g_AppUserModelID =
        L"M2Team.NanaBox"
        L"_" MILE_PROJECT_DOT_VERSION_STRING
        L"_" MILE_PROJECT_BUILD_DATE_STRING;

    WTL::CAppModule g_Module;
    std::wstring g_ConfigurationFilePath;
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
        winrt::make< winrt::NanaBox::implementation::ExitConfirmationPage>(
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
        g_ConfigurationFilePath,
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

        ::ShowMessageDialog(
            nullptr,
            InstructionText,
            ContentText);

        ::ExitProcess(ex.code());
    }

    SERVICE_STATUS_PROCESS ServiceStatus = { 0 };
    if (!::MileStartService(L"vmms", &ServiceStatus))
    {
        ::ShowMessageDialog(
            nullptr,
            Mile::WinRT::GetLocalizedString(
                L"Messages/VMMSNotAvailableInstructionText"),
            Mile::WinRT::GetLocalizedString(
                L"Messages/VMMSNotAvailableContentText"));
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

    /*{
        HWND WindowHandle = ::CreateWindowExW(
            WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME,
            L"Mile.Xaml.ContentWindow",
            nullptr,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            nullptr,
            nullptr);
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

        winrt::NanaBox::QuickStartPage Window =
            winrt::make<winrt::NanaBox::implementation::QuickStartPage>(
                WindowHandle);
        ::ShowXamlDialog(
            WindowHandle,
            460,
            460,
            winrt::get_abi(Window),
            nullptr);
    }*/

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
    std::wstring TargetBinaryPath;

    if (PackagedMode)
    {
        try
        {
            std::wstring AppBinaryPath;
            {
                std::wstring RawPath = ::GetCurrentProcessModulePath();
                std::wcsrchr(&RawPath[0], L'\\')[0] = L'\0';
                RawPath.resize(std::wcslen(RawPath.c_str()));
                AppBinaryPath = RawPath;
            }

            std::wstring TempBinaryPath;
            {
                std::wstring RawPath = ::GetLocalStateFolderPath();
                GUID TempFolderGuid;
                winrt::check_hresult(::CoCreateGuid(&TempFolderGuid));
                RawPath.append(L"\\");
                RawPath.append(::FromGuid(TempFolderGuid));
                TempBinaryPath = RawPath;
            }

            winrt::check_bool(::CreateDirectoryW(
                TempBinaryPath.c_str(),
                nullptr));

            winrt::check_bool(::CopyFileW(
                (AppBinaryPath + L"\\NanaBox.exe").c_str(),
                (TempBinaryPath + L"\\NanaBox.exe").c_str(),
                FALSE));

            winrt::check_bool(::CopyFileW(
                (AppBinaryPath + L"\\resources.pri").c_str(),
                (TempBinaryPath + L"\\resources.pri").c_str(),
                FALSE));

            winrt::check_bool(::CopyFileW(
                (AppBinaryPath + L"\\Mile.Xaml.Styles.SunValley.xbf").c_str(),
                (TempBinaryPath + L"\\Mile.Xaml.Styles.SunValley.xbf").c_str(),
                FALSE));

            TargetBinaryPath = TempBinaryPath;
        }
        catch (...)
        {
            winrt::hresult_error Exception = Mile::WinRT::ToHResultError();
            ::ShowErrorMessageDialog(Exception);
            ::ExitProcess(Exception.code());
        }
    }

    if (!::MileIsCurrentProcessElevated() || PackagedMode)
    {
        try
        {
            if (PackagedMode && !TargetBinaryPath.empty())
            {
                ApplicationName = TargetBinaryPath + L"\\NanaBox.exe";
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
                ::SimpleRemoveDirectory(TargetBinaryPath.c_str());
            }
        }
        catch (...)
        {
            winrt::hresult_error Exception = Mile::WinRT::ToHResultError();

            if (Exception.code() != ::HRESULT_FROM_WIN32(ERROR_CANCELLED))
            {
                ::ShowErrorMessageDialog(Exception);
            }

            if (PackagedMode && !TargetBinaryPath.empty())
            {
                ::SimpleRemoveDirectory(TargetBinaryPath.c_str());
            }

            ::ExitProcess(Exception.code());
        }

        ::ExitProcess(0);
    }

    ::PrerequisiteCheck();

    if (!UnresolvedCommandLine.empty())
    {
        g_ConfigurationFilePath = ::GetAbsolutePath(UnresolvedCommandLine);
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
                ::ShowErrorMessageDialog(ex);
            }

            ::ExitProcess(ex.code());
        }
    }

    {
        std::wstring CurrentPath = g_ConfigurationFilePath;
        std::wcsrchr(&CurrentPath[0], L'\\')[0] = L'\0';
        CurrentPath.resize(std::wcslen(CurrentPath.c_str()));
        ::SetCurrentDirectoryW(CurrentPath.c_str());
    } 

    WTL::CMessageLoop MessageLoop;

    ATL::AtlAxWinInit();

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    NanaBox::MainWindow MainWindow;
    if (!MainWindow.Create(
        nullptr,
        MainWindow.rcDefault,
        L"NanaBox",
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
