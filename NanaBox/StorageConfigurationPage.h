#pragma once

#include "StorageConfigurationPage.g.h"

namespace winrt
{
    using Windows::UI::Xaml::Navigation::NavigationEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage>
    {
        StorageConfigurationPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage, implementation::StorageConfigurationPage>
    {
    };
}
