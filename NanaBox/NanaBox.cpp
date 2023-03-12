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
#include "ConfigurationWindowControl.h"

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>

#include <json.hpp>

#include <Mile.Project.Version.h>

#include "NanaBoxResources.h"

#include <Mile.Helpers.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace winrt
{
    using Windows::ApplicationModel::Package;
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceManager;
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceMap;
    using Windows::UI::Xaml::ElementTheme;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::UIElement;
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
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

namespace
{
    std::vector<std::wstring> SpiltCommandLine(
        std::wstring const& CommandLine)
    {
        // Initialize the SplitArguments.
        std::vector<std::wstring> SplitArguments;

        wchar_t c = L'\0';
        int copy_character;                   /* 1 = copy char to *args */
        unsigned numslash;              /* num of backslashes seen */

        std::wstring Buffer;
        Buffer.reserve(CommandLine.size());

        /* first scan the program name, copy it, and count the bytes */
        wchar_t* p = const_cast<wchar_t*>(CommandLine.c_str());

        // A quoted program name is handled here. The handling is much simpler than
        // for other arguments. Basically, whatever lies between the leading
        // double-quote and next one, or a terminal null character is simply
        // accepted. Fancier handling is not required because the program name must
        // be a legal NTFS/HPFS file name. Note that the double-quote characters
        // are not copied, nor do they contribute to character_count.
        bool InQuotes = false;
        do
        {
            if (*p == L'"')
            {
                InQuotes = !InQuotes;
                c = *p++;
                continue;
            }

            // Copy character into argument:
            Buffer.push_back(*p);

            c = *p++;
        } while (c != L'\0' && (InQuotes || (c != L' ' && c != L'\t')));

        if (c == L'\0')
        {
            p--;
        }
        else
        {
            Buffer.resize(Buffer.size() - 1);
        }

        // Save te argument.
        SplitArguments.push_back(Buffer);

        InQuotes = false;

        // Loop on each argument
        for (;;)
        {
            if (*p)
            {
                while (*p == L' ' || *p == L'\t')
                    ++p;
            }

            // End of arguments
            if (*p == L'\0')
                break;

            // Initialize the argument buffer.
            Buffer.clear();

            // Loop through scanning one argument:
            for (;;)
            {
                copy_character = 1;

                // Rules: 2N backslashes + " ==> N backslashes and begin/end quote
                // 2N + 1 backslashes + " ==> N backslashes + literal " N
                // backslashes ==> N backslashes
                numslash = 0;

                while (*p == L'\\')
                {
                    // Count number of backslashes for use below
                    ++p;
                    ++numslash;
                }

                if (*p == L'"')
                {
                    // if 2N backslashes before, start/end quote, otherwise copy
                    // literally:
                    if (numslash % 2 == 0)
                    {
                        if (InQuotes && p[1] == L'"')
                        {
                            p++; // Double quote inside quoted string
                        }
                        else
                        {
                            // Skip first quote char and copy second:
                            copy_character = 0; // Don't copy quote
                            InQuotes = !InQuotes;
                        }
                    }

                    numslash /= 2;
                }

                // Copy slashes:
                while (numslash--)
                {
                    Buffer.push_back(L'\\');
                }

                // If at end of arg, break loop:
                if (*p == L'\0' || (!InQuotes && (*p == L' ' || *p == L'\t')))
                    break;

                // Copy character into argument:
                if (copy_character)
                {
                    Buffer.push_back(*p);
                }

                ++p;
            }

            // Save te argument.
            SplitArguments.push_back(Buffer);
        }

        return SplitArguments;
    }

    void SpiltCommandLineEx(
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
        for (auto& SplitArgument : ::SpiltCommandLine(CommandLine))
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

    winrt::hstring VFormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        _In_z_ _Printf_format_string_ va_list ArgList)
    {
        // Check the argument list.
        if (Format)
        {
            // Get the length of the format result.
            size_t nLength =
                static_cast<size_t>(::_vscwprintf(Format, ArgList)) + 1;

            // Allocate for the format result.
            std::wstring Buffer(nLength + 1, L'\0');

            // Format the string.
            int nWritten = ::_vsnwprintf_s(
                &Buffer[0],
                Buffer.size(),
                nLength,
                Format,
                ArgList);

            if (nWritten > 0)
            {
                // If succeed, resize to fit and return result.
                Buffer.resize(nWritten);
                return winrt::hstring(Buffer);
            }
        }

        // If failed, return an empty string.
        return winrt::hstring();
    }

    winrt::hstring FormatWindowsRuntimeString(
        _In_z_ _Printf_format_string_ wchar_t const* const Format,
        ...)
    {
        va_list ArgList;
        va_start(ArgList, Format);
        winrt::hstring Result = ::VFormatWindowsRuntimeString(
            Format,
            ArgList);
        va_end(ArgList);
        return Result;
    }

    winrt::hstring FromGuid(
        winrt::guid const& Value)
    {
        return ::FormatWindowsRuntimeString(
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
            Value.Data4[7]);
    }

    std::size_t GetTextFileSize(
        winrt::file_handle const& FileHandle)
    {
        FILE_STANDARD_INFO StandardInfo;

        winrt::check_bool(::GetFileInformationByHandleEx(
            FileHandle.get(),
            FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
            &StandardInfo,
            sizeof(FILE_STANDARD_INFO)));

        return static_cast<std::size_t>(StandardInfo.EndOfFile.QuadPart);
    }

    static std::string ReadAllTextFromUtf8TextFile(
        std::wstring const& Path)
    {
        winrt::file_handle FileHandle;

        FileHandle.attach(::CreateFileW(
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

        std::size_t FileSize = ::GetTextFileSize(FileHandle);

        std::string Content(FileSize, '\0');

        DWORD NumberOfBytesRead = 0;

        winrt::check_bool(::ReadFile(
            FileHandle.get(),
            const_cast<char*>(Content.c_str()),
            static_cast<DWORD>(FileSize),
            &NumberOfBytesRead,
            nullptr));

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

    static void WriteAllTextToUtf8TextFile(
        std::wstring const& Path,
        std::string& Content)
    {
        winrt::file_handle FileHandle;

        FileHandle.attach(::CreateFileW(
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

        winrt::check_bool(::WriteFile(
            FileHandle.get(),
            BOM.c_str(),
            static_cast<DWORD>(BOM.size()),
            &NumberOfBytesWritten,
            nullptr));

        winrt::check_bool(::WriteFile(
            FileHandle.get(),
            Content.c_str(),
            static_cast<DWORD>(Content.size()),
            &NumberOfBytesWritten,
            nullptr));
    }

    bool IsPackagedMode()
    {
        static bool CachedResult = ([]() -> bool
        {
            try
            {
                const auto CurrentPackage = winrt::Package::Current();
                return true;
            }
            catch (...)
            {
                return false;
            }
        }());

        return CachedResult;
    }

    std::filesystem::path GetLocalStateFolderPath()
    {
        static std::filesystem::path CachedResult = ([]() -> std::filesystem::path
        {
            std::filesystem::path FolderPath;
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
                FolderPath = std::filesystem::path(RawFolderPath);
                if (!::IsPackagedMode())
                {
                    FolderPath /= L"M2-Team\\NanaBox";
                }
                ::CoTaskMemFree(RawFolderPath);
            }

            // Create the directory if it doesn't exist.
            std::filesystem::create_directories(FolderPath);

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

    bool IsCurrentProcessElevated()
    {
        bool Result = false;

        HANDLE CurrentProcessAccessToken = nullptr;
        if (::OpenProcessToken(
            ::GetCurrentProcess(),
            TOKEN_ALL_ACCESS,
            &CurrentProcessAccessToken))
        {
            TOKEN_ELEVATION Information = { 0 };
            DWORD Length = sizeof(Information);
            if (::GetTokenInformation(
                CurrentProcessAccessToken,
                TOKEN_INFORMATION_CLASS::TokenElevation,
                &Information,
                Length,
                &Length))
            {
                Result = Information.TokenIsElevated;
            }

            ::CloseHandle(CurrentProcessAccessToken);
        }

        return Result;
    }

    winrt::hstring GetLocalizedString(
        winrt::hstring const& ResourcePath,
        winrt::hstring const& FallbackString)
    {
        try
        {
            winrt::ResourceMap CurrentResourceMap =
                winrt::ResourceManager::Current().MainResourceMap();

            if (CurrentResourceMap.HasKey(ResourcePath))
            {
                return CurrentResourceMap.Lookup(
                    ResourcePath).Candidates().GetAt(0).ValueAsString();
            }
            else
            {
                return FallbackString;
            }
        }
        catch (...)
        {
            return FallbackString;
        }
    }

    winrt::hstring GetLocalizedString(
        winrt::hstring const& ResourcePath)
    {
        return ::GetLocalizedString(ResourcePath, ResourcePath);
    }
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

    this->m_RdpClient->OnRemoteDesktopSizeChange([this](
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
    this->m_RdpClient->OnLoginComplete([this]()
    {
        if (this->m_RdpClientMode == RdpClientMode::EnhancedSession)
        {
            this->m_RdpClientMode = RdpClientMode::EnhancedVideoSyncedSession;
            this->m_DisplayResolution = CSize();
        }
    });
    this->m_RdpClient->OnDisconnected([this](
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
    this->m_RdpClient->OnRequestGoFullScreen([this]()
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
    this->m_RdpClient->OnRequestLeaveFullScreen([this]()
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
    this->m_RdpClient->OnRequestContainerMinimize([this]()
    {
        this->ShowWindow(SW_MINIMIZE);
    });
    this->m_RdpClient->OnConfirmClose([this](
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

    this->m_VirtualMachine->SystemExited([this](
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

    /*this->m_VirtualMachine->SystemRdpEnhancedModeStateChanged([this]()
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

namespace NanaBox
{

    class ConfigurationWindow :
        public ATL::CWindowImpl<ConfigurationWindow>,
        public WTL::CMessageFilter
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaBoxVirtualMachineConfigurationWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(VirtualMachineConfigurationWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_SETTINGCHANGE(OnSettingChange)
        END_MSG_MAP()

        virtual BOOL PreTranslateMessage(
            MSG* pMsg);

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnSize(
            UINT nType,
            CSize size);

        void OnSetFocus(
            ATL::CWindow wndOld);

        void OnSettingChange(
            UINT uFlags,
            LPCTSTR lpszSection);

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaBox::ConfigurationWindowControl m_ConfigurationControl = nullptr;
        NanaBox::VirtualMachineConfiguration m_Configuration;
    };

}

BOOL NanaBox::ConfigurationWindow::PreTranslateMessage(
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

int NanaBox::ConfigurationWindow::OnCreate(
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


    this->m_ConfigurationControl =
        winrt::make<winrt::NanaBox::implementation::ConfigurationWindowControl>();
    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();
    winrt::check_hresult(
        XamlSourceNative->AttachToWindow(this->m_hWnd));
    this->m_XamlSource.Content(this->m_ConfigurationControl);

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
    return 0;
}


void NanaBox::ConfigurationWindow::OnSize(
    UINT nType,
    CSize size)
{
    if (nType == SIZE_MINIMIZED)
    {
        return;
    }

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
        size.cx,
        size.cy,
        SWP_SHOWWINDOW);
}


void NanaBox::ConfigurationWindow::OnSetFocus(
    ATL::CWindow wndOld)
{
    UNREFERENCED_PARAMETER(wndOld);
    winrt::com_ptr<IDesktopWindowXamlSourceNative> XamlSourceNative =
        this->m_XamlSource.as<IDesktopWindowXamlSourceNative>();

    HWND XamlWindowHandle = nullptr;
    winrt::check_hresult(
        XamlSourceNative->get_WindowHandle(&XamlWindowHandle));
    ::SetFocus(XamlWindowHandle);
}

void NanaBox::ConfigurationWindow::OnSettingChange(
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
                        AppBinaryPath / L"Mile.Xaml.dll",
                        TempBinaryPath / L"Mile.Xaml.dll");

                    std::filesystem::copy_file(
                        AppBinaryPath / L"Mile.Xaml.Styles.SunValley.xbf",
                        TempBinaryPath / L"Mile.Xaml.Styles.SunValley.xbf");

                    std::filesystem::copy_file(
                        AppBinaryPath / L"Mile.Xaml.winmd",
                        TempBinaryPath / L"Mile.Xaml.winmd");

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

    winrt::NanaBox::App app =
        winrt::make<winrt::NanaBox::implementation::App>();

    WTL::CMessageLoop MessageLoop;

    ATL::AtlAxWinInit();

    g_Module.Init(nullptr, hInstance);
    g_Module.AddMessageLoop(&MessageLoop);

    std::unique_ptr<NanaBox::ConfigurationWindow> ConfigurationWindow = nullptr;
    std::unique_ptr<NanaBox::MainWindow> MainWindow = nullptr;

    if (ShowVirtualMachineConfig)
    {
        OutputDebugStringW(g_ConfigurationFilePath.c_str());
        OutputDebugStringW(L"\n");

        ConfigurationWindow = std::make_unique<NanaBox::ConfigurationWindow>();
        if (!ConfigurationWindow->Create(
            nullptr,
            ConfigurationWindow->rcDefault,
            g_WindowTitle.data(),
            WS_OVERLAPPEDWINDOW))
        {
            return -1;
        }

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

    app.Close();

    return Result;
}
