#pragma once

#include "MainWindowControl.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    using RequestSignalEvent =
        winrt::event<NanaBox::RequestSignalDelegate>;

    using RequestStateChangedEvent =
        winrt::event<NanaBox::RequestStateChangedDelegate>;

    struct MainWindowControl : MainWindowControlT<MainWindowControl>
    {
    public:

        MainWindowControl();

        void EnhancedSessionButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void PauseVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void RestartVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        winrt::event_token RequestEnhancedSession(
            NanaBox::RequestStateChangedDelegate const& Handler);

        void RequestEnhancedSession(
            winrt::event_token const& Token);

        winrt::event_token RequestPauseVirtualMachine(
            NanaBox::RequestStateChangedDelegate const& Handler);

        void RequestPauseVirtualMachine(
            winrt::event_token const& Token);

        winrt::event_token RequestRestartVirtualMachine(
            NanaBox::RequestSignalDelegate const& Handler);

        void RequestRestartVirtualMachine(
            winrt::event_token const& Token);

    private:

        RequestStateChangedEvent m_RequestEnhancedSession;
        RequestStateChangedEvent m_RequestPauseVirtualMachine;
        RequestSignalEvent m_RequestRestartVirtualMachine;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct MainWindowControl : MainWindowControlT<
        MainWindowControl,
        implementation::MainWindowControl>
    {
    };
}
