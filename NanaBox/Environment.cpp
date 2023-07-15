#include "Environment.h"

#include <ShlObj.h>

#include <Mile.Helpers.CppWinRT.h>

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
                if (!Mile::WinRT::IsPackagedMode())
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
