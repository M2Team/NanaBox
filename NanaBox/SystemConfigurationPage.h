#pragma once

#include "SystemConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage>
    {
        SystemConfigurationPage() 
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
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage, implementation::SystemConfigurationPage>
    {
    };
}
