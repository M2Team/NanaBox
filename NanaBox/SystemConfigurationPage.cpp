#include "pch.h"
#include "SystemConfigurationPage.h"
#if __has_include("SystemConfigurationPage.g.cpp")
#include "SystemConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t SystemConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SystemConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
