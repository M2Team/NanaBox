#pragma once

#include <Windows.h>

// https://github.com/microsoft/terminal/blob
// /c727762602b8bd12e4a3a769053204d7e92b81c5
// /src/cascadia/WindowsTerminalUniversal/pch.h#L12
// This is inexplicable, but for whatever reason, cppwinrt conflicts with the
// SDK definition of this function, so the only fix is to undef it.
// from WinBase.h
// Windows::UI::Xaml::Media::Animation::IStoryboard::GetCurrentTime
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <WinUser.h>

#include <unknwn.h> // To enable support for non-WinRT interfaces, unknwn.h must be included before any C++/WinRT headers.

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.System.h>
