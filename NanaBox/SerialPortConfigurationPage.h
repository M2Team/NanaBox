#pragma once

#include "SerialPortConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct SerialPortConfigurationPage : SerialPortConfigurationPageT<SerialPortConfigurationPage>
    {
        SerialPortConfigurationPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SerialPortConfigurationPage : SerialPortConfigurationPageT<SerialPortConfigurationPage, implementation::SerialPortConfigurationPage>
    {
    };
}
