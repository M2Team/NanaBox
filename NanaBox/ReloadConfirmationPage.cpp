#include "pch.h"
#include "ReloadConfirmationPage.h"
#if __has_include("ReloadConfirmationPage.g.cpp")
#include "ReloadConfirmationPage.g.cpp"
#endif

#include "Utils.h"

namespace winrt::NanaBox::implementation
{
    ReloadConfirmationPage::ReloadConfirmationPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle),
        m_Status(NanaBox::ReloadConfirmationStatus::Cancel)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"ReloadConfirmationPage/GridTitleTextBlock/Text").c_str());
    }

    void ReloadConfirmationPage::InitializeComponent()
    {
        ReloadConfirmationPageT::InitializeComponent();
    }

    void ReloadConfirmationPage::ReloadButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_Status = NanaBox::ReloadConfirmationStatus::Reload;
        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
    }

    void ReloadConfirmationPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
    }

    NanaBox::ReloadConfirmationStatus ReloadConfirmationPage::Status()
    {
        return this->m_Status;
    }
}
