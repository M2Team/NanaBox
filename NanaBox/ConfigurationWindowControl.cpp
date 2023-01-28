#include "pch.h"
#include "ConfigurationWindowControl.h"
#if __has_include("ConfigurationWindowControl.g.cpp")
#include "ConfigurationWindowControl.g.cpp"
#endif

namespace winrt
{
    using Windows::Foundation::IStringable;
    using Windows::UI::Xaml::Controls::NavigationView;
    using Windows::UI::Xaml::Controls::NavigationViewItem;
    using Windows::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs;
    using Windows::UI::Xaml::RoutedEventArgs;

    using Windows::UI::Xaml::Interop::TypeName;
    using Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo;
    using Windows::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo;
}

namespace winrt::NanaBox::implementation
{
    ConfigurationWindowControl::ConfigurationWindowControl()
    {
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"basic",
            winrt::xaml_typename<NanaBox::BasicConfigurationPage>()));
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"system",
            winrt::xaml_typename<NanaBox::SystemConfigurationPage>()));
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"graphics",
            winrt::xaml_typename<NanaBox::GraphicsConfigurationPage>()));
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"network",
            winrt::xaml_typename<NanaBox::NetworkConfigurationPage>()));
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"storage",
            winrt::xaml_typename<NanaBox::StorageConfigurationPage>()));
        m_pages.push_back(std::make_pair<std::wstring, winrt::TypeName>(
            L"serial",
            winrt::xaml_typename<NanaBox::SerialPortConfigurationPage>()));
    }


    void ConfigurationWindowControl::NavigationView_Loaded(
        IInspectable const& sender, RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        NavigationView().SelectedItem(NavigationView().MenuItems().GetAt(0));
        NavigationView_Navigate(L"basic", EntranceNavigationTransitionInfo());
    }

    void ConfigurationWindowControl::NavigationView_Navigate(
        std::wstring navItemTag,
        NavigationTransitionInfo const& transitionInfo)
    {
        TypeName currPageName = ContentFrame().CurrentSourcePageType();
        TypeName targetPageName;

        for (auto&& page : m_pages)
        {
            if (page.first == navItemTag)
            {
                targetPageName = page.second;
            }
        }

        if (targetPageName.Name != L"" && targetPageName.Name != currPageName.Name)
        {
            ContentFrame().Navigate(targetPageName, nullptr, transitionInfo);
        }
    }

    void ConfigurationWindowControl::NavigationView_SelectionChanged(
        IInspectable const& sender,
        NavigationViewSelectionChangedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);

        if (args.SelectedItemContainer())
        {
            auto tag = winrt::unbox_value_or<winrt::hstring>(
                args.SelectedItemContainer().Tag(), L"");
            NavigationView_Navigate(
                tag.c_str(), args.RecommendedNavigationTransitionInfo());
        }
    }

}

