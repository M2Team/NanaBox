#pragma once

#include "MainWindowControl.g.h"

#include <Windows.h>

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

        MainWindowControl(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

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

    private:

        HWND m_WindowHandle;
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
