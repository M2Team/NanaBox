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

        ::EnableWindow(this->m_WindowHandle, FALSE);

        ::ShowNewVirtualHardDiskDialog(this->m_WindowHandle);

        ::EnableWindow(this->m_WindowHandle, TRUE);
        ::SetActiveWindow(this->m_WindowHandle);
    }

    void QuickStartPage::DocumentationButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = { 0 };
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile =
            L"https://github.com/M2Team/NanaBox/"
            L"blob/main/Documents/ConfigurationReference.md";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }

    void QuickStartPage::AboutButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::EnableWindow(this->m_WindowHandle, FALSE);

        ::ShowAboutDialog(this->m_WindowHandle);

        ::EnableWindow(this->m_WindowHandle, TRUE);
        ::SetActiveWindow(this->m_WindowHandle);
    }

    void QuickStartPage::ExitButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
