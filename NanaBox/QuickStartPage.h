#pragma once

#include "QuickStartPage.g.h"

#include <Windows.h>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct QuickStartPage : QuickStartPageT<QuickStartPage>
    {
    public:

        QuickStartPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

        void LaunchVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CreateVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CreateVirtualHardDiskButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void DocumentationButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void AboutButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

    private:

        HWND m_WindowHandle;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct QuickStartPage :
        QuickStartPageT<QuickStartPage, implementation::QuickStartPage>
    {
    };
}
