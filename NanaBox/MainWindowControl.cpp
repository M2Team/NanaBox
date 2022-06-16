#include "pch.h"
#include "MainWindowControl.h"
#if __has_include("MainWindowControl.g.cpp")
#include "MainWindowControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    MainWindowControl::MainWindowControl()
    {
        InitializeComponent();
    }

    int32_t MainWindowControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindowControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindowControl::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
