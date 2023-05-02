#pragma once

#include "BooleanToStringConverter.g.h"

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::Interop::TypeName;
}
namespace winrt::NanaBox::implementation
{
    struct BooleanToStringConverter : BooleanToStringConverterT<BooleanToStringConverter>
    {
        BooleanToStringConverter() = default;

        hstring TrueValue();
        void TrueValue(hstring const& value);
        hstring FalseValue();
        void FalseValue(hstring const& value);
        IInspectable Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);
        IInspectable ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language);

    private:
        hstring m_true;
        hstring m_false;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct BooleanToStringConverter : BooleanToStringConverterT<BooleanToStringConverter, implementation::BooleanToStringConverter>
    {
    };
}
