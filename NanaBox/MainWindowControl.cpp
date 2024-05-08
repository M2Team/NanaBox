#include "pch.h"
#include "MainWindowControl.h"
#if __has_include("MainWindowControl.g.cpp")
#include "MainWindowControl.g.cpp"
#endif

#include <ShObjIdl_core.h>

#include <winrt/Windows.Services.Store.h>

namespace winrt
{
    using Windows::Services::Store::StoreContext;
    using Windows::Services::Store::StoreProduct;
    using Windows::Services::Store::StoreProductQueryResult;
    using Windows::UI::Xaml::Controls::AppBarToggleButton;
}

namespace winrt::NanaBox::implementation
{
    MainWindowControl::MainWindowControl(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {

    }

    void MainWindowControl::InitializeComponent()
    {
        MainWindowControlT::InitializeComponent();
    }

    winrt::fire_and_forget MainWindowControl::PageLoaded(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        winrt::StoreContext Context = winrt::StoreContext::GetDefault();
        if (Context)
        {
            winrt::check_hresult(
                Context.as<IInitializeWithWindow>()->Initialize(
                    this->m_WindowHandle));

            winrt::StoreProductQueryResult ProductQueryResult =
                co_await Context.GetStoreProductsAsync(
                    { L"Durable" },
                    { L"9P3KMWM424WK" });
            for (auto Item : ProductQueryResult.Products())
            {
                winrt::StoreProduct Product = Item.Value();

                this->SponsorButton().Content(
                    winrt::box_value(Mile::WinRT::GetLocalizedString(
                        Product.IsInUserCollection()
                        ? L"MainWindow/SponsorButton/SponsoredText"
                        : L"MainWindow/SponsorButton/AcquireText")));
            }
        }
    }

    void MainWindowControl::EnhancedSessionButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            (winrt::unbox_value<bool>(
                sender.as<winrt::AppBarToggleButton>().IsChecked())
                ? MAKEWPARAM(
                    NanaBox::MainWindowCommands::EnterEnhancedSession,
                    BN_CLICKED)
                : MAKEWPARAM(
                    NanaBox::MainWindowCommands::EnterBasicSession,
                    BN_CLICKED)),
            0);
    }

    void MainWindowControl::FullScreenButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::EnterFullScreen,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::PauseVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            (winrt::unbox_value<bool>(
                sender.as<winrt::AppBarToggleButton>().IsChecked())
                ? MAKEWPARAM(
                    NanaBox::MainWindowCommands::PauseVirtualMachine,
                    BN_CLICKED)
                : MAKEWPARAM(
                    NanaBox::MainWindowCommands::ResumeVirtualMachine,
                    BN_CLICKED)),
            0);
    }

    void MainWindowControl::RestartVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::RestartVirtualMachine,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::VirtualMachineSettingsButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::VirtualMachineSettings,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::ReloadVirtualMachineSettingsButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::ReloadVirtualMachineSettings,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::CreateVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::CreateVirtualHardDisk,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::DocumentationButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::Documentation,
                BN_CLICKED),
            0);
    }

    void MainWindowControl::AboutButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::PostMessageW(
            this->m_WindowHandle,
            WM_COMMAND,
            MAKEWPARAM(
                NanaBox::MainWindowCommands::About,
                BN_CLICKED),
            0);
    }

    winrt::fire_and_forget MainWindowControl::SponsorButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        // Commented out for waiting some issue when using that under Admin.

        //winrt::StoreContext Context = winrt::StoreContext::GetDefault();
        //if (Context)
        //{
        //    winrt::check_hresult(
        //        Context.as<IInitializeWithWindow>()->Initialize(
        //            this->m_WindowHandle));

        //    winrt::StoreProductQueryResult ProductQueryResult =
        //        co_await Context.GetStoreProductsAsync(
        //            { L"Durable" },
        //            { L"9P3KMWM424WK" });
        //    for (auto Item : ProductQueryResult.Products())
        //    {
        //        winrt::StoreProduct Product = Item.Value();

        //        if (!Product.IsInUserCollection())
        //        {
        //            co_await Product.RequestPurchaseAsync();
        //        }

        //        // TODO: Refresh text for Sponsor button
        //    }
        //}
    }
}
