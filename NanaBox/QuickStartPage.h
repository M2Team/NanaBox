#pragma once

#include "QuickStartPage.g.h"

#include <Windows.h>

namespace winrt::NanaBox::implementation
{
    struct QuickStartPage : QuickStartPageT<QuickStartPage>
    {
    public:

        QuickStartPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

    private:

        HWND m_WindowHandle;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct QuickStartPage :
        QuickStartPageT<QuickStartPage, implementation::QuickStartPage>
    {
    };
}
