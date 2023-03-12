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
        BasicConfigurationPage();

        void OnNavigatedTo(NavigationEventArgs const& args);

        winrt::NanaBox::ConfigurationViewModel ViewModel();
        IInspectable BoxedGuestType();
        void BoxedGuestType(IInspectable const& value);
        IVector<IInspectable> GuestTypes();
    private:
        winrt::NanaBox::ConfigurationViewModel m_viewModel{ nullptr };
        IVector<IInspectable> m_guestTypes{ nullptr };
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct BasicConfigurationPage : BasicConfigurationPageT<BasicConfigurationPage, implementation::BasicConfigurationPage>
    {
    };
}
