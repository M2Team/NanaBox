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
    MainWindowControl::MainWindowControl()
    {
        this->InitializeComponent();
    }

    void MainWindowControl::EnhancedSessionButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        this->m_RequestEnhancedSession(winrt::unbox_value<bool>(
            sender.as<winrt::AppBarToggleButton>().IsChecked()));
    }

    void MainWindowControl::PauseVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        this->m_RequestPauseVirtualMachine(winrt::unbox_value<bool>(
            sender.as<winrt::AppBarToggleButton>().IsChecked()));
    }

    void MainWindowControl::ShutdownVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_RequestShutdownVirtualMachine();
    }

    void MainWindowControl::RestartVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_RequestRestartVirtualMachine();
    }

    winrt::event_token MainWindowControl::RequestEnhancedSession(
        NanaBox::RequestStateChangedDelegate const& Handler)
    {
        return this->m_RequestEnhancedSession.add(Handler);
    }

    void MainWindowControl::RequestEnhancedSession(
        winrt::event_token const& Token)
    {
        this->m_RequestEnhancedSession.remove(Token);
    }

    winrt::event_token MainWindowControl::RequestPauseVirtualMachine(
        NanaBox::RequestStateChangedDelegate const& Handler)
    {
        return this->m_RequestPauseVirtualMachine.add(Handler);
    }

    void MainWindowControl::RequestPauseVirtualMachine(
        winrt::event_token const& Token)
    {
        this->m_RequestPauseVirtualMachine.remove(Token);
    }

    winrt::event_token MainWindowControl::RequestShutdownVirtualMachine(
        NanaBox::RequestSignalDelegate const& Handler)
    {
        return this->m_RequestShutdownVirtualMachine.add(Handler);
    }

    void MainWindowControl::RequestShutdownVirtualMachine(
        winrt::event_token const& Token)
    {
        this->m_RequestShutdownVirtualMachine.remove(Token);
    }

    winrt::event_token MainWindowControl::RequestRestartVirtualMachine(
        NanaBox::RequestSignalDelegate const& Handler)
    {
        return this->m_RequestRestartVirtualMachine.add(Handler);
    }

    void MainWindowControl::RequestRestartVirtualMachine(
        winrt::event_token const& Token)
    {
        this->m_RequestRestartVirtualMachine.remove(Token);
    }
}
