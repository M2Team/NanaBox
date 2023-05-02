#pragma once

#include "VirtualMachineConfiguration.h"
#include "ScsiDeviceViewModel.g.h"

#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}

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
        uint32_t DeviceType();
        void DeviceType(uint32_t const& value);
        hstring Path();
        void Path(hstring const& value);

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    private:
        ::NanaBox::ScsiDeviceConfiguration* m_scsiConfig;
        hstring m_path;
    };
}
