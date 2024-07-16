#pragma once

#include "pch.h"

#include <vector>
#include <string>
#include <winrt/base.h>

void SplitCommandLineEx(
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

std::wstring GetAbsolutePath(
    std::wstring const& FileName);

HWND CreateXamlDialog(
    _In_opt_ HWND ParentWindowHandle);

int ShowXamlWindow(
    _In_opt_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
    _In_ LPVOID Content,
    _In_ HWND ParentWindowHandle);

int ShowXamlDialog(
    _In_opt_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
    _In_ LPVOID Content,
    _In_ HWND ParentWindowHandle);

void ShowMessageDialog(
    _In_opt_ HWND ParentWindowHandle,
    _In_ winrt::hstring const& InstructionText,
    _In_ winrt::hstring const& ContentText);

void ShowErrorMessageDialog(
    _In_opt_ HWND ParentWindowHandle,
    _In_ winrt::hresult_error const& Exception);

void ShowErrorMessageDialog(
    _In_ winrt::hresult_error const& Exception);

void SimpleRemoveDirectory(
    _In_ LPCWSTR RootPath);

std::wstring GetLocalStateFolderPath();

std::wstring GetCurrentProcessModulePath();

DWORD SimpleCreateVirtualDisk(
    _In_ PCWSTR Path,
    _In_ UINT64 Size,
    _Out_ PHANDLE Handle);

DWORD SimpleResizeVirtualDisk(
    _In_ PCWSTR Path,
    _In_ UINT64 Size);

winrt::handle ShowAboutDialog(
    _In_ HWND ParentWindowHandle);

winrt::handle ShowNewVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle);

winrt::handle ShowResizeVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle);

BOOL LaunchDocumentation();

std::string GetCurrentProcessUserStringSid();

#include <Mile.Helpers.CppBase.h>
#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}
