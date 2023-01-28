#include "pch.h"
#include "ViewModel.h"
#if __has_include("ViewModel.g.cpp")
#include "ViewModel.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    int32_t ViewModel::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ViewModel::MyProperty(int32_t /*value*/)
    {
        throw hresult_not_implemented();
    }
}
