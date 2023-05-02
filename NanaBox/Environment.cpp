#include "Environment.h"

#include <ShlObj.h>

namespace winrt
{
    using Windows::ApplicationModel::Package;
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
