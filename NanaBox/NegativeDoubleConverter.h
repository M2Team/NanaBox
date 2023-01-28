#pragma once

#include "NegativeDoubleConverter.g.h"

namespace winrt::NanaBox::implementation
{
    struct NegativeDoubleConverter : NegativeDoubleConverterT<NegativeDoubleConverter>
    {
        NegativeDoubleConverter() = default;

        winrt::Windows::Foundation::IInspectable Convert(
                winrt::Windows::Foundation::IInspectable const& value,
                winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
                winrt::Windows::Foundation::IInspectable const& parameter,
                hstring const& language);

        winrt::Windows::Foundation::IInspectable ConvertBack(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            hstring const& language);
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct NegativeDoubleConverter : NegativeDoubleConverterT<NegativeDoubleConverter, implementation::NegativeDoubleConverter>
    {
    };
}
