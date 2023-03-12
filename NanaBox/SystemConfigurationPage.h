#pragma once

#include "SystemConfigurationPage.g.h"

namespace winrt
{
    using Windows::Foundation::PropertyValue;
    using Windows::Foundation::Collections::IVector;
    using Windows::UI::Xaml::Navigation::NavigationEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage>
    {
        SystemConfigurationPage();

        void OnNavigatedTo(NavigationEventArgs const& args);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
        IVector<IInspectable> AllowedProcessorCount();

        hstring UInt32ToString(uint32_t value);

        IInspectable BoxedProcessorCount();
        void BoxedProcessorCount(IInspectable const& value);
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
        IVector<IInspectable> m_allowedProcessorCount{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage, implementation::SystemConfigurationPage>
    {
    };
}
