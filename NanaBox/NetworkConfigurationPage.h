#pragma once

#include "NetworkConfigurationPage.g.h"

namespace winrt
{
    using Windows::UI::Xaml::Navigation::NavigationEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage>
    {
        NetworkConfigurationPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage, implementation::NetworkConfigurationPage>
    {
    };
}
