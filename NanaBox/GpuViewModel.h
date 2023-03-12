#pragma once

#include "GpuViewModel.g.h"

namespace winrt::NanaBox::implementation
{
    struct GpuViewModel : GpuViewModelT<GpuViewModel>
    {
        GpuViewModel() = default;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct GpuViewModel : GpuViewModelT<GpuViewModel, implementation::GpuViewModel>
    {
    };
}
