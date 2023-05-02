#pragma once

#include "RelayCommand.g.h"

#include <functional>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::EventHandler;
}
namespace winrt::NanaBox::implementation
{
    struct RelayCommand : RelayCommandT<RelayCommand>
    {
        RelayCommand(
            std::function<void(winrt::IInspectable)> action,
            std::function<bool(winrt::IInspectable)> canExecute);

        RelayCommand(std::function<void(winrt::IInspectable)> action);

        winrt::event_token CanExecuteChanged(winrt::EventHandler<winrt::IInspectable> const& handler);
        void CanExecuteChanged(winrt::event_token const& token) noexcept;
        bool CanExecute(winrt::IInspectable const& parameter);
        void Execute(winrt::IInspectable const& parameter);

        void RaiseCanExecuteChanged();
    private:
        std::function<void(winrt::IInspectable)> m_action;
        std::function<bool(winrt::IInspectable)> m_canExecute;
        winrt::event<winrt::EventHandler<winrt::IInspectable>> m_canExecuteChanged;
    };
}
