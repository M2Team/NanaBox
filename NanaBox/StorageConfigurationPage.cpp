#include "pch.h"
#include "StorageConfigurationPage.h"
#if __has_include("StorageConfigurationPage.g.cpp")
#include "StorageConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    void StorageConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel StorageConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }
}
