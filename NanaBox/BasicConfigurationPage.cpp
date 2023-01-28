#include "pch.h"
#include "BasicConfigurationPage.h"
#if __has_include("BasicConfigurationPage.g.cpp")
#include "BasicConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t BasicConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void BasicConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
