/*
 * PROJECT:   NanaBox
 * FILE:      HostCompute.h
 * PURPOSE:   Definition for the Host Compute API C++/WinRT wrapper
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#ifndef NANABOX_HOSTCOMPUTE
#define NANABOX_HOSTCOMPUTE

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[HostCompute] You should use a C++ compiler with the C++17 standard."
#endif

#include <Windows.h>
#include <computecore.h>
#include <computenetwork.h>

#include <winrt/Windows.Foundation.h>

namespace NanaBox
{
    struct HcsOperationTraits
    {
        using type = HCS_OPERATION;

        static void close(type value) noexcept
        {
            ::HcsCloseOperation(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    using HcsOperation = winrt::handle_type<HcsOperationTraits>;

    struct HcsSystemTraits
    {
        using type = HCS_SYSTEM;

        static void close(type value) noexcept
        {
            ::HcsCloseComputeSystem(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    using HcsSystem = winrt::handle_type<HcsSystemTraits>;

    struct ComputeSystem
    {
    public:

        ComputeSystem(
            winrt::hstring const& Id,
            winrt::hstring const& Configuration);

        ComputeSystem(
            winrt::hstring const& Id);

        void Start();

        void Shutdown();

        void Terminate();

        void Pause(
            winrt::hstring const& Options = winrt::hstring());

        void Resume();

        void Save(
            winrt::hstring const& Options);

        winrt::hstring GetProperties(
            winrt::hstring const& PropertyQuery = winrt::hstring());

        void Modify(
            winrt::hstring const& Configuration);

        winrt::event_token SystemExited(
            winrt::delegate<winrt::hstring> const& Handler);

        void SystemExited(
            winrt::event_token const& Token);

        winrt::event_token SystemRdpEnhancedModeStateChanged(
            winrt::delegate<> const& Handler);

        void SystemRdpEnhancedModeStateChanged(
            winrt::event_token const& Token);

    private:

        HcsOperation m_Operation;
        HcsSystem m_ComputeSystem;

        static void CALLBACK ComputeSystemCallback(
            HCS_EVENT* Event,
            void* Context);

        winrt::event<winrt::delegate<winrt::hstring>> m_SystemExited;
        winrt::event<winrt::delegate<>> m_SystemRdpEnhancedModeStateChanged;
    };
}

namespace winrt::NanaBox
{
    using namespace ::NanaBox;
}

#endif // !NANABOX_HOSTCOMPUTE
