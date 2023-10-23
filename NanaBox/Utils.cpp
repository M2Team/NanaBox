#include "Utils.h"

namespace winrt
{
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceManager;
    using winrt::Windows::ApplicationModel::Resources::Core::ResourceMap;
}

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

std::string ReadAllTextFromUtf8TextFile(
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

void WriteAllTextToUtf8TextFile(
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
