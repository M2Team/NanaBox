#pragma once

#include "ReloadConfirmationPage.g.h"

#include <Windows.h>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct ReloadConfirmationPage :
        ReloadConfirmationPageT<ReloadConfirmationPage>
    {
    public:

        ReloadConfirmationPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

        void ReloadButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CancelButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        NanaBox::ReloadConfirmationStatus Status();

    private:

        HWND m_WindowHandle;
        NanaBox::ReloadConfirmationStatus m_Status;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct ReloadConfirmationPage : ReloadConfirmationPageT<
        ReloadConfirmationPage, implementation::ReloadConfirmationPage>
    {
    };
}
