#include "pch.h"
#include "ConfigurationViewModel.h"
#if __has_include("ConfigurationViewModel.g.cpp")
#include "ConfigurationViewModel.g.cpp"
#endif

#include "ComPortsViewModel.h"
#include "NetworkAdapterViewModel.h"
#include "ScsiDeviceViewModel.h"
#include "SelectOption.h"
#include "RelayCommand.h"

#include <functional>
namespace winrt::NanaBox::implementation
{
    ConfigurationViewModel::ConfigurationViewModel(::NanaBox::VirtualMachineConfiguration* config)
        : m_configuration{ config }
    {
        m_name = winrt::to_hstring(m_configuration->Name);
        m_guestStateFile = winrt::to_hstring(m_configuration->GuestStateFile);
        m_runtimeStateFile = winrt::to_hstring(m_configuration->RuntimeStateFile);
        m_saveStateFile = winrt::to_hstring(m_configuration->SaveStateFile);

        m_guestTypeList = winrt::single_threaded_observable_vector<NanaBox::SelectOption>({
            winrt::make<implementation::SelectOption>(L"Windows",
                static_cast<uint32_t>(::NanaBox::GuestType::Windows)),
            winrt::make<implementation::SelectOption>(L"Linux",
                static_cast<uint32_t>(::NanaBox::GuestType::Linux))
        });

        m_processorCountList = winrt::single_threaded_observable_vector<NanaBox::SelectOption>();
        {
            uint32_t count = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
            if (count <= 0) count = 1;

            for (uint32_t i = 1; i <= count; i++)
            {
                auto item = winrt::make<implementation::SelectOption>(winrt::to_hstring(i), i);
                m_processorCountList.Append(item);
            }
        }

        m_guestType = nullptr;
        {            
            for (auto&& item : m_guestTypeList)
            {
                if (static_cast<::NanaBox::GuestType>(item.Value()) == m_configuration->GuestType)
                {
                    m_guestType = item;
                    break;
                }
            }
        }
        
        m_processorCount = nullptr;
        {
            for (auto&& item : m_processorCountList)
            {
                if (item.Value() == m_configuration->ProcessorCount)
                {
                    m_processorCount = item;
                    break;
                }
            }
        }

        m_comPorts = winrt::make<implementation::ComPortsViewModel>(&config->ComPorts);

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

        m_deleteNetAdapterCmd = winrt::make<implementation::RelayCommand>(
            std::bind(&ConfigurationViewModel::DeleteNetworkAdapter, this, std::placeholders::_1)
        );
        m_deleteScsiDeviceCmd = winrt::make<implementation::RelayCommand>(
            std::bind(&ConfigurationViewModel::DeleteScsiDevice, this, std::placeholders::_1)            
        );
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

    NanaBox::SelectOption ConfigurationViewModel::GuestType()
    {
        return m_guestType;
    }
    void ConfigurationViewModel::GuestType(NanaBox::SelectOption const& value)
    {
        if (m_guestType != value)
        {            
            m_guestType = value;
            m_configuration->GuestType = static_cast<::NanaBox::GuestType>(value.Value());
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"GuestType" });
        }
    }

    NanaBox::SelectOption ConfigurationViewModel::ProcessorCount()
    {
        return m_processorCount;
    }
    void ConfigurationViewModel::ProcessorCount(NanaBox::SelectOption value)
    {
        if (m_processorCount != value)
        {            
            m_processorCount = value;
            m_configuration->ProcessorCount = value.Value();
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
        return m_comPorts;
    }
    winrt::NanaBox::GpuViewModel ConfigurationViewModel::Gpu()
    {
        return m_gpu;
    }
    winrt::IObservableVector<winrt::NanaBox::NetworkAdapterViewModel> ConfigurationViewModel::NetworkAdapters()
    {
        return m_netAdapters;
    }
    winrt::IObservableVector<winrt::NanaBox::ScsiDeviceViewModel> ConfigurationViewModel::ScsiDevices()
    {
        return m_scsiDevices;
    }

    IObservableVector<NanaBox::SelectOption> ConfigurationViewModel::GuestTypeList()
    {
        return m_guestTypeList;
    }
    IObservableVector<NanaBox::SelectOption> ConfigurationViewModel::ProcessorCountList()
    {
        return m_processorCountList;
    }

    void ConfigurationViewModel::AddNetworkAdapter()
    {
        m_configuration->NetworkAdapters.push_back(::NanaBox::NetworkAdapterConfiguration{});
        auto&& device = m_configuration->NetworkAdapters.back();
        device.Connected = true;
        device.Enabled = true;

        auto impl = winrt::make_self<implementation::NetworkAdapterViewModel>(&device);
        m_netAdapters.Append(*impl);
    }
    void ConfigurationViewModel::DeleteNetworkAdapter(IInspectable item)
    {
        auto vm = item.try_as<NanaBox::NetworkAdapterViewModel>();
        if (!vm) return;

        uint32_t index;
        if (!m_netAdapters.IndexOf(vm, index)) return;

        m_netAdapters.RemoveAt(index);
        auto iter = m_configuration->NetworkAdapters.begin();
        std::advance(iter, index);
        m_configuration->NetworkAdapters.erase(iter);
    }
    winrt::ICommand ConfigurationViewModel::DeleteNetworkAdapterCommand()
    {
        return m_deleteNetAdapterCmd;
    }

    void ConfigurationViewModel::AddVirtualDisk()
    {
        m_configuration->ScsiDevices.push_back(::NanaBox::ScsiDeviceConfiguration{});
        auto&& device = m_configuration->ScsiDevices.back();
        device.Type = ::NanaBox::ScsiDeviceType::VirtualDisk;
        device.Enabled = true;

        auto impl = winrt::make_self<implementation::ScsiDeviceViewModel>(&device);
        m_scsiDevices.Append(*impl);
    }
    void ConfigurationViewModel::AddVirtualImage()
    {
        m_configuration->ScsiDevices.push_back(::NanaBox::ScsiDeviceConfiguration{});
        auto&& device = m_configuration->ScsiDevices.back();
        device.Type = ::NanaBox::ScsiDeviceType::VirtualImage;
        device.Enabled = true;

        auto impl = winrt::make_self<implementation::ScsiDeviceViewModel>(&device);
        m_scsiDevices.Append(*impl);
    }
    void ConfigurationViewModel::DeleteScsiDevice(IInspectable item)
    {
        auto vm = item.try_as<NanaBox::ScsiDeviceViewModel>();
        if (!vm) return;

        uint32_t index;
        if (!m_scsiDevices.IndexOf(vm, index)) return;

        m_scsiDevices.RemoveAt(index);
        auto iter = m_configuration->ScsiDevices.begin();
        std::advance(iter, index);
        m_configuration->ScsiDevices.erase(iter);
    }
    winrt::ICommand ConfigurationViewModel::DeleteScsiDeviceCommand()
    {
        return m_deleteScsiDeviceCmd;
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
