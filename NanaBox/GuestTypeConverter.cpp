#include "pch.h"
#include "GuestTypeConverter.h"
#if __has_include("GuestTypeConverter.g.cpp")
#include "GuestTypeConverter.g.cpp"
#endif

#include "ConfigurationViewModel.h"

namespace winrt::NanaBox::implementation
{
    IInspectable GuestTypeConverter::Convert(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        hstring str;
        GuestType type = winrt::unbox_value<GuestType>(value);

        switch (type) {
        case GuestType::Linux:
            str = L"Linux";
            break;
        case GuestType::Windows:
            str = L"Windows";
            break;
        default:
            str = L"Unknown";
            break;
        }

        return winrt::box_value(str);
    }

    IInspectable GuestTypeConverter::ConvertBack(IInspectable const& value, TypeName const& targetType, IInspectable const& parameter, hstring const& language)
    {
        UNREFERENCED_PARAMETER(targetType);
        UNREFERENCED_PARAMETER(parameter);
        UNREFERENCED_PARAMETER(language);

        hstring str = winrt::unbox_value<hstring>(value);
        GuestType type = GuestType::Unknown;

        if (str == L"Windows")
        {
            type = GuestType::Windows;
        }
        else if (str == L"Linux")
        {
            type = GuestType::Linux;
        }

        return winrt::box_value<GuestType>(type);
    }
}
