#pragma once

#include "MainWindowExitNoticeControl.g.h"

namespace winrt::NanaBox::implementation
{
    struct MainWindowExitNoticeControl : MainWindowExitNoticeControlT<
        MainWindowExitNoticeControl>
    {
        MainWindowExitNoticeControl();
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MainWindowExitNoticeControl : MainWindowExitNoticeControlT<
        MainWindowExitNoticeControl,
        implementation::MainWindowExitNoticeControl>
    {
    };
}
