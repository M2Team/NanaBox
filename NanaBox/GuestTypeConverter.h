#pragma once

#include "GuestTypeConverter.g.h"


namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::NanaBox::implementation
{
    struct GuestTypeConverter : GuestTypeConverterT<GuestTypeConverter>
    {
        GuestTypeConverter() = default;

        IInspectable Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);
        IInspectable ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct GuestTypeConverter : GuestTypeConverterT<GuestTypeConverter, implementation::GuestTypeConverter>
    {
    };
}
