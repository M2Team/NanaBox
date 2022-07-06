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
}
