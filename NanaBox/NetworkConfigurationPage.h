#pragma once

#include "NetworkConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage>
    {
        NetworkConfigurationPage() = default;        
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct NetworkConfigurationPage : NetworkConfigurationPageT<NetworkConfigurationPage, implementation::NetworkConfigurationPage>
    {
    };
}
