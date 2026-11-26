#pragma once

#include "pch.h"

#include <ShlObj.h>

#include <vector>
#include <string>
#include <winrt/base.h>

namespace NanaBox
{
    class MainWindow;
}

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

std::string GetAbsoluteUtf8Path(
    std::string const& FileName);

/**
 * @brief Shows a single-file Open or Save dialog. COM must already be
 *        initialized on the calling thread.
 * @param ParentWindowHandle The optional owner window.
 * @param SaveMode TRUE for Save mode; FALSE for Open mode.
 * @param FileTypeCount The nonzero number of file type filters.
 * @param FileTypes The file type filters. The first type is selected initially.
 * @param Title The optional dialog title.
 * @param DefaultExtension The optional default extension, without a period.
 * @param DefaultFileName The optional initial file name.
 * @param FilePath Receives the selected path allocated by the Shell.
 *                 The caller must free it with CoTaskMemFree. Set to nullptr
 *                 on failure, including cancellation.
 * @return S_OK on success, HRESULT_FROM_WIN32(ERROR_CANCELLED) on cancellation,
 *         or another HRESULT error code on failure.
 */
HRESULT ShowFileDialog(
    _In_opt_ HWND ParentWindowHandle,
    _In_ BOOL SaveMode,
    _In_ UINT FileTypeCount,
    _In_reads_(FileTypeCount) COMDLG_FILTERSPEC const* FileTypes,
    _In_opt_ LPCWSTR Title,
    _In_opt_ LPCWSTR DefaultExtension,
    _In_opt_ LPCWSTR DefaultFileName,
    _Outptr_result_nullonfailure_ LPWSTR* FilePath);

void SetForegroundWindowSimple(
    _In_ HWND WindowHandle);

HWND CreateXamlWindow(
    _In_opt_ HWND ParentWindowHandle,
    _In_ DWORD ExtendedWindowStyle,
    _In_ DWORD WindowStyle);

HWND CreateXamlDialog(
    _In_opt_ HWND ParentWindowHandle);

int ShowXamlWindow(
    _In_opt_ HWND WindowHandle,
    _In_ int Width,
    _In_ int Height,
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

DWORD SimpleCompactVirtualDisk(
    _In_ PCWSTR Path);

winrt::handle ShowAboutDialog(
    _In_ HWND ParentWindowHandle);

winrt::handle ShowVirtualMachineSettingsWindow(
    _In_ HWND ParentWindowHandle,
    std::wstring const& ConfigurationFilePath = std::wstring(),
    std::wstring const& Name = std::wstring(),
    _In_opt_ NanaBox::MainWindow* MainWindow = nullptr);

winrt::handle ShowNewVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle);

winrt::handle ShowCompactVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle);

winrt::handle ShowResizeVirtualHardDiskDialog(
    _In_ HWND ParentWindowHandle);

BOOL LaunchDocumentation();

std::string GetCurrentProcessUserStringSid();

HWND ShowOperationWaitingWindow(
    _In_ HWND ParentWindowHandle);

#include <Mile.Helpers.CppBase.h>
#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}
