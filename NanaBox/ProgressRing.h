#pragma once

#include "ProgressRing.g.h"
#include <winrt/Windows.UI.Composition.h>

namespace winrt::NanaBox::implementation
{
    struct ProgressRing : ProgressRingT<ProgressRing>
    {
        ProgressRing();

        static Windows::UI::Xaml::DependencyProperty ForegroundProperty();
        Windows::UI::Xaml::Media::SolidColorBrush Foreground();
        void Foreground(Windows::UI::Xaml::Media::SolidColorBrush const& value);

        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize);
        Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size const& finalSize);

    private:
        static void OnForegroundChanged(Windows::UI::Xaml::DependencyObject d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs args);

        Windows::UI::Composition::Compositor m_compositor{ nullptr };
        Windows::UI::Composition::CompositionSpriteShape m_shape{ nullptr };
        Windows::UI::Composition::ShapeVisual m_visual{ nullptr };

        static Windows::UI::Xaml::DependencyProperty m_foregroundProperty;
        const float m_defaultProgressRingSize = 80.0f;
        const float m_strokeThickness = 1.5f;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ProgressRing : ProgressRingT<ProgressRing, implementation::ProgressRing>
    {
    };
}
