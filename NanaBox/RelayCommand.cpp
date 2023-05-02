#include "pch.h"
#include "RelayCommand.h"
#if __has_include("RelayCommand.g.cpp")
#include "RelayCommand.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    RelayCommand::RelayCommand(std::function<void(winrt::IInspectable)> action)
    {
        m_action = action;
        m_canExecute = [](IInspectable) { return true; };
    }

    RelayCommand::RelayCommand(
        std::function<void(winrt::IInspectable)> action,
        std::function<bool(winrt::IInspectable)> canExecute)
    {
        m_action = action;
        m_canExecute = canExecute;
    }

    bool RelayCommand::CanExecute(winrt::IInspectable const& parameter)
    {
        return m_canExecute(parameter);
    }

    void RelayCommand::Execute(winrt::IInspectable const& parameter)
    {
        m_action(parameter);
    }

    void RelayCommand::RaiseCanExecuteChanged()
    {
        this->CanExecuteChanged(*this, nullptr);
    }
}
