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
}

namespace winrt::NanaBox::implementation
{
    DependencyProperty _headerProperty{ nullptr };
    DependencyProperty _headerTemplateProperty{ nullptr };
    DependencyProperty _isExpandedProperty{ nullptr };

    MinimalExpander::MinimalExpander()
    {
        if (!_headerProperty)
        {
            _headerProperty = DependencyProperty::Register(
                L"Header",
                xaml_typename<IInspectable>(),
                xaml_typename<NanaBox::MinimalExpander>(),
                PropertyMetadata{ IInspectable{} }
            );
        }

        if (!_headerTemplateProperty)
        {
            _headerTemplateProperty = DependencyProperty::Register(
                L"HeaderTemplate",
                xaml_typename<DataTemplate>(),
                xaml_typename<NanaBox::MinimalExpander>(),
                PropertyMetadata{ DataTemplate{} }
            );
        }

        if (!_isExpandedProperty)
        {
            _isExpandedProperty = DependencyProperty::Register(
                L"IsExpanded",
                xaml_typename<bool>(),
                xaml_typename<NanaBox::MinimalExpander>(),
                PropertyMetadata{
                    winrt::box_value<bool>(false),
                    PropertyChangedCallback(&OnIsExpandedPropertyChanged)
                });
        }
    }

    IInspectable MinimalExpander::Header()
    {
        return GetValue(_headerProperty);
    }

    void MinimalExpander::Header(IInspectable value)
    {
        SetValue(_headerProperty, value);
    }

    DependencyProperty MinimalExpander::HeaderProperty()
    {
        return _headerProperty;
    }

    DataTemplate MinimalExpander::HeaderTemplate()
    {
        return GetValue(_headerTemplateProperty).as<DataTemplate>();
    }

    void MinimalExpander::HeaderTemplate(DataTemplate value)
    {
        SetValue(_headerTemplateProperty, value);
    }

    DependencyProperty MinimalExpander::HeaderTemplateProperty()
    {
        return _headerTemplateProperty;
    }

    bool MinimalExpander::IsExpanded()
    {
        return winrt::unbox_value<bool>(GetValue(_isExpandedProperty));
    }

    void MinimalExpander::IsExpanded(bool value)
    {
        SetValue(_isExpandedProperty, winrt::box_value(value));
    }

    DependencyProperty MinimalExpander::IsExpandedProperty()
    {
        return _isExpandedProperty;
    }

    void MinimalExpander::OnIsExpandedPropertyChanged(
        DependencyObject const& sender,
        DependencyPropertyChangedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(args);
        auto owner = sender.as<winrt::NanaBox::MinimalExpander>();
        auto self = winrt::get_self<MinimalExpander>(owner);
        self->UpdateExpandState(true);
    }

    void MinimalExpander::OnApplyTemplate()
    {
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

}
