#include "pch.h"
#include "BasicConfigurationPage.h"
#if __has_include("BasicConfigurationPage.g.cpp")
#include "BasicConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    void BasicConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel BasicConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }
}
