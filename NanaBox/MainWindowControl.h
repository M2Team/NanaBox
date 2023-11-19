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

namespace NanaBox
{
    namespace MainWindowCommands
    {
        enum
        {
            EnterBasicSession = 0x1001,
            EnterEnhancedSession = 0x1002,
            EnterFullScreen = 0x1003,
            PauseVirtualMachine = 0x1004,
            ResumeVirtualMachine = 0x1005,
            RestartVirtualMachine = 0x1006,
            VirtualMachineSettings = 0x1007,
            ReloadVirtualMachineSettings = 0x1008,
            CreateVirtualHardDisk = 0x1009,
            Documentation = 0x100A,
            About = 0x100B,
        };
    }
}

namespace winrt::NanaBox
{
    using namespace ::NanaBox;
}

namespace winrt::NanaBox::implementation
{
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

        void VirtualMachineSettingsButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void ReloadVirtualMachineSettingsButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CreateVirtualHardDiskButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void DocumentationButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void AboutButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

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
