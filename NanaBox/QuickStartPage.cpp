#include "pch.h"
#include "QuickStartPage.h"
#if __has_include("QuickStartPage.g.cpp")
#include "QuickStartPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    QuickStartPage::QuickStartPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"QuickStartPage/GridTitleTextBlock/Text").c_str());

        HICON ApplicationIconHandle = reinterpret_cast<HICON>(::LoadImageW(
            ::GetModuleHandleW(nullptr),
            MAKEINTRESOURCE(IDI_NANABOX),
            IMAGE_ICON,
            256,
            256,
            LR_SHARED));
        if (ApplicationIconHandle)
        {
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                TRUE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                FALSE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
        }
    }

    void QuickStartPage::InitializeComponent()
    {
        QuickStartPageT::InitializeComponent();
    }

    void QuickStartPage::LaunchVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void QuickStartPage::CreateVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void QuickStartPage::CreateVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowNewVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::DocumentationButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::LaunchDocumentation();
    }

    void QuickStartPage::AboutButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowAboutDialog(this->m_WindowHandle);
    }
}
