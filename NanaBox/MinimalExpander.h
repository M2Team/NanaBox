#pragma once

#include "MinimalExpander.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::DataTemplate;
    using Windows::UI::Xaml::DependencyObject;
    using Windows::UI::Xaml::DependencyProperty;
    using Windows::UI::Xaml::DependencyPropertyChangedEventArgs;
    using Windows::UI::Xaml::SizeChangedEventArgs;
    using Windows::UI::Xaml::Controls::Border;
}

namespace winrt::NanaBox::implementation
{
    struct MinimalExpander : MinimalExpanderT<MinimalExpander>
    {
        MinimalExpander();

        winrt::IInspectable Header();
        void Header(winrt::IInspectable value);

        winrt::DataTemplate HeaderTemplate();
        void HeaderTemplate(winrt::DataTemplate value);

        bool IsExpanded();
        void IsExpanded(bool value);

        double NegativeContentHeight();
        void NegativeContentHeight(double value);

        void OnApplyTemplate();
        void UpdateExpandState(bool useTransitions);

        static winrt::DependencyProperty HeaderProperty() { return _headerProperty; }
        static winrt::DependencyProperty HeaderTemplateProperty() { return _headerTemplateProperty; }
        static winrt::DependencyProperty IsExpandedProperty() { return _isExpandedProperty; }
        static winrt::DependencyProperty NegativeContentHeightProperty() { return _negativeContentHeightProperty; }

        static void OnIsExpandedPropertyChanged(
            winrt::DependencyObject const& sender,
            winrt::DependencyPropertyChangedEventArgs const& args);

    private:
        static winrt::DependencyProperty _headerProperty;
        static winrt::DependencyProperty _headerTemplateProperty;
        static winrt::DependencyProperty _isExpandedProperty;
        static winrt::DependencyProperty _negativeContentHeightProperty;

        void OnContentSizeChanged(
            winrt::IInspectable const& sender,
            winrt::SizeChangedEventArgs const& args
        );
        winrt::Border::SizeChanged_revoker m_contentSizeChangedRevoker{};
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MinimalExpander : MinimalExpanderT<MinimalExpander, implementation::MinimalExpander>
    {
    };
}
