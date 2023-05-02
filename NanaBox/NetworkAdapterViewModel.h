#pragma once

#include "VirtualMachineConfiguration.h"
#include "NetworkAdapterViewModel.g.h"

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

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    private:
        ::NanaBox::NetworkAdapterConfiguration* m_netAdapterConfig;
        hstring m_macAddress;
        hstring m_endpointId;
    };
}
