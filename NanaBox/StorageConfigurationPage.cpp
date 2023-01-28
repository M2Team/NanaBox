#include "pch.h"
#include "StorageConfigurationPage.h"
#if __has_include("StorageConfigurationPage.g.cpp")
#include "StorageConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t StorageConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void StorageConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
