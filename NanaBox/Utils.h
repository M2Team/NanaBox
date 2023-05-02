#pragma once

#include "pch.h"

#include <vector>
#include <string>
#include <winrt/base.h>

std::vector<std::wstring> SpiltCommandLine(
    std::wstring const& CommandLine);

void SpiltCommandLineEx(
    std::wstring const& CommandLine,
    std::vector<std::wstring> const& OptionPrefixes,
    std::vector<std::wstring> const& OptionParameterSeparators,
    std::wstring& ApplicationName,
    std::map<std::wstring, std::wstring>& OptionsAndParameters,
    std::wstring& UnresolvedCommandLine);

winrt::hstring VFormatWindowsRuntimeString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    _In_z_ _Printf_format_string_ va_list ArgList);

winrt::hstring FormatWindowsRuntimeString(
    _In_z_ _Printf_format_string_ wchar_t const* const Format,
    ...);

winrt::hstring FromGuid(
    winrt::guid const& Value);

std::string ReadAllTextFromUtf8TextFile(
    std::wstring const& Path);

void WriteAllTextToUtf8TextFile(
    std::wstring const& Path,
    std::string& Content);

winrt::hstring GetLocalizedString(
    winrt::hstring const& ResourcePath,
    winrt::hstring const& FallbackString);

winrt::hstring GetLocalizedString(
    winrt::hstring const& ResourcePath);
