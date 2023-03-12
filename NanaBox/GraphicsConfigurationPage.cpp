#include "pch.h"
#include "GraphicsConfigurationPage.h"
#if __has_include("GraphicsConfigurationPage.g.cpp")
#include "GraphicsConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    void GraphicsConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel GraphicsConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }
}
