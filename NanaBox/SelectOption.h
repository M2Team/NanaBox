#pragma once

#include "SelectOption.g.h"

#include <Mile.Helpers.CppWinRT.h>

namespace winrt::Mile
{
    using namespace ::Mile;
}

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Data::PropertyChangedEventArgs;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;
}

namespace winrt::NanaBox::implementation
{
    struct SelectOption : SelectOptionT<SelectOption>
    {
        SelectOption(hstring const& name, uint32_t value);

        hstring Name();
        void Name(hstring const& value);
        uint32_t Value();
        void Value(uint32_t value);

        Mile::WinRT::Event<PropertyChangedEventHandler> PropertyChanged;

    private:
        hstring m_name;
        uint32_t m_value;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SelectOption : SelectOptionT<SelectOption, implementation::SelectOption>
    {
    };
}
