#include "pch.h"
#include "SelectOption.h"
#if __has_include("SelectOption.g.cpp")
#include "SelectOption.g.cpp"
#endif

namespace winrt::NanaBox::implementation
{
    SelectOption::SelectOption(hstring const& name, uint32_t value)
        : m_name{ name }, m_value{ value } {}
    
    hstring SelectOption::Name()
    {
        return m_name;
    }
    void SelectOption::Name(hstring const& value)
    {
        if (m_name != value)
        {
            m_name = value;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"Name" });
        }
    }

    uint32_t SelectOption::Value()
    {
        return m_value;
    }
    void SelectOption::Value(uint32_t value)
    {
        if (m_value != value)
        {
            m_value = value;
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"Value" });
        }
    }

    winrt::event_token SelectOption::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }
    void SelectOption::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
