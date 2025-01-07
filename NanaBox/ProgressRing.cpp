#include "pch.h"
#include "ProgressRing.h"
#if __has_include("ProgressRing.g.cpp")
#include "ProgressRing.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    ProgressRing::ProgressRing()
    {
        m_compositor = Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(*this).Compositor();

        auto ellipse = m_compositor.CreateEllipseGeometry();
        ellipse.Radius({ 7, 7 });

        m_shape = m_compositor.CreateSpriteShape(ellipse);
        m_shape.StrokeDashCap(Windows::UI::Composition::CompositionStrokeCap::Round);
        m_shape.StrokeStartCap(Windows::UI::Composition::CompositionStrokeCap::Round);
        m_shape.StrokeEndCap(Windows::UI::Composition::CompositionStrokeCap::Round);
        m_shape.StrokeThickness(m_strokeThickness);
        m_shape.TransformMatrix({ 5.0f, 0.0f, 0.0f, 5.0f, 40.0f, 40.0f });

        m_visual = m_compositor.CreateShapeVisual();
        m_visual.Size({ m_defaultProgressRingSize, m_defaultProgressRingSize });
        m_visual.Shapes().Append(m_shape);

        Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(*this, m_visual);

        auto holdThenStepEasing = m_compositor.CreateStepEasingFunction();
        holdThenStepEasing.IsFinalStepSingleFrame(true);

        auto cubicBezierEasing = m_compositor.CreateCubicBezierEasingFunction({ 0.166999996f, 0.166999996f }, { 0.833000004f, 0.833000004f });

        auto rotationAnimation = m_compositor.CreateScalarKeyFrameAnimation();
        rotationAnimation.InsertKeyFrame(0.0f, 0.0f, holdThenStepEasing);
        rotationAnimation.InsertKeyFrame(0.5f, 450.0f, cubicBezierEasing);
        rotationAnimation.InsertKeyFrame(1.0f, 1080.0f, cubicBezierEasing);
        rotationAnimation.Duration(Windows::Foundation::TimeSpan{ std::chrono::seconds(2) });
        rotationAnimation.IterationBehavior(Windows::UI::Composition::AnimationIterationBehavior::Forever);

        auto sizeAnimation = m_compositor.CreateScalarKeyFrameAnimation();
        sizeAnimation.InsertKeyFrame(0.0f, 0.0f, holdThenStepEasing);
        sizeAnimation.InsertKeyFrame(0.5f, 0.5f, cubicBezierEasing);
        sizeAnimation.InsertKeyFrame(1.0f, 0.0f, cubicBezierEasing);
        sizeAnimation.Duration(Windows::Foundation::TimeSpan{ std::chrono::seconds(2) });
        sizeAnimation.IterationBehavior(Windows::UI::Composition::AnimationIterationBehavior::Forever);

        m_shape.StartAnimation(L"RotationAngleInDegrees", rotationAnimation);
        ellipse.StartAnimation(L"TrimEnd", sizeAnimation);
    }

    Windows::Foundation::Size ProgressRing::MeasureOverride(Windows::Foundation::Size const& availableSize)
    {
        auto size = availableSize.Width > availableSize.Height ? availableSize.Height : availableSize.Width;
        if (size == INFINITY)
        {
            size = m_defaultProgressRingSize;
        }

        return { size, size };
    }

    Windows::Foundation::Size ProgressRing::ArrangeOverride(Windows::Foundation::Size const& finalSize)
    {
        auto size = finalSize.Width > finalSize.Height ? finalSize.Height : finalSize.Width;

        float scale = size / m_defaultProgressRingSize;
        m_visual.Scale({ scale, scale, scale });

        float widthOffset = (finalSize.Width - size) / 2;
        float heightOffset = (finalSize.Height - size) / 2;
        m_visual.Offset({ widthOffset, heightOffset, 0.0f });

        return finalSize;
    }

    Windows::UI::Xaml::DependencyProperty ProgressRing::m_foregroundProperty =
        Windows::UI::Xaml::DependencyProperty::Register(
            L"Foreground",
            winrt::xaml_typename<Windows::UI::Xaml::Media::SolidColorBrush>(),
            winrt::xaml_typename<NanaBox::ProgressRing>(),
            Windows::UI::Xaml::PropertyMetadata{ nullptr, &OnForegroundChanged }
        );

    void ProgressRing::OnForegroundChanged(Windows::UI::Xaml::DependencyObject d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs args)
    {
        auto progressRing = d.as<NanaBox::ProgressRing>();
        auto progressRingImpl = winrt::get_self<NanaBox::implementation::ProgressRing>(progressRing);

        auto brush = progressRingImpl->m_shape.StrokeBrush();

        if (brush)
        {
            brush.as<Windows::UI::Composition::CompositionColorBrush>().Color(args.NewValue().as<Windows::UI::Xaml::Media::SolidColorBrush>().Color());
        }
    }

    Windows::UI::Xaml::DependencyProperty ProgressRing::ForegroundProperty()
    {
        return m_foregroundProperty;
    }

    Windows::UI::Xaml::Media::SolidColorBrush ProgressRing::Foreground()
    {
        return winrt::unbox_value<Windows::UI::Xaml::Media::SolidColorBrush>(GetValue(m_foregroundProperty));
    }

    void ProgressRing::Foreground(Windows::UI::Xaml::Media::SolidColorBrush const& value)
    {
        SetValue(m_foregroundProperty, value);
    }
}
