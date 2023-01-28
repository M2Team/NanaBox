#pragma once

#include "ViewModel.g.h"

namespace winrt::NanaBox::implementation
{
    struct ViewModel : ViewModelT<ViewModel>
    {
        ViewModel() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ViewModel : ViewModelT<ViewModel, implementation::ViewModel>
    {
    };
}
