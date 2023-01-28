#include "pch.h"
#include "NetworkConfigurationPage.h"
#if __has_include("NetworkConfigurationPage.g.cpp")
#include "NetworkConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t NetworkConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void NetworkConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
