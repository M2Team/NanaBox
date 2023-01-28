#pragma once

#include "MinimalExpander.g.h"

namespace winrt::NanaBox::implementation
{
    struct MinimalExpander : MinimalExpanderT<MinimalExpander>
    {
        MinimalExpander();

        winrt::Windows::Foundation::IInspectable Header();
        void Header(winrt::Windows::Foundation::IInspectable value);

        winrt::Windows::UI::Xaml::DataTemplate HeaderTemplate();
        void HeaderTemplate(winrt::Windows::UI::Xaml::DataTemplate value);

        bool IsExpanded();
        void IsExpanded(bool value);

        void OnApplyTemplate();
        void UpdateExpandState(bool useTransitions);

        static winrt::Windows::UI::Xaml::DependencyProperty HeaderProperty();
        static winrt::Windows::UI::Xaml::DependencyProperty HeaderTemplateProperty();
        static winrt::Windows::UI::Xaml::DependencyProperty IsExpandedProperty();

        static void OnIsExpandedPropertyChanged(
            winrt::Windows::UI::Xaml::DependencyObject const& sender,
            winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs const& args);

    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MinimalExpander : MinimalExpanderT<MinimalExpander, implementation::MinimalExpander>
    {
    };
}
