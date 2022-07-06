#pragma once

#include "MainWindowControl.g.h"

namespace winrt::NanaBox::implementation
{
    struct MainWindowControl : MainWindowControlT<MainWindowControl>
    {
        MainWindowControl();
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MainWindowControl : MainWindowControlT<
        MainWindowControl,
        implementation::MainWindowControl>
    {
    };
}
