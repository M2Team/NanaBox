#pragma once

#include "NewVirtualHardDiskPage.g.h"

#include <winrt/Windows.System.h>

#include <Windows.h>

namespace winrt
{
    using Windows::Foundation::IInspectable;
    using Windows::System::DispatcherQueue;
    using Windows::UI::Xaml::Controls::TextBox;
    using Windows::UI::Xaml::Controls::TextBoxBeforeTextChangingEventArgs;
    using Windows::UI::Xaml::RoutedEventArgs;
}

namespace winrt::NanaBox::implementation
{
    struct NewVirtualHardDiskPage :
        NewVirtualHardDiskPageT<NewVirtualHardDiskPage>
    {
    public:

        NewVirtualHardDiskPage(
            _In_ HWND WindowHandle = nullptr);

        void InitializeComponent();

        void NaturalNumberTextBoxBeforeTextChanging(
            winrt::TextBox const& sender,
            winrt::TextBoxBeforeTextChangingEventArgs const& args);

        void FileNameBrowseButtonClickHandler(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CreateButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

        void CancelButtonClick(
            winrt::IInspectable const& sender,
            winrt::RoutedEventArgs const& e);

    private:

        HWND m_WindowHandle;
        winrt::DispatcherQueue m_DispatcherQueue = nullptr;
    };
}

namespace winrt::NanaBox::factory_implementation
{
    struct NewVirtualHardDiskPage : NewVirtualHardDiskPageT<
        NewVirtualHardDiskPage, implementation::NewVirtualHardDiskPage>
    {
    };
}
