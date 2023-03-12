#include "pch.h"
#include "ConfigurationViewModel.h"
#if __has_include("ConfigurationViewModel.g.cpp")
#include "ConfigurationViewModel.g.cpp"
#endif

#include "NetworkAdapterViewModel.h"
#include "ScsiDeviceViewModel.h"

namespace winrt::NanaBox::implementation
{
    ConfigurationViewModel::ConfigurationViewModel(::NanaBox::VirtualMachineConfiguration* config)
        : m_configuration{ config }
    {        
        m_name = winrt::to_hstring(m_configuration->Name);
        m_guestStateFile = winrt::to_hstring(m_configuration->GuestStateFile);
        m_runtimeStateFile = winrt::to_hstring(m_configuration->RuntimeStateFile);
        m_saveStateFile = winrt::to_hstring(m_configuration->SaveStateFile);

        m_netAdapters = winrt::single_threaded_observable_vector<NanaBox::NetworkAdapterViewModel>();
        for (auto&& device : m_configuration->NetworkAdapters) {
            auto impl = winrt::make_self<implementation::NetworkAdapterViewModel>(&device);
            m_netAdapters.Append(*impl);
        }

        m_scsiDevices = winrt::single_threaded_observable_vector<NanaBox::ScsiDeviceViewModel>();
        for (auto&& device : m_configuration->ScsiDevices) {
            auto impl = winrt::make_self<implementation::ScsiDeviceViewModel>(&device);
            m_scsiDevices.Append(*impl);
        }
    }

    int32_t ConfigurationViewModel::Version()
    {
        return m_configuration->Version;
    }

    hstring ConfigurationViewModel::Name()
    {
        return m_name;
    }
    void ConfigurationViewModel::Name(hstring const& value)
    {
        if (m_name != value)
        {
            m_name = value;
            m_configuration->Name = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"Name" });
        }
    }

    winrt::NanaBox::GuestType ConfigurationViewModel::GuestType()
    {
        return static_cast<winrt::NanaBox::GuestType>(m_configuration->GuestType);
    }
    void ConfigurationViewModel::GuestType(winrt::NanaBox::GuestType const& value)
    {
        auto newValue = static_cast<::NanaBox::GuestType>(value);
        if (m_configuration->GuestType != newValue)
        {
            m_configuration->GuestType = newValue;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"GuestType" });
        }
    }

    uint32_t ConfigurationViewModel::ProcessorCount()
    {
        return m_configuration->ProcessorCount;
    }
    void ConfigurationViewModel::ProcessorCount(uint32_t value)
    {
        if (m_configuration->ProcessorCount != value)
        {
            m_configuration->ProcessorCount = value;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"ProcessorCount" });
        }
    }

    uint64_t ConfigurationViewModel::MemorySize()
    {
        return m_configuration->MemorySize;
    }
    void ConfigurationViewModel::MemorySize(uint64_t value)
    {
        if (m_configuration->MemorySize != value)
        {
            m_configuration->MemorySize = value;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"MemorySize" });
        }
    }

    bool ConfigurationViewModel::ExposeVirtualizationExtensions()
    {
        return m_configuration->ExposeVirtualizationExtensions;
    }
    void ConfigurationViewModel::ExposeVirtualizationExtensions(bool value)
    {
        if (m_configuration->ExposeVirtualizationExtensions != value)
        {
            m_configuration->ExposeVirtualizationExtensions = value;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"ExposeVirtualizationExtensions" });
        }
    }

    hstring ConfigurationViewModel::GuestStateFile()
    {
        return m_guestStateFile;
    }
    void ConfigurationViewModel::GuestStateFile(hstring const& value)
    {
        if (m_guestStateFile != value)
        {
            m_guestStateFile = value;
            m_configuration->GuestStateFile = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"GuestStateFile" });
        }
    }

    hstring ConfigurationViewModel::RuntimeStateFile()
    {
        return m_runtimeStateFile;
    }
    void ConfigurationViewModel::RuntimeStateFile(hstring const& value)
    {
        if (m_runtimeStateFile != value)
        {
            m_runtimeStateFile = value;
            m_configuration->RuntimeStateFile = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"RuntimeStateFile" });
        }
    }

    hstring ConfigurationViewModel::SaveStateFile()
    {
        return m_saveStateFile;
    }
    void ConfigurationViewModel::SaveStateFile(hstring const& value)
    {
        if (m_saveStateFile != value)
        {
            m_saveStateFile = value;
            m_configuration->SaveStateFile = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"SaveStateFile" });
        }
    }

    winrt::NanaBox::ComPortsViewModel ConfigurationViewModel::ComPorts()
    {
        throw hresult_not_implemented();
    }
    winrt::NanaBox::GpuViewModel ConfigurationViewModel::Gpu()
    {
        throw hresult_not_implemented();
    }
    winrt::IObservableVector<winrt::NanaBox::NetworkAdapterViewModel> ConfigurationViewModel::NetworkAdapters()
    {
        return m_netAdapters;
    }
    winrt::IObservableVector<winrt::NanaBox::ScsiDeviceViewModel> ConfigurationViewModel::ScsiDevices()
    {
        return m_scsiDevices;
    }

    winrt::ICommand ConfigurationViewModel::SaveCommand()
    {
        throw hresult_not_implemented();
    }

    winrt::event_token ConfigurationViewModel::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }
    void ConfigurationViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
