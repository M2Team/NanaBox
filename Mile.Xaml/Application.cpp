/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Application.cpp
 * PURPOSE:   Implementation for the Mile.Xaml.Application
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"

#include "Application.h"
#include "Application.g.cpp"

namespace winrt::Mile::Xaml::implementation
{
    Application::Application(winrt::XamlMetadataProviders const& Providers)
    {
        for(auto const& Provider : Providers)
        {
            this->m_Providers.Append(Provider);
        }

        this->Initialize();
    }

    Application::Application()
    {
    }

    void Application::Initialize()
    {
        const auto out = outer();
        if (out)
        {
            winrt::IXamlMetadataProvider provider(nullptr);
            winrt::check_hresult(out->QueryInterface(
                winrt::guid_of<winrt::IXamlMetadataProvider>(),
                reinterpret_cast<void**>(winrt::put_abi(provider))));
            this->m_Providers.Append(provider);
        }

        this->m_WindowsXamlManager =
            winrt::WindowsXamlManager::InitializeForCurrentThread();
    }

    void Application::Close()
    {
        if (this->m_IsClosed)
        {
            return;
        }

        this->m_IsClosed = true;

        this->m_WindowsXamlManager.Close();
        this->m_Providers.Clear();
        this->m_WindowsXamlManager = nullptr;

        this->Exit();

        {
            MSG Message;
            while (::PeekMessageW(&Message, nullptr, 0, 0, PM_REMOVE))
            {
                ::DispatchMessageW(&Message);
            }
        }
    }

    Application::~Application()
    {
        this->Close();
    }

    winrt::IXamlType Application::GetXamlType(winrt::TypeName const& type)
    {
        for (const auto& Provider : this->m_Providers)
        {
            const auto Current = Provider.GetXamlType(type);
            if (Current != nullptr)
            {
                return Current;
            }
        }

        return nullptr;
    }

    winrt::IXamlType Application::GetXamlType(winrt::hstring const& fullName)
    {
        for (const auto& Provider : this->m_Providers)
        {
            const auto Current = Provider.GetXamlType(fullName);
            if (Current != nullptr)
            {
                return Current;
            }
        }

        return nullptr;
    }

    winrt::XmlnsDefinitions Application::GetXmlnsDefinitions()
    {
        std::list<winrt::XmlnsDefinition> Definitions;

        for (const auto& Provider : this->m_Providers)
        {
            for (const auto& Definition : Provider.GetXmlnsDefinitions())
            {
                Definitions.insert(Definitions.begin(), Definition);
            }
        }

        return winrt::XmlnsDefinitions(Definitions.begin(), Definitions.end());
    }

    winrt::XamlMetadataProviders Application::MetadataProviders()
    {
        return this->m_Providers;
    }
}

namespace winrt::Mile::Xaml::factory_implementation
{
    Application::Application()
    {
        // Workaround a bug where twinapi.appcore.dll and threadpoolwinrt.dll
        // gets loaded after it has been unloaded because of a call to
        // GetActivationFactory.
        LPCWSTR PreloadModuleNames[] =
        {
            L"twinapi.appcore.dll",
            L"threadpoolwinrt.dll",
        };
        const size_t PreloadModuleNamesCount =
            sizeof(PreloadModuleNames) / sizeof(*PreloadModuleNames);
        for (size_t i = 0; i < PreloadModuleNamesCount; ++i)
        {
            this->m_PreloadModules.push_back(
                ::LoadLibraryExW(
                    PreloadModuleNames[i],
                    nullptr,
                    LOAD_LIBRARY_SEARCH_SYSTEM32));
        }
    }

    Application::~Application()
    {
        for (auto const& PreloadModules : this->m_PreloadModules)
        {
            ::FreeLibrary(PreloadModules);
        }
        this->m_PreloadModules.clear();
    }
}
