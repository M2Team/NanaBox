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
        SystemConfigurationPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);
        winrt::NanaBox::ConfigurationViewModel ViewModel();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage, implementation::SystemConfigurationPage>
    {
    };
}
