#include "pch.h"
#include "QuickStartPage.h"
#if __has_include("QuickStartPage.g.cpp")
#include "QuickStartPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

#include <ShlObj.h>

#include "ConfigurationManager.h"

namespace winrt::NanaBox
{
    using namespace ::NanaBox;
}

namespace winrt::NanaBox::implementation
{
    QuickStartPage::QuickStartPage(
        _In_ HWND WindowHandle,
        _Out_opt_ std::wstring* ConfigurationFilePath) :
        m_WindowHandle(WindowHandle),
        m_ConfigurationFilePath(ConfigurationFilePath)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"QuickStartPage/GridTitleTextBlock/Text").c_str());

        HICON ApplicationIconHandle = reinterpret_cast<HICON>(::LoadImageW(
            ::GetModuleHandleW(nullptr),
            MAKEINTRESOURCE(IDI_NANABOX),
            IMAGE_ICON,
            256,
            256,
            LR_SHARED));
        if (ApplicationIconHandle)
        {
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                TRUE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                FALSE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
        }
    }

    void QuickStartPage::InitializeComponent()
    {
        QuickStartPageT::InitializeComponent();
    }

    void QuickStartPage::LaunchVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
            try
            {
                if (!this->m_ConfigurationFilePath)
                {
                    return;
                }

                std::wstring FileTypeName = Mile::FormatWideString(
                    L"%s (*.7b)",
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationFileTypeName",
                        L"[NanaBox Virtual Machine Configuration]").c_str());

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    {
                        FileTypeName.c_str(),
                        L"*.7b"
                    }
                };
                LPWSTR RawFilePath = nullptr;
                winrt::check_hresult(::ShowFileDialog(
                    this->m_WindowHandle,
                    FALSE,
                    ARRAYSIZE(SupportedFileTypes),
                    SupportedFileTypes,
                    nullptr,
                    nullptr,
                    nullptr,
                    &RawFilePath));
                auto FreeFilePath = Mile::ScopeExitTaskHandler([RawFilePath]()
                {
                    ::CoTaskMemFree(RawFilePath);
                });
                *this->m_ConfigurationFilePath = RawFilePath;
                if (!this->m_ConfigurationFilePath->empty())
                {
                    ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
                }
            }
            catch (...)
            {

            }
        }));
    }

    void QuickStartPage::CreateVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowVirtualMachineSettingsWindow(this->m_WindowHandle);
    }

    void QuickStartPage::VirtualMachineSettingsButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
            try
            {
                std::wstring FileTypeName = Mile::FormatWideString(
                    L"%s (*.7b)",
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationFileTypeName",
                        L"[NanaBox Virtual Machine Configuration]").c_str());

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    {
                        FileTypeName.c_str(),
                        L"*.7b"
                    }
                };
                LPWSTR RawFilePath = nullptr;
                winrt::check_hresult(::ShowFileDialog(
                    this->m_WindowHandle,
                    FALSE,
                    ARRAYSIZE(SupportedFileTypes),
                    SupportedFileTypes,
                    nullptr,
                    nullptr,
                    nullptr,
                    &RawFilePath));
                auto FreeFilePath = Mile::ScopeExitTaskHandler([RawFilePath]()
                {
                    ::CoTaskMemFree(RawFilePath);
                });
                std::wstring ConfigurationFilePath(RawFilePath);
                if (!ConfigurationFilePath.empty())
                {
                    NanaBox::VirtualMachineConfiguration Configuration =
                        NanaBox::DeserializeConfiguration(
                            ::ReadAllTextFromUtf8TextFile(ConfigurationFilePath));
                    ::ShowVirtualMachineSettingsWindow(
                        this->m_WindowHandle,
                        ConfigurationFilePath,
                        Mile::ToWideString(CP_UTF8, Configuration.Name));
                }
            }
            catch (...)
            {

            }
        }));
    }

    void QuickStartPage::CreateVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowNewVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::ResizeVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowResizeVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::CompactVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowCompactVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::AboutButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowAboutDialog(this->m_WindowHandle);
    }
}
