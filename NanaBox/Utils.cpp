#include "Utils.h"

#include <Mile.Helpers.h>
#include <Mile.Xaml.h>

#include <ShlObj.h>

#include <initguid.h>
#include <virtdisk.h>
#pragma comment(lib,"virtdisk.lib")

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <sddl.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

#include "MessagePage.h"
#include "AboutPage.h"
#include "NewVirtualHardDiskPage.h"
#include "ResizeVirtualHardDiskPage.h"

namespace winrt
{
    using Windows::UI::Xaml::HorizontalAlignment;
    using Windows::UI::Xaml::ThicknessHelper;
    using Windows::UI::Xaml::VerticalAlignment;
    using NanaBox::ProgressRing;
}

void SplitCommandLineEx(
    std::wstring const& CommandLine,
    std::vector<std::wstring> const& OptionPrefixes,
    std::vector<std::wstring> const& OptionParameterSeparators,
    std::wstring& ApplicationName,
    std::map<std::wstring, std::wstring>& OptionsAndParameters,
    std::wstring& UnresolvedCommandLine)
{
    ApplicationName.clear();
    OptionsAndParameters.clear();
    UnresolvedCommandLine.clear();

    size_t arg_size = 0;
    for (auto& SplitArgument : Mile::SplitCommandLineWideString(CommandLine))
    {
        // We need to process the application name at the beginning.
        if (ApplicationName.empty())
        {
            // For getting the unresolved command line, we need to cumulate
            // length which including spaces.
            arg_size += SplitArgument.size() + 1;

            // Save
            ApplicationName = SplitArgument;
        }
        else
        {
            bool IsOption = false;
            size_t OptionPrefixLength = 0;

            for (auto& OptionPrefix : OptionPrefixes)
            {
                if (0 == _wcsnicmp(
                    SplitArgument.c_str(),
                    OptionPrefix.c_str(),
                    OptionPrefix.size()))
                {
                    IsOption = true;
                    OptionPrefixLength = OptionPrefix.size();
                }
            }

            if (IsOption)
            {
                // For getting the unresolved command line, we need to cumulate
                // length which including spaces.
                arg_size += SplitArgument.size() + 1;

                // Get the option name and parameter.

                wchar_t* OptionStart = &SplitArgument[0] + OptionPrefixLength;
                wchar_t* ParameterStart = nullptr;

                for (auto& OptionParameterSeparator
                    : OptionParameterSeparators)
                {
                    wchar_t* Result = wcsstr(
                        OptionStart,
                        OptionParameterSeparator.c_str());
                    if (nullptr == Result)
                    {
                        continue;
                    }

                    Result[0] = L'\0';
                    ParameterStart = Result + OptionParameterSeparator.size();

                    break;
                }

                // Save
                OptionsAndParameters[(OptionStart ? OptionStart : L"")] =
                    (ParameterStart ? ParameterStart : L"");
            }
            else
            {
                // Get the approximate location of the unresolved command line.
                // We use "(arg_size - 1)" to ensure that the program path
                // without quotes can also correctly parse.
                wchar_t* search_start =
                    const_cast<wchar_t*>(CommandLine.c_str()) + (arg_size - 1);

                // Get the unresolved command line. Search for the beginning of
                // the first parameter delimiter called space and exclude the
                // first space by adding 1 to the result.
                wchar_t* command = wcsstr(search_start, L" ") + 1;

                // Omit the space. (Thanks to wzzw.)
                while (command && *command == L' ')
                {
                    ++command;
                }

                // Save
                if (command)
                {
                    UnresolvedCommandLine = command;
                }

                break;
            }
        }
    }
}

winrt::hstring FromGuid(
    winrt::guid const& Value)
{
    return winrt::hstring(Mile::FormatWideString(
        L"%08x-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
        Value.Data1,
        Value.Data2,
        Value.Data3,
        Value.Data4[0],
        Value.Data4[1],
        Value.Data4[2],
        Value.Data4[3],
        Value.Data4[4],
        Value.Data4[5],
        Value.Data4[6],
        Value.Data4[7]));
}

std::string ReadAllTextFromUtf8TextFile(
    std::wstring const& Path)
{
    winrt::file_handle FileHandle;

    FileHandle.attach(::MileCreateFile(
        Path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr));
    if (!FileHandle)
    {
        winrt::throw_last_error();
    }

    std::size_t FileSize = 0;
    winrt::check_bool(::MileGetFileSizeByHandle(
        FileHandle.get(),
        &FileSize));

    std::string Content(FileSize, '\0');

    DWORD NumberOfBytesRead = 0;

    winrt::check_bool(::MileReadFile(
        FileHandle.get(),
        const_cast<char*>(Content.c_str()),
        static_cast<DWORD>(FileSize),
        &NumberOfBytesRead));

    if (!(FileSize > 3 &&
        Content[0] == '\xEF' &&
        Content[1] == '\xBB' &&
        Content[2] == '\xBF'))
    {
        throw winrt::hresult_invalid_argument(
            L"UTF-8 with BOM is required.");
    }

    return Content;
}

void WriteAllTextToUtf8TextFile(
    std::wstring const& Path,
    std::string& Content)
{
    winrt::file_handle FileHandle;

    FileHandle.attach(::MileCreateFile(
        Path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        nullptr,
        CREATE_ALWAYS,
        FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr));
    if (!FileHandle)
    {
        winrt::throw_last_error();
    }

    DWORD NumberOfBytesWritten = 0;

    const std::string BOM = "\xEF\xBB\xBF";

    winrt::check_bool(::MileWriteFile(
        FileHandle.get(),
        BOM.c_str(),
        static_cast<DWORD>(BOM.size()),
        &NumberOfBytesWritten));

    winrt::check_bool(::MileWriteFile(
        FileHandle.get(),
        Content.c_str(),
        static_cast<DWORD>(Content.size()),
        &NumberOfBytesWritten));
}

std::wstring GetAbsolutePath(
    std::wstring const& FileName)
{
    // 32767 is the maximum path length without the terminating null character.
    std::wstring Path(32767, L'\0');
    Path.resize(::GetFullPathNameW(
        FileName.c_str(),
        static_cast<DWORD>(Path.size()),
        &Path[0],
        nullptr));
    return Path;
}

HWND CreateXamlDialog(
    _In_opt_ HWND ParentWindowHandle)
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
        ParentWindowHandle,
        nullptr,
        nullptr,
        nullptr);
}

int ShowXamlWindow(
    _In_opt_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
    _In_ LPVOID Content,
    _In_ HWND ParentWindowHandle)
{
    if (!WindowHandle)
    {
        return -1;
    }

    if (FAILED(::MileXamlSetXamlContentForContentWindow(
        WindowHandle,
        Content)))
    {
        return -1;
    }

    UINT DpiValue = ::GetDpiForWindow(WindowHandle);

    int ScaledWidth = ::MulDiv(Width, DpiValue, USER_DEFAULT_SCREEN_DPI);
    int ScaledHeight = ::MulDiv(Height, DpiValue, USER_DEFAULT_SCREEN_DPI);

    RECT ParentRect = { 0 };
    if (ParentWindowHandle)
    {
        ::GetWindowRect(ParentWindowHandle, &ParentRect);
    }
    else
    {
        HMONITOR MonitorHandle = ::MonitorFromWindow(
            WindowHandle,
            MONITOR_DEFAULTTONEAREST);
        if (MonitorHandle)
        {
            MONITORINFO MonitorInfo;
            MonitorInfo.cbSize = sizeof(MONITORINFO);
            if (::GetMonitorInfoW(MonitorHandle, &MonitorInfo))
            {
                ParentRect = MonitorInfo.rcWork;
            }
        }
    }

    int ParentWidth = ParentRect.right - ParentRect.left;
    int ParentHeight = ParentRect.bottom - ParentRect.top;

    ::SetWindowPos(
        WindowHandle,
        nullptr,
        ParentRect.left + ((ParentWidth - ScaledWidth) / 2),
        ParentRect.top + ((ParentHeight - ScaledHeight) / 2),
        ScaledWidth,
        ScaledHeight,
        SWP_NOZORDER | SWP_NOACTIVATE);

    ::ShowWindow(WindowHandle, SW_SHOW);
    ::UpdateWindow(WindowHandle);

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

    return static_cast<int>(Message.wParam);
}

int ShowXamlDialog(
    _In_opt_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
    _In_ LPVOID Content,
    _In_ HWND ParentWindowHandle)
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

    HMENU MenuHandle = ::GetSystemMenu(WindowHandle, FALSE);
    if (MenuHandle)
    {
        ::RemoveMenu(MenuHandle, 0, MF_SEPARATOR);
        ::RemoveMenu(MenuHandle, SC_RESTORE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_SIZE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_MINIMIZE, MF_BYCOMMAND);
        ::RemoveMenu(MenuHandle, SC_MAXIMIZE, MF_BYCOMMAND);
    }

    if (ParentWindowHandle)
    {
        ::EnableWindow(ParentWindowHandle, FALSE);
    }

    int Result = ::ShowXamlWindow(
        WindowHandle,
        Width,
        Height,
        Content,
        ParentWindowHandle);

    if (ParentWindowHandle)
    {
        ::EnableWindow(ParentWindowHandle, TRUE);
        ::SetForegroundWindow(ParentWindowHandle);
        ::SetActiveWindow(ParentWindowHandle);
    }

    return Result;
}

void ShowMessageDialog(
    _In_opt_ HWND ParentWindowHandle,
    _In_ winrt::hstring const& InstructionText,
    _In_ winrt::hstring const& ContentText)
{
    winrt::handle Thread = winrt::handle(Mile::CreateThread([&]()
    {
        winrt::check_hresult(::MileXamlThreadInitialize());

        HWND WindowHandle = ::CreateXamlDialog(ParentWindowHandle);
        if (!WindowHandle)
        {
            return;
        }

        winrt::NanaBox::MessagePage Window =
            winrt::make<winrt::NanaBox::implementation::MessagePage>(
                WindowHandle,
                InstructionText,
                ContentText);
        ::ShowXamlDialog(
            WindowHandle,
            480,
            320,
            winrt::get_abi(Window),
            ParentWindowHandle);

        winrt::check_hresult(::MileXamlThreadUninitialize());
    }));
    ::WaitForSingleObject(Thread.get(), INFINITE);
}

void ShowErrorMessageDialog(
    _In_opt_ HWND ParentWindowHandle,
    _In_ winrt::hresult_error const& Exception)
{
    ::ShowMessageDialog(
        ParentWindowHandle,
        Mile::WinRT::GetLocalizedString(
            L"Messages/ErrorInstructionText"),
        winrt::hstring(Mile::FormatWideString(
            L"[0x%08lX] - %s",
            static_cast<HRESULT>(Exception.code()),
            Exception.message().c_str())));
}

void ShowErrorMessageDialog(
    _In_ winrt::hresult_error const& Exception)
{
    ::ShowErrorMessageDialog(nullptr, Exception);
}

void SimpleRemoveDirectory(
    _In_ LPCWSTR RootPath)
{
    HANDLE RootHandle = ::MileCreateFile(
        RootPath,
        SYNCHRONIZE |
        FILE_LIST_DIRECTORY |
        DELETE |
        FILE_READ_ATTRIBUTES |
        FILE_WRITE_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        nullptr);
    if (RootHandle != INVALID_HANDLE_VALUE)
    {
        Mile::EnumerateFileByHandle(
            RootHandle,
            [&](
                _In_ PMILE_FILE_ENUMERATE_INFORMATION Information) -> BOOL
        {
            if (::MileIsDotsName(Information->FileName))
            {
                return TRUE;
            }

            std::wstring CurrentPath = Mile::FormatWideString(
                L"%s\\%s",
                RootPath,
                Information->FileName);

            if (Information->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                ::SimpleRemoveDirectory(CurrentPath.c_str());
                return TRUE;
            }

            ::MileDeleteFileIgnoreReadonlyAttribute(CurrentPath.c_str());

            return TRUE;
        });

        ::MileDeleteFileIgnoreReadonlyAttributeByHandle(RootHandle);

        ::CloseHandle(RootHandle);
    }
}

std::wstring GetLocalStateFolderPath()
{
    static std::wstring CachedResult = ([]() -> std::wstring
    {
        std::wstring FolderPath;
        {
            LPWSTR RawFolderPath = nullptr;
            // KF_FLAG_FORCE_APP_DATA_REDIRECTION, when engaged, causes
            // SHGetKnownFolderPath to return the new AppModel paths
            // (Packages/xxx/RoamingState, etc.) for standard path requests.
            // Using this flag allows us to avoid
            // Windows.Storage.ApplicationData completely.
            winrt::check_hresult(::SHGetKnownFolderPath(
                FOLDERID_LocalAppData,
                KF_FLAG_FORCE_APP_DATA_REDIRECTION,
                nullptr,
                &RawFolderPath));
            FolderPath = std::wstring(RawFolderPath);
            if (!Mile::WinRT::IsPackagedMode())
            {
                FolderPath += L"\\M2-Team\\NanaBox";
            }
            ::CoTaskMemFree(RawFolderPath);
        }

        // Create the directory if it doesn't exist.
        ::MileCreateDirectory(FolderPath.c_str());

        return FolderPath;
    }());

    return CachedResult;
}

std::wstring GetCurrentProcessModulePath()
{
    // 32767 is the maximum path length without the terminating null character.
    std::wstring Path(32767, L'\0');
    Path.resize(::GetModuleFileNameW(
        nullptr, &Path[0], static_cast<DWORD>(Path.size())));
    return Path;
}

DWORD SimpleCreateVirtualDisk(
    _In_ PCWSTR Path,
    _In_ UINT64 Size,
    _Out_ PHANDLE Handle)
{
    VIRTUAL_STORAGE_TYPE StorageType;
    StorageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    StorageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

    CREATE_VIRTUAL_DISK_PARAMETERS Parameters;
    std::memset(&Parameters, 0, sizeof(CREATE_VIRTUAL_DISK_PARAMETERS));
    Parameters.Version = CREATE_VIRTUAL_DISK_VERSION_2;
    Parameters.Version2.MaximumSize = Size;
    Parameters.Version2.BlockSizeInBytes =
        (0 == ::_wcsicmp(::PathFindExtensionW(Path), L".vhdx"))
        ? 0x100000 // 1 MiB for VHDX
        : 0x80000; // 512 KiB for VHD

    return ::CreateVirtualDisk(
        &StorageType,
        Path,
        VIRTUAL_DISK_ACCESS_NONE,
        nullptr,
        CREATE_VIRTUAL_DISK_FLAG_NONE,
        0,
        &Parameters,
        nullptr,
        Handle);
}

DWORD SimpleResizeVirtualDisk(
    _In_ PCWSTR Path,
    _In_ UINT64 Size)
{
    HANDLE DiskHandle = INVALID_HANDLE_VALUE;

    VIRTUAL_STORAGE_TYPE StorageType;
    StorageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    StorageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

    DWORD Error = ::OpenVirtualDisk(
        &StorageType,
        Path,
        VIRTUAL_DISK_ACCESS_ALL,
        OPEN_VIRTUAL_DISK_FLAG_NONE,
        nullptr,
        &DiskHandle);
    if (ERROR_SUCCESS == Error)
    {
        RESIZE_VIRTUAL_DISK_PARAMETERS Parameters;
        std::memset(&Parameters, 0, sizeof(RESIZE_VIRTUAL_DISK_PARAMETERS));
        Parameters.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
        Parameters.Version1.NewSize = Size;

        Error = ::ResizeVirtualDisk(
            DiskHandle,
            RESIZE_VIRTUAL_DISK_FLAG_NONE,
            &Parameters,
            nullptr);

        ::CloseHandle(DiskHandle);
    }
    return Error;
}


DWORD SimpleCompactVirtualDisk(
    _In_ PCWSTR Path)
{
    HANDLE DiskHandle = INVALID_HANDLE_VALUE;

    VIRTUAL_STORAGE_TYPE StorageType;
    StorageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
    StorageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

    DWORD Error = ::OpenVirtualDisk(
        &StorageType,
        Path,
        VIRTUAL_DISK_ACCESS_ALL,
        OPEN_VIRTUAL_DISK_FLAG_NONE,
        nullptr,
        &DiskHandle);
    if (ERROR_SUCCESS == Error)
    {
        Error = ::CompactVirtualDisk(
            DiskHandle,
            COMPACT_VIRTUAL_DISK_FLAG_NONE,
            nullptr,
            nullptr);

        ::CloseHandle(DiskHandle);
    }
    return Error;
}

winrt::handle ShowAboutDialog(
    _In_ HWND ParentWindowHandle)
{
    return winrt::handle(Mile::CreateThread([=]()
    {
        winrt::check_hresult(::MileXamlThreadInitialize());

        HWND WindowHandle = ::CreateXamlDialog(ParentWindowHandle);
        if (!WindowHandle)
        {
            return;
        }

        winrt::NanaBox::AboutPage Window =
            winrt::make<winrt::NanaBox::implementation::AboutPage>(
                WindowHandle);
        ::ShowXamlDialog(
            WindowHandle,
            480,
            320,
            winrt::get_abi(Window),
            ParentWindowHandle);

        winrt::check_hresult(::MileXamlThreadUninitialize());
    }));
}

winrt::handle ShowNewVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle)
{
    return winrt::handle(Mile::CreateThread([=]()
    {
        winrt::check_hresult(::MileXamlThreadInitialize());

        HWND WindowHandle = ::CreateXamlDialog(ParentWindowHandle);
        if (!WindowHandle)
        {
            return;
        }

        winrt::NanaBox::NewVirtualHardDiskPage Window =
            winrt::make<winrt::NanaBox::implementation::NewVirtualHardDiskPage>(
                WindowHandle);
        ::ShowXamlDialog(
            WindowHandle,
            480,
            320,
            winrt::get_abi(Window),
            ParentWindowHandle);

        winrt::check_hresult(::MileXamlThreadUninitialize());
    }));
}

winrt::handle ShowCompactVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle)
{
    return winrt::handle(Mile::CreateThread([=]()
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

            COMDLG_FILTERSPEC SupportedFileTypes[] =
            {
                { L"VHDX (*.vhdx)", L"*.vhdx" },
                { L"VHD (*.vhd)", L"*.vhd" }
            };

            winrt::check_hresult(FileDialog->SetFileTypes(
                ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

            winrt::check_hresult(FileDialog->SetTitle(
                Mile::WinRT::GetLocalizedString(
                    L"CompactVirtualHardDiskWizard/PathTip").c_str()));

            // Note: The array is 1-indexed
            winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

            winrt::check_hresult(FileDialog->SetDefaultExtension(L"vhdx"));

            winrt::check_hresult(FileDialog->Show(ParentWindowHandle));

            winrt::hstring FilePath;
            {
                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                LPWSTR RawFilePath = nullptr;
                winrt::check_hresult(Result->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &RawFilePath));
                if (RawFilePath)
                {
                    FilePath = winrt::to_hstring(RawFilePath);
                    ::CoTaskMemFree(RawFilePath);
                }
            }

            winrt::hstring SuccessInstructionText =
                Mile::WinRT::GetLocalizedString(
                    L"CompactVirtualHardDiskWizard/SuccessInstructionText");
            winrt::hstring SuccessContentText =
                Mile::WinRT::GetLocalizedString(
                    L"CompactVirtualHardDiskWizard/SuccessContextText");

            try
            {
                HWND WaitingHandle = ::ShowOperationWaitingWindow(ParentWindowHandle);

                DWORD Error = SimpleCompactVirtualDisk(FilePath.c_str());

                if (ERROR_SUCCESS == Error)
                {
                    ::ShowMessageDialog(
                        ParentWindowHandle,
                        SuccessInstructionText.c_str(),
                        Mile::FormatWideString(
                            SuccessContentText.c_str(),
                            FilePath.c_str()).c_str());
                }
                else
                {
                    ::ShowErrorMessageDialog(
                        ParentWindowHandle,
                        winrt::hresult_error(HRESULT_FROM_WIN32(Error)));
                }

                ::SendMessageW(WaitingHandle, WM_CLOSE, 0, 0);
            }
            catch (...)
            {

            }
        }
        catch (...)
        {

        }
    }));
}

winrt::handle ShowResizeVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle)
{
    return winrt::handle(Mile::CreateThread([=]()
    {
        winrt::check_hresult(::MileXamlThreadInitialize());

        HWND WindowHandle = ::CreateXamlDialog(ParentWindowHandle);
        if (!WindowHandle)
        {
            return;
        }

        winrt::NanaBox::ResizeVirtualHardDiskPage Window =
            winrt::make<winrt::NanaBox::implementation::ResizeVirtualHardDiskPage>(
                WindowHandle);
        ::ShowXamlDialog(
            WindowHandle,
            480,
            320,
            winrt::get_abi(Window),
            ParentWindowHandle);

        winrt::check_hresult(::MileXamlThreadUninitialize());
    }));
}

BOOL LaunchDocumentation()
{
    SHELLEXECUTEINFOW ExecInfo = { 0 };
    ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
    ExecInfo.lpVerb = L"open";
    ExecInfo.lpFile =
        L"https://github.com/M2Team/NanaBox/"
        L"blob/main/Documents/ConfigurationReference.md";
    ExecInfo.nShow = SW_SHOWNORMAL;
    return ::ShellExecuteExW(&ExecInfo);
}

std::string GetCurrentProcessUserStringSid()
{
    static std::string CachedResult = ([]() -> std::string
    {
        std::string Result;

        HANDLE CurrentProcessToken = nullptr;
        if (::OpenProcessToken(
            ::GetCurrentProcess(),
            TOKEN_ALL_ACCESS,
            &CurrentProcessToken))
        {
            DWORD Length = 0;
            ::GetTokenInformation(
                CurrentProcessToken,
                TOKEN_INFORMATION_CLASS::TokenUser,
                nullptr,
                0,
                &Length);
            if (ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
            {
                PTOKEN_USER Information = reinterpret_cast<PTOKEN_USER>(
                    ::MileAllocateMemory(Length));
                if (Information)
                {
                    if (::GetTokenInformation(
                        CurrentProcessToken,
                        TOKEN_INFORMATION_CLASS::TokenUser,
                        Information,
                        Length,
                        &Length))
                    {
                        LPWSTR StringSid = nullptr;
                        if (::ConvertSidToStringSidW(
                            Information->User.Sid,
                            &StringSid))
                        {
                            Result = Mile::ToString(
                                CP_UTF8,
                                std::wstring(StringSid));
                            ::LocalFree(StringSid);
                        }
                    }

                    ::MileFreeMemory(Information);
                }
            }

            ::CloseHandle(CurrentProcessToken);
        }

        return Result;
    }());

    return CachedResult;
}

HWND ShowOperationWaitingWindow(
    _In_ HWND ParentWindowHandle)
{
    HWND Result = nullptr;

    HANDLE ReturnEvent = ::CreateEventExW(
        nullptr,
        nullptr,
        0,
        EVENT_ALL_ACCESS);
    if (!ReturnEvent)
    {
        return nullptr;
    }

    ::CloseHandle(Mile::CreateThread([=, &Result]()
    {
        winrt::check_hresult(::MileXamlThreadInitialize());

        HWND WindowHandle = ::CreateWindowExW(
            0,
            L"Mile.Xaml.ContentWindow",
            nullptr,
            WS_POPUP,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            ParentWindowHandle,
            nullptr,
            nullptr,
            nullptr);
        if (!WindowHandle)
        {
            return;
        }
        Result = WindowHandle;

        ::SetEvent(ReturnEvent);

        DWM_WINDOW_CORNER_PREFERENCE Preference = DWMWCP_ROUNDSMALL;
        ::DwmSetWindowAttribute(
            WindowHandle,
            DWMWA_WINDOW_CORNER_PREFERENCE,
            &Preference,
            sizeof(DWM_WINDOW_CORNER_PREFERENCE));

        winrt::ProgressRing Content = winrt::ProgressRing();
        Content.Margin(winrt::ThicknessHelper::FromUniformLength(12));
        Content.HorizontalAlignment(winrt::HorizontalAlignment::Stretch);
        Content.VerticalAlignment(winrt::VerticalAlignment::Stretch);
        // Content.IsActive(true);

        ::ShowXamlDialog(
            WindowHandle,
            72,
            72,
            winrt::get_abi(Content),
            ParentWindowHandle);

        winrt::check_hresult(::MileXamlThreadUninitialize());
    }));

    ::WaitForSingleObjectEx(ReturnEvent, INFINITE, FALSE);

    return Result;
}
