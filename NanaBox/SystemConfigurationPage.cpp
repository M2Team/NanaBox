#include "pch.h"
#include "SystemConfigurationPage.h"
#if __has_include("SystemConfigurationPage.g.cpp")
#include "SystemConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    SystemConfigurationPage::SystemConfigurationPage()
    {
        uint32_t processorCount = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
        if (processorCount <= 0) processorCount = 1;

        m_allowedProcessorCount = winrt::single_threaded_vector<IInspectable>();
        for (uint32_t i = 1; i <= processorCount; i++)
        {
            m_allowedProcessorCount.Append(winrt::box_value<uint32_t>(i));
        }
    }

    void SystemConfigurationPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        auto vm = args.Parameter().try_as<ConfigurationViewModel>();
        if (vm) m_viewModel = vm;
    }

    winrt::NanaBox::ConfigurationViewModel SystemConfigurationPage::ViewModel()
    {
        return m_viewModel;
    }

    IVector<IInspectable> SystemConfigurationPage::AllowedProcessorCount()
    {
        return m_allowedProcessorCount;
    }

    hstring SystemConfigurationPage::UInt32ToString(uint32_t value)
    {
        return winrt::to_hstring(value);
    }


    IInspectable SystemConfigurationPage::BoxedProcessorCount()
    {
        uint32_t value = m_viewModel.ProcessorCount();
        for (auto&& item : m_allowedProcessorCount)
        {
            if (winrt::unbox_value<uint32_t>(item) == value)
            {
                return item;
            }
        }

        return nullptr;
    }

    void SystemConfigurationPage::BoxedProcessorCount(IInspectable const& value)
    {
        m_viewModel.ProcessorCount(winrt::unbox_value<uint32_t>(value));
    }
}
