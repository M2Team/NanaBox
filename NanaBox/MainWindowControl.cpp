#include "pch.h"
#include "MainWindowControl.h"
#if __has_include("MainWindowControl.g.cpp")
#include "MainWindowControl.g.cpp"
#endif

namespace winrt
{
    using Windows::UI::Xaml::Controls::AppBarToggleButton;
}

namespace winrt::NanaBox::implementation
{
    MainWindowControl::MainWindowControl(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {

    }

    void MainWindowControl::InitializeComponent()
    {
        MainWindowControlT::InitializeComponent();
    }

    void MainWindowControl::EnhancedSessionButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            (winrt::unbox_value<bool>(
                sender.as<winrt::AppBarToggleButton>().IsChecked())
                ? MAKEWPARAM(
                    NanaBox::MainWindowCommands::EnterEnhancedSession,
                    BN_CLICKED)
                : MAKEWPARAM(
                    NanaBox::MainWindowCommands::EnterBasicSession,
                    BN_CLICKED)),
            0);
    }

    void MainWindowControl::FullScreenButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::EnterFullScreen,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::PauseVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            (winrt::unbox_value<bool>(
                sender.as<winrt::AppBarToggleButton>().IsChecked())
                ? MAKEWPARAM(
                    NanaBox::MainWindowCommands::PauseVirtualMachine,
                    BN_CLICKED)
                : MAKEWPARAM(
                    NanaBox::MainWindowCommands::ResumeVirtualMachine,
                    BN_CLICKED)),
            0);
    }

    void MainWindowControl::RestartVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::RestartVirtualMachine,
                BN_CLICKED),
            0);
    }
}
