/*
 * PROJECT:   NanaBox
 * FILE:      HostCompute.cpp
 * PURPOSE:   Implementation for the Host Compute API C++/WinRT wrapper
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "HostCompute.h"

#pragma comment(lib, "computecore.lib")
#pragma comment(lib, "computenetwork.lib")

namespace
{
    winrt::hstring WaitForOperationResult(
        NanaBox::HcsOperation const& Operation)
    {
        winrt::hstring Result;

        PWSTR RawResult = nullptr;
        HRESULT hr = ::HcsWaitForOperationResult(
            Operation.get(),
            INFINITE,
            &RawResult);
        if (RawResult)
        {
            Result = winrt::hstring(RawResult);
            ::LocalFree(RawResult);
        }
        if (FAILED(hr))
        {
            throw winrt::hresult_error(hr, Result);
        }

        return Result;
    }
}

NanaBox::ComputeSystem::ComputeSystem(
    winrt::hstring const& Id,
    winrt::hstring const& Configuration)
{
    this->m_Operation.attach(::HcsCreateOperation(
        nullptr,
        nullptr));
    winrt::check_pointer(
        this->m_Operation.get());

    winrt::check_hresult(::HcsCreateComputeSystem(
        Id.c_str(),
        Configuration.c_str(),
        this->m_Operation.get(),
        nullptr,
        this->m_ComputeSystem.put()));

    ::WaitForOperationResult(
        this->m_Operation);

    winrt::check_hresult(::HcsSetComputeSystemCallback(
        this->m_ComputeSystem.get(),
        HcsEventOptionNone,
        this,
        NanaBox::ComputeSystem::ComputeSystemCallback));
}

NanaBox::ComputeSystem::ComputeSystem(
    winrt::hstring const& Id)
{
    this->m_Operation.attach(::HcsCreateOperation(
        nullptr,
        nullptr));
    winrt::check_pointer(
        this->m_Operation.get());

    winrt::check_hresult(::HcsOpenComputeSystem(
        Id.c_str(),
        GENERIC_ALL,
        this->m_ComputeSystem.put()));

    ::WaitForOperationResult(
        this->m_Operation);

    winrt::check_hresult(::HcsSetComputeSystemCallback(
        this->m_ComputeSystem.get(),
        HcsEventOptionNone,
        this,
        NanaBox::ComputeSystem::ComputeSystemCallback));
}

void NanaBox::ComputeSystem::Start()
{
    winrt::check_hresult(::HcsStartComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        nullptr));

    ::WaitForOperationResult(
        this->m_Operation);
}

void NanaBox::ComputeSystem::Shutdown()
{
    winrt::check_hresult(::HcsShutDownComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        nullptr));

    ::WaitForOperationResult(
        this->m_Operation);
}

void NanaBox::ComputeSystem::Terminate()
{
    winrt::check_hresult(::HcsTerminateComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        nullptr));

    ::WaitForOperationResult(
        this->m_Operation);
}

//void NanaBox::ComputeSystem::Crash(
//    winrt::hstring const& Options)
//{
//    winrt::check_hresult(::HcsCrashComputeSystem(
//        this->m_ComputeSystem.get(),
//        this->m_Operation.get(),
//        Options.empty() ? nullptr : Options.c_str()));
//
//    ::WaitForOperationResult(
//        this->m_Operation);
//}

void NanaBox::ComputeSystem::Pause(
    winrt::hstring const& Options)
{
    winrt::check_hresult(::HcsPauseComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        Options.empty() ? nullptr : Options.c_str()));

    ::WaitForOperationResult(
        this->m_Operation);
}

void NanaBox::ComputeSystem::Resume()
{
    winrt::check_hresult(::HcsResumeComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        nullptr));

    ::WaitForOperationResult(
        this->m_Operation);
}

void NanaBox::ComputeSystem::Save(
    winrt::hstring const& Options)
{
    winrt::check_hresult(::HcsSaveComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        Options.empty() ? nullptr : Options.c_str()));

    ::WaitForOperationResult(
        this->m_Operation);
}

winrt::hstring NanaBox::ComputeSystem::GetProperties(
    winrt::hstring const& PropertyQuery)
{
    winrt::check_hresult(::HcsGetComputeSystemProperties(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        PropertyQuery.empty() ? nullptr : PropertyQuery.c_str()));

    return ::WaitForOperationResult(
        this->m_Operation);
}

void NanaBox::ComputeSystem::Modify(
    winrt::hstring const& Configuration)
{
    winrt::check_hresult(::HcsModifyComputeSystem(
        this->m_ComputeSystem.get(),
        this->m_Operation.get(),
        Configuration.c_str(),
        nullptr));

    ::WaitForOperationResult(
        this->m_Operation);
}

winrt::event_token NanaBox::ComputeSystem::SystemExited(
    winrt::delegate<winrt::hstring> const& Handler)
{
    return this->m_SystemExited.add(Handler);
}

void NanaBox::ComputeSystem::SystemExited(
    winrt::event_token const& Token)
{
    this->m_SystemExited.remove(Token);
}

winrt::event_token NanaBox::ComputeSystem::SystemRdpEnhancedModeStateChanged(
    winrt::delegate<> const& Handler)
{
    return this->m_SystemRdpEnhancedModeStateChanged.add(Handler);
}

void NanaBox::ComputeSystem::SystemRdpEnhancedModeStateChanged(
    winrt::event_token const& Token)
{
    this->m_SystemRdpEnhancedModeStateChanged.remove(Token);
}

void CALLBACK NanaBox::ComputeSystem::ComputeSystemCallback(
    HCS_EVENT* Event,
    void* Context)
{
    NanaBox::ComputeSystem* Object =
        reinterpret_cast<NanaBox::ComputeSystem*>(Context);

    switch (Event->Type)
    {
    case HcsEventSystemExited:
        Object->m_SystemExited(winrt::hstring(Event->EventData));
        break;
    case HcsEventSystemCrashInitiated:
        break;
    case HcsEventSystemCrashReport:
        break;
    case HcsEventSystemRdpEnhancedModeStateChanged:
        Object->m_SystemRdpEnhancedModeStateChanged();
        break;
    case HcsEventSystemGuestConnectionClosed:
        break;
    case HcsEventServiceDisconnect:
        break;
    default:
        break;
    }
}
