#pragma once

#include "MainWindowExitNoticeControl.g.h"

#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    using RequestSignalEvent =
        Mile::WinRT::Event<NanaBox::RequestSignalDelegate>;

    struct MainWindowExitNoticeControl : MainWindowExitNoticeControlT<
        MainWindowExitNoticeControl>
    {
    public:

        MainWindowExitNoticeControl();

        void SuspendButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void PowerOffButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CancelButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        NanaBox::MainWindowExitNoticeStatus Status();

        RequestSignalEvent RequestCloseDialog;

    private:

        NanaBox::MainWindowExitNoticeStatus m_Status;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MainWindowExitNoticeControl : MainWindowExitNoticeControlT<
        MainWindowExitNoticeControl,
        implementation::MainWindowExitNoticeControl>
    {
    };
}
