#include "Utils.h"

namespace winrt
{
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceManager;
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceMap;
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
