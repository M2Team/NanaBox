#define NOMINMAX
#include "pch.h"
#include "ProgressRing.h"
#if __has_include("ProgressRing.g.cpp")
#include "ProgressRing.g.cpp"
#endif

namespace winrt
{
    using Windows::Foundation::Numerics::float2;
    using Windows::Foundation::Numerics::float3;
    using Windows::Foundation::Numerics::float3x2;
    using Windows::Foundation::TimeSpan;
    using Windows::UI::Color;
    using Windows::UI::Composition::AnimationIterationBehavior;
    using Windows::UI::Composition::CompositionColorBrush;
    using Windows::UI::Composition::CompositionEllipseGeometry;
    using Windows::UI::Composition::CompositionStrokeCap;
    using Windows::UI::Composition::CubicBezierEasingFunction;
    using Windows::UI::Composition::ScalarKeyFrameAnimation;
    using Windows::UI::Composition::StepEasingFunction;
    using Windows::UI::Xaml::Hosting::ElementCompositionPreview;
    using Windows::UI::Xaml::PropertyMetadata;
    using Windows::UI::Xaml::PropertyChangedCallback;
}

namespace winrt::NanaBox::implementation
{
    ProgressRing::ProgressRing()
    {
        this->m_Compositor =
            winrt::ElementCompositionPreview::GetElementVisual(
                *this).Compositor();

        winrt::CompositionEllipseGeometry Ellipse =
            this->m_Compositor.CreateEllipseGeometry();
        Ellipse.Radius({ 7, 7 });

        this->m_Shape = this->m_Compositor.CreateSpriteShape(Ellipse);
        this->m_Shape.StrokeDashCap(winrt::CompositionStrokeCap::Round);
        this->m_Shape.StrokeStartCap(winrt::CompositionStrokeCap::Round);
        this->m_Shape.StrokeEndCap(winrt::CompositionStrokeCap::Round);
        this->m_Shape.StrokeThickness(this->m_StrokeThickness);
        this->m_Shape.TransformMatrix(
            winrt::float3x2(5.0f, 0.0f, 0.0f, 5.0f, 40.0f, 40.0f));

        this->m_Visual = this->m_Compositor.CreateShapeVisual();
        this->m_Visual.Size(winrt::float2(
            this->m_DefaultProgressRingSize,
            this->m_DefaultProgressRingSize));
        this->m_Visual.Shapes().Append(this->m_Shape);

        winrt::ElementCompositionPreview::SetElementChildVisual(
            *this,
            this->m_Visual);

        winrt::StepEasingFunction HoldThenStepEasing =
            this->m_Compositor.CreateStepEasingFunction();
        HoldThenStepEasing.IsFinalStepSingleFrame(true);

        winrt::CubicBezierEasingFunction CubicBezierEasing =
            this->m_Compositor.CreateCubicBezierEasingFunction(
                winrt::float2(0.166999996f, 0.166999996f),
                winrt::float2(0.833000004f, 0.833000004f));

        winrt::ScalarKeyFrameAnimation RotationAnimation =
            this->m_Compositor.CreateScalarKeyFrameAnimation();
        RotationAnimation.InsertKeyFrame(0.0f, 0.0f, HoldThenStepEasing);
        RotationAnimation.InsertKeyFrame(0.5f, 450.0f, CubicBezierEasing);
        RotationAnimation.InsertKeyFrame(1.0f, 1080.0f, CubicBezierEasing);
        RotationAnimation.Duration(winrt::TimeSpan(std::chrono::seconds(2)));
        RotationAnimation.IterationBehavior(
            winrt::AnimationIterationBehavior::Forever);

        winrt::ScalarKeyFrameAnimation SizeAnimation =
            this->m_Compositor.CreateScalarKeyFrameAnimation();
        SizeAnimation.InsertKeyFrame(0.0f, 0.0f, HoldThenStepEasing);
        SizeAnimation.InsertKeyFrame(0.5f, 0.5f, CubicBezierEasing);
        SizeAnimation.InsertKeyFrame(1.0f, 0.0f, CubicBezierEasing);
        SizeAnimation.Duration(winrt::TimeSpan(std::chrono::seconds(2)));
        SizeAnimation.IterationBehavior(
            winrt::AnimationIterationBehavior::Forever);

        this->m_Shape.StartAnimation(
            L"RotationAngleInDegrees",
            RotationAnimation);
        Ellipse.StartAnimation(L"TrimEnd", SizeAnimation);
    }

    winrt::DependencyProperty ProgressRing::ForegroundProperty()
    {
        return ProgressRing::m_ForegroundProperty;
    }

    winrt::SolidColorBrush ProgressRing::Foreground()
    {
        return winrt::unbox_value<winrt::SolidColorBrush>(
            this->GetValue(ProgressRing::m_ForegroundProperty));
    }

    void ProgressRing::Foreground(
        winrt::SolidColorBrush const& value)
    {
        this->SetValue(ProgressRing::m_ForegroundProperty, value);
    }

    winrt::Size ProgressRing::MeasureOverride(
        winrt::Size const& availableSize)
    {
        float Size = std::min(availableSize.Width, availableSize.Height);
        if (Size == INFINITY)
        {
            Size = this->m_DefaultProgressRingSize;
        }
        return winrt::Size(Size, Size);
    }

    winrt::Size ProgressRing::ArrangeOverride(
        winrt::Size const& finalSize)
    {
        float Size = std::min(finalSize.Width, finalSize.Height);
        float Scale = Size / this->m_DefaultProgressRingSize;
        this->m_Visual.Scale(winrt::float3(Scale, Scale, Scale));
        this->m_Visual.Offset(winrt::float3(
            (finalSize.Width - Size) / 2,
            (finalSize.Height - Size) / 2,
            0.0f));
        return finalSize;
    }

    winrt::DependencyProperty ProgressRing::m_ForegroundProperty =
        winrt::DependencyProperty::Register(
            L"Foreground",
            winrt::xaml_typename<winrt::SolidColorBrush>(),
            winrt::xaml_typename<NanaBox::ProgressRing>(),
            winrt::PropertyMetadata(
                nullptr,
                winrt::PropertyChangedCallback(&OnForegroundChanged)));

    void ProgressRing::OnForegroundChanged(
        winrt::DependencyObject sender,
        winrt::DependencyPropertyChangedEventArgs args)
    {
        using InstanceType = NanaBox::ProgressRing;
        using ImplementationType = NanaBox::implementation::ProgressRing;

        InstanceType Instance =
            sender.as<InstanceType>();
        ImplementationType* Implementation =
            winrt::get_self<ImplementationType>(Instance);

        winrt::CompositionColorBrush Brush =
            Implementation->m_Shape.StrokeBrush(
            ).try_as<winrt::CompositionColorBrush>();
        if (!Brush)
        {
            Brush = Implementation->m_Compositor.CreateColorBrush();
            Implementation->m_Shape.StrokeBrush(Brush);
        }

        Brush.Color(args.NewValue().as<winrt::SolidColorBrush>().Color());
    }
}
