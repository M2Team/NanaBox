#pragma once

#include "NetworkConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage>
    {
        NetworkConfigurationPage() 
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
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage, implementation::NetworkConfigurationPage>
    {
    };
}
