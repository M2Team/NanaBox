#include "pch.h"
#include "ScsiDeviceViewModel.h"
#if __has_include("ScsiDeviceViewModel.g.cpp")
#include "ScsiDeviceViewModel.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    ScsiDeviceViewModel::ScsiDeviceViewModel(::NanaBox::ScsiDeviceConfiguration* scsiConfig)
        : m_scsiConfig{ scsiConfig }
    {
        m_path = winrt::to_hstring(m_scsiConfig->Path);
    }

    bool ScsiDeviceViewModel::Enabled()
    {
        return m_scsiConfig->Enabled;
    }
    void ScsiDeviceViewModel::Enabled(bool value)
    {
        if (m_scsiConfig->Enabled != value)
        {
            m_scsiConfig->Enabled = value;
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"Enabled" });
        }
    }

    uint32_t ScsiDeviceViewModel::DeviceType()
    {
        return static_cast<uint32_t>(m_scsiConfig->Type);
    }
    void ScsiDeviceViewModel::DeviceType(uint32_t const& value)
    {
        auto newValue = static_cast<::NanaBox::ScsiDeviceType>(value);
        if (m_scsiConfig->Type != newValue)
        {
            m_scsiConfig->Type = newValue;
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"DeviceType" });
        }
    }

    hstring ScsiDeviceViewModel::Path()
    {
        return m_path;
    }
    void ScsiDeviceViewModel::Path(hstring const& value)
    {
        if (m_path != value)
        {
            m_path = value;
            m_scsiConfig->Path = winrt::to_string(value);
            this->PropertyChanged(*this, PropertyChangedEventArgs{ L"Path" });
        }
    }
}
