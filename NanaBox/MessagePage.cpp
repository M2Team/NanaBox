#include "pch.h"
#include "MessagePage.h"
#if __has_include("MessagePage.g.cpp")
#include "MessagePage.g.cpp"
#endif

#include "Utils.h"

namespace winrt::NanaBox::implementation
{
    MessagePage::MessagePage(
        _In_ HWND WindowHandle,
        _In_ winrt::hstring const& InstructionText,
        _In_ winrt::hstring const& ContentText) :
        m_WindowHandle(WindowHandle),
        m_InstructionText(InstructionText),
        m_ContentText(ContentText)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            this->m_InstructionText.c_str());
    }

    void MessagePage::InitializeComponent()
    {
        MessagePageT::InitializeComponent();

        this->InstructionTextBlock().Text(this->m_InstructionText);
        this->ContentTextBlock().Text(this->m_ContentText);
    }

    void MessagePage::ConfirmButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
