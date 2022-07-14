#include "pch.h"
#include "MainWindowExitNoticeControl.h"
#if __has_include("MainWindowExitNoticeControl.g.cpp")
#include "MainWindowExitNoticeControl.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    MainWindowExitNoticeControl::MainWindowExitNoticeControl()
        : m_Status(NanaBox::MainWindowExitNoticeStatus::Cancel)
    {
        this->InitializeComponent();
    }

    void MainWindowExitNoticeControl::SuspendButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_Status = NanaBox::MainWindowExitNoticeStatus::Suspend;
        this->m_RequestCloseDialog();
    }

    void MainWindowExitNoticeControl::PowerOffButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_Status = NanaBox::MainWindowExitNoticeStatus::PowerOff;
        this->m_RequestCloseDialog();
    }

    void MainWindowExitNoticeControl::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        this->m_RequestCloseDialog();
    }

    NanaBox::MainWindowExitNoticeStatus MainWindowExitNoticeControl::Status()
    {
        return this->m_Status;
    }

    winrt::event_token MainWindowExitNoticeControl::RequestCloseDialog(
        NanaBox::RequestSignalDelegate const& Handler)
    {
        return this->m_RequestCloseDialog.add(Handler);
    }

    void MainWindowExitNoticeControl::RequestCloseDialog(
        winrt::event_token const& Token)
    {
        this->m_RequestCloseDialog.remove(Token);
    }
}
