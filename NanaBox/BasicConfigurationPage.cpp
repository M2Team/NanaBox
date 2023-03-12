#include "pch.h"
#include "BasicConfigurationPage.h"
#if __has_include("BasicConfigurationPage.g.cpp")
#include "BasicConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    BasicConfigurationPage::BasicConfigurationPage()
    {
        m_guestTypes = winrt::single_threaded_vector<IInspectable>({
            winrt::box_value<GuestType>(GuestType::Windows),
            winrt::box_value<GuestType>(GuestType::Linux)
        });
    }

    void BasicConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel BasicConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }

    IVector<IInspectable> BasicConfigurationPage::GuestTypes()
    {
        return m_guestTypes;
    }

    IInspectable BasicConfigurationPage::BoxedGuestType()
    {
        GuestType value = m_viewModel.GuestType();
        for (auto&& item : m_guestTypes)
        {
            if (winrt::unbox_value<GuestType>(item) == value)
            {
                return item;
            }
        }

        return nullptr;
    }

    void BasicConfigurationPage::BoxedGuestType(IInspectable const& value)
    {
        m_viewModel.GuestType(winrt::unbox_value<GuestType>(value));
    }
}
