#pragma once

#include "GraphicsConfigurationPage.g.h"

namespace winrt::NanaBox::implementation
{
    struct GraphicsConfigurationPage : GraphicsConfigurationPageT<GraphicsConfigurationPage>
    {
        GraphicsConfigurationPage() = default;        
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct GraphicsConfigurationPage : GraphicsConfigurationPageT<GraphicsConfigurationPage, implementation::GraphicsConfigurationPage>
    {
    };
}
