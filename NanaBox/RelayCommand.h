#pragma once

#include "RelayCommand.g.h"

#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}

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

        Mile::WinRT::Event<winrt::EventHandler<winrt::IInspectable>> CanExecuteChanged;

        bool CanExecute(winrt::IInspectable const& parameter);
        void Execute(winrt::IInspectable const& parameter);

        void RaiseCanExecuteChanged();
    private:
        std::function<void(winrt::IInspectable)> m_action;
        std::function<bool(winrt::IInspectable)> m_canExecute;
    };
}
