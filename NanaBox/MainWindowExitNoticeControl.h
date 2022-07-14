#pragma once

#include "MainWindowExitNoticeControl.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    using RequestSignalEvent =
        winrt::event<NanaBox::RequestSignalDelegate>;

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

        winrt::event_token RequestCloseDialog(
            NanaBox::RequestSignalDelegate const& Handler);

        void RequestCloseDialog(
            winrt::event_token const& Token);

    private:

        NanaBox::MainWindowExitNoticeStatus m_Status;

        RequestSignalEvent m_RequestCloseDialog;
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
