#include "pch.h"
#include "ResizeVirtualHardDiskPage.h"
#if __has_include("ResizeVirtualHardDiskPage.g.cpp")
#include "ResizeVirtualHardDiskPage.g.cpp"
#endif

#include "Utils.h"

#include <regex>

#include <ShObjIdl_core.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::NanaBox::implementation
{
    ResizeVirtualHardDiskPage::ResizeVirtualHardDiskPage(
        _In_ HWND WindowHandle)
        : m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"ResizeVirtualHardDiskPage/GridTitleTextBlock/Text").c_str());
    }


    void ResizeVirtualHardDiskPage::InitializeComponent() {
        ResizeVirtualHardDiskPageT::InitializeComponent();

        this->m_DispatcherQueue =
            winrt::DispatcherQueue::GetForCurrentThread();
    }

    void ResizeVirtualHardDiskPage::ResizeButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void ResizeVirtualHardDiskPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        // 在这里添加取消操作的逻辑，比如返回上一个页面
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }

    void ResizeVirtualHardDiskPage::FileNameBrowseButtonClickHandler(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e) {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
    }


    void ResizeVirtualHardDiskPage::NaturalNumberTextBoxBeforeTextChanging(
        winrt::TextBox const& sender,
        winrt::TextBoxBeforeTextChangingEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
        for (wchar_t const& c : e.NewText())
        {
            if (!iswdigit(c))
            {
                e.Cancel(true);
                return;
            }
        }
    }
}
