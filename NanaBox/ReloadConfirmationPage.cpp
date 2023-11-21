#include "pch.h"
#include "ReloadConfirmationPage.h"
#if __has_include("ReloadConfirmationPage.g.cpp")
#include "ReloadConfirmationPage.g.cpp"
#endif

#include "Utils.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    ReloadConfirmationPage::ReloadConfirmationPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
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
    }

    void ReloadConfirmationPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
