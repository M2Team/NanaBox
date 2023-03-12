#pragma once

#include "UInt32StringConverter.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Interop::TypeName;
}

namespace winrt::NanaBox::implementation
{
    struct UInt32StringConverter : UInt32StringConverterT<UInt32StringConverter>
    {
        UInt32StringConverter() = default;

        IInspectable Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);
        IInspectable ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct UInt32StringConverter : UInt32StringConverterT<UInt32StringConverter, implementation::UInt32StringConverter>
    {
    };
}
