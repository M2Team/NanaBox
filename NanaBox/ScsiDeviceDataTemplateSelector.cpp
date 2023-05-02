#include "pch.h"
#include "ScsiDeviceDataTemplateSelector.h"
#if __has_include("ScsiDeviceDataTemplateSelector.g.cpp")
#include "ScsiDeviceDataTemplateSelector.g.cpp"
#endif

#include "VirtualMachineConfiguration.h"

namespace winrt::NanaBox::implementation
{
    DataTemplate ScsiDeviceDataTemplateSelector::VirtualDiskTemplate()
    {
        return m_virtualDiskTemplate;
    }
    void ScsiDeviceDataTemplateSelector::VirtualDiskTemplate(DataTemplate const& value)
    {
        m_virtualDiskTemplate = value;
    }

    DataTemplate ScsiDeviceDataTemplateSelector::VirtualImageTemplate()
    {
        return m_virtualImageTemplate;
    }
    void ScsiDeviceDataTemplateSelector::VirtualImageTemplate(DataTemplate const& value)
    {
        m_virtualImageTemplate = value;
    }

    DataTemplate ScsiDeviceDataTemplateSelector::SelectTemplateCore(IInspectable obj)
    {
        auto vm = obj.as<ScsiDeviceViewModel>();
        auto deviceType = static_cast<::NanaBox::ScsiDeviceType>(vm.DeviceType());

        switch (deviceType)
        {
        case ::NanaBox::ScsiDeviceType::VirtualDisk:
            return m_virtualDiskTemplate;
        case ::NanaBox::ScsiDeviceType::VirtualImage:
            return m_virtualImageTemplate;
        case ::NanaBox::ScsiDeviceType::PhysicalDevice:
        default:
            throw hresult_not_implemented(); // TODO
        }
    }

    DataTemplate ScsiDeviceDataTemplateSelector::SelectTemplateCore(IInspectable obj, DependencyObject container)
    {
        auto vm = obj.as<ScsiDeviceViewModel>();
        auto deviceType = static_cast<::NanaBox::ScsiDeviceType>(vm.DeviceType());

        switch (deviceType)
        {
        case ::NanaBox::ScsiDeviceType::VirtualDisk:
            return m_virtualDiskTemplate;
        case ::NanaBox::ScsiDeviceType::VirtualImage:
            return m_virtualImageTemplate;
        case ::NanaBox::ScsiDeviceType::PhysicalDevice:
        default:
            throw hresult_not_implemented(); // TODO
        }
    }
}
