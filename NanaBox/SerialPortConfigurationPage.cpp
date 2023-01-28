#include "pch.h"
#include "SerialPortConfigurationPage.h"
#if __has_include("SerialPortConfigurationPage.g.cpp")
#include "SerialPortConfigurationPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    int32_t SerialPortConfigurationPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void SerialPortConfigurationPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

}
