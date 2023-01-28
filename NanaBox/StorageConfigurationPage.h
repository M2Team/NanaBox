#pragma once

#include "StorageConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage>
    {
        StorageConfigurationPage() 
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
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage, implementation::StorageConfigurationPage>
    {
    };
}
