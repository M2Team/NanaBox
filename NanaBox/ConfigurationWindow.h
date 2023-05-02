#pragma once

#include "pch.h"

#include <filesystem>

#include "ConfigurationWindowControl.h"
#include "VirtualMachineConfiguration.h"

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlhost.h>
#include <atltypes.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>

namespace winrt
{
    using Windows::UI::Xaml::Hosting::DesktopWindowXamlSource;
}

namespace NanaBox
{

    class ConfigurationWindow :
        public ATL::CWindowImpl<ConfigurationWindow>,
        public WTL::CMessageFilter
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaBoxVirtualMachineConfigurationWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(MainWindowExitNoticeWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()

        virtual BOOL PreTranslateMessage(
            MSG* pMsg);

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnDestroy();

        bool Initialize();

        void LoadConfiguration(std::filesystem::path const& path);
    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::DesktopWindowXamlSource m_XamlSource;
        winrt::NanaBox::ConfigurationWindowControl m_ConfigurationControl = nullptr;
        NanaBox::VirtualMachineConfiguration m_Configuration;
        std::filesystem::path m_ConfigurationPath;
    };

}
