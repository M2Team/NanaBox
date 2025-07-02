#pragma once

#include "ProgressRing.g.h"
#include <winrt/Windows.UI.Composition.h>

namespace winrt
{
    using Windows::Foundation::Size;
    using Windows::UI::Composition::Compositor;
    using Windows::UI::Composition::CompositionSpriteShape;
    using Windows::UI::Composition::ShapeVisual;
    using Windows::UI::Xaml::DependencyObject;
    using Windows::UI::Xaml::DependencyProperty;
    using Windows::UI::Xaml::DependencyPropertyChangedEventArgs;
    using Windows::UI::Xaml::Media::SolidColorBrush;
}

namespace winrt::NanaBox::implementation
{
    struct ProgressRing : ProgressRingT<ProgressRing>
    {
        ProgressRing();

        static winrt::DependencyProperty ForegroundProperty();

        winrt::SolidColorBrush Foreground();

        void Foreground(
            winrt::SolidColorBrush const& value);

        winrt::Size MeasureOverride(
            winrt::Size const& availableSize);

        winrt::Size ArrangeOverride(
            winrt::Size const& finalSize);

    private:

        static winrt::DependencyProperty m_ForegroundProperty;

        static void OnForegroundChanged(
            winrt::DependencyObject sender,
            winrt::DependencyPropertyChangedEventArgs args);

        winrt::Compositor m_Compositor = nullptr;
        winrt::CompositionSpriteShape m_Shape = nullptr;
        winrt::ShapeVisual m_Visual = nullptr;

        const float m_DefaultProgressRingSize = 80.0f;
        const float m_StrokeThickness = 1.5f;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ProgressRing :
        ProgressRingT<ProgressRing, implementation::ProgressRing>
    {
    };
}
