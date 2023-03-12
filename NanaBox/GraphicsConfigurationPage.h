#pragma once

#include "GraphicsConfigurationPage.g.h"

namespace winrt
{
    using Windows::UI::Xaml::Navigation::NavigationEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct GraphicsConfigurationPage : GraphicsConfigurationPageT<GraphicsConfigurationPage>
    {
        GraphicsConfigurationPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct GraphicsConfigurationPage : GraphicsConfigurationPageT<GraphicsConfigurationPage, implementation::GraphicsConfigurationPage>
    {
    };
}
