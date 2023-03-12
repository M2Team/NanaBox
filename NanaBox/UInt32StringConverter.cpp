#include "pch.h"
#include "UInt32StringConverter.h"
#if __has_include("UInt32StringConverter.g.cpp")
#include "UInt32StringConverter.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    IInspectable UInt32StringConverter::Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        auto pv = value.try_as<winrt::Windows::Foundation::IPropertyValue>();
        if (pv) return winrt::box_value(winrt::to_hstring(pv.GetUInt32()));

        throw hresult_not_implemented();
    }

    IInspectable UInt32StringConverter::ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(value);
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        throw hresult_not_implemented();
    }
}
