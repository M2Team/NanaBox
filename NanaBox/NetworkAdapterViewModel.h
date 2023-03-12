#pragma once

#include "VirtualMachineConfiguration.h"
#include "NetworkAdapterViewModel.g.h"


namespace winrt
{
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
}

namespace winrt::NanaBox::implementation
{
    struct NetworkAdapterViewModel : NetworkAdapterViewModelT<NetworkAdapterViewModel>
    {
        NetworkAdapterViewModel(::NanaBox::NetworkAdapterConfiguration* netAdapterConfig);

        bool Enabled();
        void Enabled(bool value);
        bool Connected();
        void Connected(bool value);
        hstring MacAddress();
        void MacAddress(hstring const& value);
        hstring EndpointId();
        void EndpointId(hstring const& value);

        winrt::event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        ::NanaBox::NetworkAdapterConfiguration* m_netAdapterConfig;
        hstring m_macAddress;
        hstring m_endpointId;
        winrt::event<PropertyChangedEventHandler> m_propertyChanged;
    };
}
