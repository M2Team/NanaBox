#include "pch.h"
#include "NetworkConfigurationPage.h"
#if __has_include("NetworkConfigurationPage.g.cpp")
#include "NetworkConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    void NetworkConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel NetworkConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }
}
