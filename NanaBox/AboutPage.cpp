#include "pch.h"
#include "AboutPage.h"
#if __has_include("AboutPage.g.cpp")
#include "AboutPage.g.cpp"
#endif

#include <Windows.h>

#include <Mile.Project.Version.h>

#include <winrt/Windows.UI.Xaml.Documents.h>

#include "Utils.h"

namespace winrt::NanaBox::implementation
{
    AboutPage::AboutPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"AboutPage/GridTitleTextBlock/Text").c_str());
    }

    void AboutPage::InitializeComponent()
    {
        AboutPageT::InitializeComponent();

        this->Version().Text(winrt::hstring(
            L"NanaBox " MILE_PROJECT_VERSION_STRING
            L" (" MILE_PROJECT_DOT_VERSION_STRING L")"));
    }

    void AboutPage::GitHubButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = {};
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile = L"https://github.com/M2Team/NanaBox";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }

    void AboutPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
