#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ConfigurationWindowControl.g.h"

namespace winrt::NanaBox::implementation
{
    struct ConfigurationWindowControl : ConfigurationWindowControlT<ConfigurationWindowControl>
    {
        ConfigurationWindowControl();

        void NavigationView_Navigate(
            std::wstring navItemTag,
            Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo);
        void NavigationView_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void NavigationView_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        std::vector<std::pair<std::wstring, winrt::Windows::UI::Xaml::Interop::TypeName>> m_pages;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ConfigurationWindowControl : ConfigurationWindowControlT<ConfigurationWindowControl, implementation::ConfigurationWindowControl>
    {
    };
}
