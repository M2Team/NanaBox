#include "pch.h"
#include "ExitConfirmationPage.h"
#if __has_include("ExitConfirmationPage.g.cpp")
#include "ExitConfirmationPage.g.cpp"
#endif

#include "Utils.h"

namespace winrt::NanaBox::implementation
{
    ExitConfirmationPage::ExitConfirmationPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle),
        m_Status(NanaBox::ExitConfirmationStatus::Cancel)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"ExitConfirmationPage/GridTitleTextBlock/Text").c_str());
    }

    void ExitConfirmationPage::InitializeComponent()
    {
        ExitConfirmationPageT::InitializeComponent();
    }

    void ExitConfirmationPage::SuspendButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_Status = NanaBox::ExitConfirmationStatus::Suspend;
        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
    }

    void ExitConfirmationPage::PowerOffButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_Status = NanaBox::ExitConfirmationStatus::PowerOff;
        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
    }

    void ExitConfirmationPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
    }

    NanaBox::ExitConfirmationStatus ExitConfirmationPage::Status()
    {
        return this->m_Status;
    }
}
