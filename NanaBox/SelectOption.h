#pragma once

#include "SelectOption.g.h"

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

        winrt::event_token PropertyChanged(PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
    private:
        hstring m_name;
        uint32_t m_value;
        winrt::event<PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct SelectOption : SelectOptionT<SelectOption, implementation::SelectOption>
    {
    };
}
