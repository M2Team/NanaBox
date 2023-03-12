#pragma once

#include "SystemConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage>
    {
        SystemConfigurationPage() = default;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SystemConfigurationPage : SystemConfigurationPageT<SystemConfigurationPage, implementation::SystemConfigurationPage>
    {
    };
}
