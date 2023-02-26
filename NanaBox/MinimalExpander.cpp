#include "pch.h"
#include "MinimalExpander.h"
#if __has_include("MinimalExpander.g.cpp")
#include "MinimalExpander.g.cpp"
#endif

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::DependencyObject;
    using Windows::UI::Xaml::DependencyProperty;
    using Windows::UI::Xaml::DependencyPropertyChangedEventArgs;
    using Windows::UI::Xaml::PropertyMetadata;
    using Windows::UI::Xaml::PropertyChangedCallback;
    using Windows::UI::Xaml::DataTemplate;
    using Windows::UI::Xaml::VisualStateManager;
    using Windows::UI::Xaml::SizeChangedEventArgs;
    using Windows::UI::Xaml::Controls::Border;
    using Windows::UI::Xaml::Hosting::ElementCompositionPreview;
}

namespace winrt::NanaBox::implementation
{
    winrt::DependencyProperty MinimalExpander::_headerProperty{ nullptr };
    winrt::DependencyProperty MinimalExpander::_headerTemplateProperty{ nullptr };
    winrt::DependencyProperty MinimalExpander::_isExpandedProperty{ nullptr };
    winrt::DependencyProperty MinimalExpander::_negativeContentHeightProperty{ nullptr };

    MinimalExpander::MinimalExpander()
    {
        if (!_headerProperty)
        {
            _headerProperty = winrt::DependencyProperty::Register(
                L"Header",
                winrt::xaml_typename<winrt::IInspectable>(),
                winrt::xaml_typename<NanaBox::MinimalExpander>(),
                winrt::PropertyMetadata{ winrt::IInspectable{nullptr} }
            );
        }

        if (!_headerTemplateProperty)
        {
            _headerTemplateProperty = winrt::DependencyProperty::Register(
                L"HeaderTemplate",
                winrt::xaml_typename<winrt::DataTemplate>(),
                winrt::xaml_typename<NanaBox::MinimalExpander>(),
                winrt::PropertyMetadata{ winrt::DataTemplate{nullptr} }
            );
        }

        if (!_isExpandedProperty)
        {
            _isExpandedProperty = winrt::DependencyProperty::Register(
                L"IsExpanded",
                winrt::xaml_typename<bool>(),
                winrt::xaml_typename<NanaBox::MinimalExpander>(),
                winrt::PropertyMetadata{
                    winrt::box_value<bool>(false),
                    winrt::PropertyChangedCallback(&OnIsExpandedPropertyChanged)
                });
        }

        if (!_negativeContentHeightProperty)
        {
            _negativeContentHeightProperty = winrt::DependencyProperty::Register(
                L"NegativeContentHeight",
                winrt::xaml_typename<double>(),
                winrt::xaml_typename<NanaBox::MinimalExpander>(),
                winrt::PropertyMetadata{ winrt::box_value<double>(0.0) });
        }
    }

    IInspectable MinimalExpander::Header()
    {
        return GetValue(_headerProperty);
    }

    void MinimalExpander::Header(winrt::IInspectable value)
    {
        SetValue(_headerProperty, value);
    }

    DataTemplate MinimalExpander::HeaderTemplate()
    {
        return GetValue(_headerTemplateProperty).as<winrt::DataTemplate>();
    }

    void MinimalExpander::HeaderTemplate(winrt::DataTemplate value)
    {
        SetValue(_headerTemplateProperty, value);
    }

    bool MinimalExpander::IsExpanded()
    {
        return winrt::unbox_value<bool>(GetValue(_isExpandedProperty));
    }

    void MinimalExpander::IsExpanded(bool value)
    {
        UNREFERENCED_PARAMETER(value);

        SetValue(_isExpandedProperty, winrt::box_value(value));
    }

    void MinimalExpander::OnIsExpandedPropertyChanged(
        winrt::DependencyObject const& sender,
        winrt::DependencyPropertyChangedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(args);
        auto owner = sender.as<winrt::NanaBox::MinimalExpander>();
        auto self = winrt::get_self<MinimalExpander>(owner);
        self->UpdateExpandState(true);
    }

    double MinimalExpander::NegativeContentHeight()
    {
        return winrt::unbox_value<double>(GetValue(_negativeContentHeightProperty));
    }

    void MinimalExpander::NegativeContentHeight(double value)
    {
        SetValue(_negativeContentHeightProperty, winrt::box_value(value));
    }

    void MinimalExpander::OnApplyTemplate()
    {
        if (auto expanderContentClip = GetTemplateChild(L"ExpanderContentClip").try_as<winrt::Border>())
        {
            auto visual = winrt::ElementCompositionPreview::GetElementVisual(expanderContentClip);
            visual.Clip(visual.Compositor().CreateInsetClip());
        }

        if (auto expanderContent = GetTemplateChild(L"ExpanderContent").try_as<winrt::Border>())
        {
            m_contentSizeChangedRevoker = expanderContent.SizeChanged(
                winrt::auto_revoke, { this, &MinimalExpander::OnContentSizeChanged });
        }

        UpdateExpandState(false);
    }


    void MinimalExpander::UpdateExpandState(bool useTransitions)
    {
        const auto isExpanded = IsExpanded();

        if (isExpanded)
        {
            winrt::VisualStateManager::GoToState(*this, L"ExpandDown", useTransitions);
        }
        else
        {
            winrt::VisualStateManager::GoToState(*this, L"CollapseUp", useTransitions);
        }
    }

    void MinimalExpander::OnContentSizeChanged(
        winrt::IInspectable const& sender,
        winrt::SizeChangedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);

        auto height = args.NewSize().Height;
        NegativeContentHeight(-1.0 * height);
    }

}
