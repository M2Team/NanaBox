#pragma once

#include "MessagePage.g.h"

#include <Windows.h>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct MessagePage : MessagePageT<MessagePage>
    {
        MessagePage(
            _In_ HWND WindowHandle = nullptr,
            _In_ winrt::hstring const& InstructionText = winrt::hstring(),
            _In_ winrt::hstring const& ContentText = winrt::hstring());

        void InitializeComponent();

        void ConfirmButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);
    private:

        HWND m_WindowHandle;
        winrt::hstring m_InstructionText;
        winrt::hstring m_ContentText;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MessagePage :
        MessagePageT<MessagePage, implementation::MessagePage>
    {
    };
}
