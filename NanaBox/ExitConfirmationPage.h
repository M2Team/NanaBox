#pragma once

#include "ExitConfirmationPage.g.h"

#include <Windows.h>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct ExitConfirmationPage :
        ExitConfirmationPageT<ExitConfirmationPage>
    {
    public:

        ExitConfirmationPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

        void SuspendButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void PowerOffButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CancelButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        NanaBox::ExitConfirmationStatus Status();

    private:

        HWND m_WindowHandle;
        NanaBox::ExitConfirmationStatus m_Status;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ExitConfirmationPage :
        ExitConfirmationPageT<
            ExitConfirmationPage,
            implementation::ExitConfirmationPage>
    {
    };
}
