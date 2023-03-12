#pragma once

#include "StorageConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage>
    {
        StorageConfigurationPage() = default;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct StorageConfigurationPage : StorageConfigurationPageT<StorageConfigurationPage, implementation::StorageConfigurationPage>
    {
    };
}
