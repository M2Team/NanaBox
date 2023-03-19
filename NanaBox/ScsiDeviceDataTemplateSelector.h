#pragma once

#include "ScsiDeviceDataTemplateSelector.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::DataTemplate;
    using Windows::UI::Xaml::DependencyObject;
}

namespace winrt::NanaBox::implementation
{
    struct ScsiDeviceDataTemplateSelector : ScsiDeviceDataTemplateSelectorT<ScsiDeviceDataTemplateSelector>
    {
        ScsiDeviceDataTemplateSelector() = default;

        DataTemplate VirtualDiskTemplate();
        void VirtualDiskTemplate(DataTemplate const& value);
        DataTemplate VirtualImageTemplate();
        void VirtualImageTemplate(DataTemplate const& value);

        DataTemplate SelectTemplateCore(IInspectable obj);
        DataTemplate SelectTemplateCore(IInspectable obj, DependencyObject container);
    private:
        DataTemplate m_virtualDiskTemplate{ nullptr };
        DataTemplate m_virtualImageTemplate{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ScsiDeviceDataTemplateSelector : ScsiDeviceDataTemplateSelectorT<ScsiDeviceDataTemplateSelector, implementation::ScsiDeviceDataTemplateSelector>
    {
    };
}
