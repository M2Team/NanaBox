#pragma once

#include "VirtualMachineConfiguration.h"
#include "ScsiDeviceViewModel.g.h"

namespace winrt
{
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
}

namespace winrt::NanaBox::implementation
{
    struct ScsiDeviceViewModel : ScsiDeviceViewModelT<ScsiDeviceViewModel>
    {
        ScsiDeviceViewModel(::NanaBox::ScsiDeviceConfiguration* scsiConfig);

        bool Enabled();
        void Enabled(bool value);
        winrt::NanaBox::ScsiDeviceType DeviceType();
        void DeviceType(winrt::NanaBox::ScsiDeviceType const& value);
        hstring Path();
        void Path(hstring const& value);

        winrt::event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
    private:
        ::NanaBox::ScsiDeviceConfiguration* m_scsiConfig;
        hstring m_path;
        winrt::event<PropertyChangedEventHandler> m_propertyChanged;
    };
}
