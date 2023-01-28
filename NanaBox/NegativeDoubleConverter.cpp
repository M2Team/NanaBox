#include "pch.h"
#include "NegativeDoubleConverter.h"
#if __has_include("NegativeDoubleConverter.g.cpp")
#include "NegativeDoubleConverter.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    winrt::Windows::Foundation::IInspectable
        NegativeDoubleConverter::Convert(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);
        
        double negative = -1.0 * winrt::unbox_value<double>(value);
        return winrt::box_value<double>(negative);
    }

    winrt::Windows::Foundation::IInspectable
        NegativeDoubleConverter::ConvertBack(
            winrt::Windows::Foundation::IInspectable const& value,
            winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
            winrt::Windows::Foundation::IInspectable const& parameter,
            hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        double negative = -1.0 * winrt::unbox_value<double>(value);
        return winrt::box_value<double>(negative);
    }
}
