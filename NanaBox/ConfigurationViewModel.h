#pragma once

#include "VirtualMachineConfiguration.h"
#include "ConfigurationViewModel.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Collections::IVector;
    using Windows::Foundation::Collections::IObservableVector;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
    using Windows::UI::Xaml::Input::ICommand;
}

namespace winrt::NanaBox::implementation
{
    struct ConfigurationViewModel : ConfigurationViewModelT<ConfigurationViewModel>
    {
        ConfigurationViewModel(::NanaBox::VirtualMachineConfiguration* config);

        int32_t Version();
        hstring Name();
        void Name(hstring const& value);
        NanaBox::SelectOption GuestType();
        void GuestType(NanaBox::SelectOption const& value);
        NanaBox::SelectOption ProcessorCount();
        void ProcessorCount(NanaBox::SelectOption value);
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
        NanaBox::ComPortsViewModel ComPorts();
        NanaBox::GpuViewModel Gpu();
        IObservableVector<NanaBox::NetworkAdapterViewModel> NetworkAdapters();
        IObservableVector<NanaBox::ScsiDeviceViewModel> ScsiDevices();

        IObservableVector<NanaBox::SelectOption> GuestTypeList();
        IObservableVector<NanaBox::SelectOption> ProcessorCountList();

        ICommand SaveCommand();

        winrt::event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        ::NanaBox::VirtualMachineConfiguration* m_configuration;

        hstring m_name;
        hstring m_guestStateFile;
        hstring m_runtimeStateFile;
        hstring m_saveStateFile;

        NanaBox::ComPortsViewModel m_comPorts{ nullptr };
        NanaBox::GpuViewModel m_gpu{ nullptr };
        IObservableVector<NanaBox::NetworkAdapterViewModel> m_netAdapters{ nullptr };
        IObservableVector<NanaBox::ScsiDeviceViewModel> m_scsiDevices{ nullptr };
        
        NanaBox::SelectOption m_guestType{ nullptr };
        NanaBox::SelectOption m_processorCount{ nullptr };
        IObservableVector<NanaBox::SelectOption> m_guestTypeList{ nullptr };
        IObservableVector<NanaBox::SelectOption> m_processorCountList{ nullptr };

        winrt::event<PropertyChangedEventHandler> m_propertyChanged;
    };
}

