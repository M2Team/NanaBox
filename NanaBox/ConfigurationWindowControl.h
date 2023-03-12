#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ConfigurationWindowControl.g.h"

namespace winrt
{
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Interop::TypeName;
    using Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs;
    using Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo;
}

namespace winrt::NanaBox::implementation
{
    struct ConfigurationWindowControl : ConfigurationWindowControlT<ConfigurationWindowControl>
    {
        ConfigurationWindowControl();
        void InitializeComponent();

        void NavigationView_Navigate(std::wstring navItemTag,
            IInspectable const& parameter,
            NavigationTransitionInfo const& transitionInfo);
        void NavigationView_SelectionChanged(IInspectable const& sender,
            NavigationViewSelectionChangedEventArgs const& args);
        void NavigationView_Loaded(IInspectable const& sender, RoutedEventArgs const& e);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
        void ViewModel(winrt::NanaBox::ConfigurationViewModel const& value);
    private:
        std::vector<std::pair<std::wstring, TypeName>> m_pages;
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ConfigurationWindowControl : ConfigurationWindowControlT<ConfigurationWindowControl, implementation::ConfigurationWindowControl>
    {
    };
}
