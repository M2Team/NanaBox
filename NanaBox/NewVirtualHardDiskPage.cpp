#include "pch.h"
#include "NewVirtualHardDiskPage.h"
#if __has_include("NewVirtualHardDiskPage.g.cpp")
#include "NewVirtualHardDiskPage.g.cpp"
#endif

#include "Utils.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    NewVirtualHardDiskPage::NewVirtualHardDiskPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"NewVirtualHardDiskPage/GridTitleTextBlock/Text").c_str());
    }

    void NewVirtualHardDiskPage::InitializeComponent()
    {
        NewVirtualHardDiskPageT::InitializeComponent();
    }

    void NewVirtualHardDiskPage::CreateButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }

    void NewVirtualHardDiskPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
