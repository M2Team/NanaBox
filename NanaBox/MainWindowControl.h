#pragma once

#include "MainWindowControl.g.h"

namespace winrt::NanaBox::implementation
{
    struct MainWindowControl : MainWindowControlT<MainWindowControl>
    {
        MainWindowControl();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MainWindowControl : MainWindowControlT<MainWindowControl, implementation::MainWindowControl>
    {
    };
}
