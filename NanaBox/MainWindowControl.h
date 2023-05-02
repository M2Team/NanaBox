#pragma once

#include "MainWindowControl.g.h"

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

    using RequestStateChangedEvent =
        Mile::WinRT::Event<NanaBox::RequestStateChangedDelegate>;

    struct MainWindowControl : MainWindowControlT<MainWindowControl>
    {
    public:

        MainWindowControl();

        void EnhancedSessionButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void FullScreenButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void PauseVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void RestartVirtualMachineButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        RequestStateChangedEvent RequestEnhancedSession;
        RequestSignalEvent RequestFullScreen;
        RequestStateChangedEvent RequestPauseVirtualMachine;
        RequestSignalEvent RequestRestartVirtualMachine;
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
