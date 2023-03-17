#include "pch.h"
#include "SystemConfigurationPage.h"
#if __has_include("SystemConfigurationPage.g.cpp")
#include "SystemConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{    
    void SystemConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel SystemConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }
}
