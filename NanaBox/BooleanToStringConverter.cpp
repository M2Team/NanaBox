#include "pch.h"
#include "BooleanToStringConverter.h"
#if __has_include("BooleanToStringConverter.g.cpp")
#include "BooleanToStringConverter.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    hstring BooleanToStringConverter::TrueValue()
    {
        return m_true;
    }

    void BooleanToStringConverter::TrueValue(hstring const& value)
    {
        m_true = value;
    }

    hstring BooleanToStringConverter::FalseValue()
    {
        return m_false;
    }
    void BooleanToStringConverter::FalseValue(hstring const& value)
    {
        m_false = value;
    }

    IInspectable BooleanToStringConverter::Convert(IInspectable const& value,
        TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        bool v = winrt::unbox_value<bool>(value);
        hstring str = v ? m_true : m_false;
        return winrt::box_value(str);
    }

    IInspectable BooleanToStringConverter::ConvertBack(IInspectable const& value,
        TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(value);
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        throw hresult_not_implemented();
    }
}
