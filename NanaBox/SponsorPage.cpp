#include "pch.h"
#include "SponsorPage.h"
#if __has_include("SponsorPage.g.cpp")
#include "SponsorPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

#include <ShObjIdl_core.h>

#include <winrt/Windows.Services.Store.h>

namespace winrt
{
    using Windows::Services::Store::StoreContext;
    using Windows::Services::Store::StoreProduct;
    using Windows::Services::Store::StoreProductQueryResult;
}

namespace winrt::NanaBox::implementation
{
    SponsorPage::SponsorPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"SponsorPage/GridTitleTextBlock/Text").c_str());

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

    void SponsorPage::InitializeComponent()
    {
        SponsorPageT::InitializeComponent();
    }

    void SponsorPage::ContributeButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = {};
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile =
            L"https://github.com/M2Team/NanaBox/"
            L"blob/main/CONTRIBUTING.md";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }

    winrt::fire_and_forget SponsorPage::BuySponsorEditionButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

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

                co_await Product.RequestPurchaseAsync();
            }
        }
    }

    void SponsorPage::SponsorEditionPolicyButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        SHELLEXECUTEINFOW ExecInfo = {};
        ExecInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        ExecInfo.lpVerb = L"open";
        ExecInfo.lpFile =
            L"https://github.com/M2Team/NanaBox/"
            L"blob/main/Documents/SponsorEdition.md";
        ExecInfo.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&ExecInfo);
    }
}
