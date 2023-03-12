#pragma once

#include "BasicConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct BasicConfigurationPage : BasicConfigurationPageT<BasicConfigurationPage>
    {
        BasicConfigurationPage() = default;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct BasicConfigurationPage : BasicConfigurationPageT<BasicConfigurationPage, implementation::BasicConfigurationPage>
    {
    };
}
