#pragma once
#include <Windows.h>
#include <functional>
#include <winrt/Windows.Storage.Streams.h>

namespace winrt
{
    using Windows::Storage::Streams::IRandomAccessStream;
}

namespace NanaBox
{
    winrt::IRandomAccessStream CaptureVirtualMachine(
        _In_ HWND WindowHandle,
        _In_ HWND ToolbarHandle,
        _In_opt_ HANDLE StopEvent,
        std::wstring const& FilePath,
        std::function<void()> const& RecordingStarted);
}
