#pragma once

#include "BasicConfigurationPage.g.h"

namespace winrt
{
    using Windows::Foundation::IReference;
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Collections::IVector;
    using Windows::UI::Xaml::Navigation::NavigationEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct BasicConfigurationPage : BasicConfigurationPageT<BasicConfigurationPage>
    {
        BasicConfigurationPage() = default;

        void OnNavigatedTo(NavigationEventArgs const& args);
        winrt::NanaBox::ConfigurationViewModel ViewModel();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct BasicConfigurationPage : BasicConfigurationPageT<BasicConfigurationPage, implementation::BasicConfigurationPage>
    {
    };
}
