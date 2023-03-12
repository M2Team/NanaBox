#pragma once

#include "pch.h"
#include "VirtualMachineConfiguration.h"

#include "ConfigurationViewModel.g.h"

namespace winrt
{
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
    using Windows::UI::Xaml::Input::ICommand;
    using Windows::Foundation::Collections::IObservableVector;
}

namespace winrt::NanaBox::implementation
{
    struct ConfigurationViewModel : ConfigurationViewModelT<ConfigurationViewModel>
    {
        ConfigurationViewModel(::NanaBox::VirtualMachineConfiguration* config);

        int32_t Version();
        hstring Name();
        void Name(hstring const& value);
        winrt::NanaBox::GuestType GuestType();
        void GuestType(winrt::NanaBox::GuestType const& value);
        uint32_t ProcessorCount();
        void ProcessorCount(uint32_t value);
        uint64_t MemorySize();
        void MemorySize(uint64_t value);
        bool ExposeVirtualizationExtensions();
        void ExposeVirtualizationExtensions(bool value);
        hstring GuestStateFile();
        void GuestStateFile(hstring const& value);
        hstring RuntimeStateFile();
        void RuntimeStateFile(hstring const& value);
        hstring SaveStateFile();
        void SaveStateFile(hstring const& value);
        winrt::NanaBox::ComPortsViewModel ComPorts();
        winrt::NanaBox::GpuViewModel Gpu();
        IObservableVector<winrt::NanaBox::NetworkAdapterViewModel> NetworkAdapters();
        IObservableVector<winrt::NanaBox::ScsiDeviceViewModel> ScsiDevices();

        ICommand SaveCommand();

        winrt::event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        ::NanaBox::VirtualMachineConfiguration* m_configuration;
        hstring m_name;
        hstring m_guestStateFile;
        hstring m_runtimeStateFile;
        hstring m_saveStateFile;
        IObservableVector<winrt::NanaBox::NetworkAdapterViewModel> m_netAdapters{ nullptr };
        IObservableVector<winrt::NanaBox::ScsiDeviceViewModel> m_scsiDevices{ nullptr };
        winrt::event<PropertyChangedEventHandler> m_propertyChanged;
    };
}

