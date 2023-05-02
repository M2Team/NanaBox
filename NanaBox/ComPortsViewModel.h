#pragma once

#include "VirtualMachineConfiguration.h"
#include "ComPortsViewModel.g.h"

#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::Foundation::Collections::IVector;
    using Windows::Foundation::Collections::IObservableVector;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
}

namespace winrt::NanaBox::implementation
{
    struct ComPortsViewModel : ComPortsViewModelT<ComPortsViewModel>
    {
        ComPortsViewModel(::NanaBox::ComPortsConfiguration* comConfig);

        NanaBox::SelectOption UefiConsoleMode();
        void UefiConsoleMode(NanaBox::SelectOption const& value);
        hstring Com1RedirectPath();
        void Com1RedirectPath(hstring const& value);
        hstring Com2RedirectPath();
        void Com2RedirectPath(hstring const& value);

        IObservableVector<NanaBox::SelectOption> UefiConsoleModeList();

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    private:
        ::NanaBox::ComPortsConfiguration* m_comConfig{ nullptr };
        hstring m_com1Path;
        hstring m_com2Path;
        NanaBox::SelectOption m_consoleMode{ nullptr };

        IObservableVector<NanaBox::SelectOption> m_consoleModeList{ nullptr };
    };
}
