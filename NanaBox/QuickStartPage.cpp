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
}
