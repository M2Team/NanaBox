#include "pch.h"
#include "GraphicsConfigurationPage.h"
#if __has_include("GraphicsConfigurationPage.g.cpp")
#include "GraphicsConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t GraphicsConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void GraphicsConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
