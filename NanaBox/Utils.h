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

winrt::hstring FromGuid(
    winrt::guid const& Value);

std::string ReadAllTextFromUtf8TextFile(
    std::wstring const& Path);

void WriteAllTextToUtf8TextFile(
    std::wstring const& Path,
    std::string& Content);

#include <Mile.Helpers.CppBase.h>
#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}
