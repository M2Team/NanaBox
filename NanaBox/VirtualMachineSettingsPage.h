#pragma once

#include "VirtualMachineSettingsPage.g.h"

#include <Windows.h>

#include <string>

namespace NanaBox
{
    class MainWindow;
}

namespace winrt::NanaBox
{
    using ::NanaBox::MainWindow;
}

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct VirtualMachineSettingsPage :
        VirtualMachineSettingsPageT<VirtualMachineSettingsPage>
    {
    public:

        VirtualMachineSettingsPage(
            _In_ HWND WindowHandle = nullptr,
            std::wstring const& ConfigurationFilePath = std::wstring(),
            std::wstring const& Name = std::wstring(),
            _In_opt_ NanaBox::MainWindow* MainWindow = nullptr);

        void InitializeComponent();

        void OpenButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void ReferenceButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void ApplyButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CreateButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

    private:

        HWND m_WindowHandle;
        std::wstring m_ConfigurationFilePath;
        NanaBox::MainWindow* m_MainWindow;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct VirtualMachineSettingsPage : VirtualMachineSettingsPageT<
        VirtualMachineSettingsPage,
        implementation::VirtualMachineSettingsPage>
    {
    };
}
