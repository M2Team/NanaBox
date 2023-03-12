#pragma once

#include "ComPortsViewModel.g.h"

namespace winrt::NanaBox::implementation
{
    struct ComPortsViewModel : ComPortsViewModelT<ComPortsViewModel>
    {
        ComPortsViewModel() = default;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ComPortsViewModel : ComPortsViewModelT<ComPortsViewModel, implementation::ComPortsViewModel>
    {
    };
}
