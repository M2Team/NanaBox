#include "pch.h"
#include "NetworkAdapterViewModel.h"
#if __has_include("NetworkAdapterViewModel.g.cpp")
#include "NetworkAdapterViewModel.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    NetworkAdapterViewModel::NetworkAdapterViewModel(::NanaBox::NetworkAdapterConfiguration* netAdapterConfig) :
        m_netAdapterConfig{ netAdapterConfig }
    {
        m_macAddress = winrt::to_hstring(m_netAdapterConfig->MacAddress);
        m_endpointId = winrt::to_hstring(m_netAdapterConfig->EndpointId);
    }

    bool NetworkAdapterViewModel::Enabled()
    {
        return m_netAdapterConfig->Enabled;
    }
    void NetworkAdapterViewModel::Enabled(bool value)
    {
        if (m_netAdapterConfig->Enabled != value)
        {
            m_netAdapterConfig->Enabled = value;
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"Enabled" });
        }
    }

    bool NetworkAdapterViewModel::Connected()
    {
        return m_netAdapterConfig->Connected;
    }
    void NetworkAdapterViewModel::Connected(bool value)
    {
        if (m_netAdapterConfig->Connected != value)
        {
            m_netAdapterConfig->Connected = value;
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"Connected" });
        }
    }

    hstring NetworkAdapterViewModel::MacAddress()
    {
        return m_macAddress;
    }
    void NetworkAdapterViewModel::MacAddress(hstring const& value)
    {
        if (m_macAddress != value)
        {
            m_macAddress = value;
            m_netAdapterConfig->MacAddress = winrt::to_string(value);
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"MacAddress" });
        }
    }

    hstring NetworkAdapterViewModel::EndpointId()
    {
        return m_endpointId;
    }
    void NetworkAdapterViewModel::EndpointId(hstring const& value)
    {
        if (m_endpointId != value)
        {
            m_endpointId = value;
            m_netAdapterConfig->EndpointId = winrt::to_string(value);
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"EndpointId" });
        }
    }
}
