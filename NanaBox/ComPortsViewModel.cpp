#include "pch.h"
#include "ComPortsViewModel.h"
#if __has_include("ComPortsViewModel.g.cpp")
#include "ComPortsViewModel.g.cpp"
#endif

#include "SelectOption.h"

namespace winrt::NanaBox::implementation
{
    ComPortsViewModel::ComPortsViewModel(::NanaBox::ComPortsConfiguration* comConfig)
        : m_comConfig {comConfig}
    {
        m_com1Path = winrt::to_hstring(comConfig->ComPort1);
        m_com2Path = winrt::to_hstring(comConfig->ComPort2);

        m_consoleModeList = winrt::single_threaded_observable_vector<NanaBox::SelectOption>({
            winrt::make<implementation::SelectOption>(L"[Disabled]",
                static_cast<uint32_t>(::NanaBox::UefiConsoleMode::Disabled)),
            winrt::make<implementation::SelectOption>(L"[Default]",
                static_cast<uint32_t>(::NanaBox::UefiConsoleMode::Default)),
            winrt::make<implementation::SelectOption>(L"[COM1]",
                static_cast<uint32_t>(::NanaBox::UefiConsoleMode::ComPort1)),
            winrt::make<implementation::SelectOption>(L"[COM2]",
                static_cast<uint32_t>(::NanaBox::UefiConsoleMode::ComPort2))
        });

        m_consoleMode = nullptr;        
        for (auto&& item : m_consoleModeList)
        {
            if (static_cast<::NanaBox::UefiConsoleMode>(item.Value()) == m_comConfig->UefiConsole)
            {
                m_consoleMode = item;
                break;
            }
        }        

    }

    NanaBox::SelectOption ComPortsViewModel::UefiConsoleMode()
    {
        return m_consoleMode;
    }
    void ComPortsViewModel::UefiConsoleMode(NanaBox::SelectOption const& value)
    {
        if (m_consoleMode != value)
        {
            m_consoleMode = value;
            m_comConfig->UefiConsole = static_cast<::NanaBox::UefiConsoleMode>(value.Value());
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"UefiConsoleMode" });
        }
    }

    hstring ComPortsViewModel::Com1RedirectPath()
    {
        return m_com1Path;
    }
    void ComPortsViewModel::Com1RedirectPath(hstring const& value)
    {
        if (m_com1Path != value)
        {
            m_com1Path = value;
            m_comConfig->ComPort1 = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"Com1RedirectPath" });
        }
    }

    hstring ComPortsViewModel::Com2RedirectPath()
    {
        return m_com2Path;
    }
    void ComPortsViewModel::Com2RedirectPath(hstring const& value)
    {
        if (m_com2Path != value)
        {
            m_com2Path = value;
            m_comConfig->ComPort2 = winrt::to_string(value);
            m_propertyChanged(*this, PropertyChangedEventArgs{ L"Com2RedirectPath" });
        }
    }

    IObservableVector<NanaBox::SelectOption> ComPortsViewModel::UefiConsoleModeList()
    {
        return m_consoleModeList;
    }

    winrt::event_token ComPortsViewModel::PropertyChanged(PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }
    void ComPortsViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
