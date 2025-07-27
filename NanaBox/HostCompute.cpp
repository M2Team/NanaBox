/*
 * PROJECT:    NanaBox
 * FILE:       HostCompute.cpp
 * PURPOSE:    Implementation for the Host Compute API C++/WinRT wrapper
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "HostCompute.h"

#pragma comment(lib, "computecore.lib")
#pragma comment(lib, "computenetwork.lib")

#include <Mile.Json.h>

namespace winrt
{
    struct hlocal_string_traits
    {
        using type = PWSTR;

        static void close(type value) noexcept
        {
            ::LocalFree(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    using hlocal_string = handle_type<hlocal_string_traits>;

    struct cotaskmem_string_traits
    {
        using type = PWSTR;

        static void close(type value) noexcept
        {
            ::CoTaskMemFree(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    using cotaskmem_string = handle_type<cotaskmem_string_traits>;
}

namespace
{
    winrt::hstring WaitForOperationResult(
        NanaBox::HcsOperation const& Operation)
    {
        winrt::hstring Result;

        winrt::hlocal_string RawResult;
        HRESULT hr = ::HcsWaitForOperationResult(
            Operation.get(),
            INFINITE,
            RawResult.put());
        if (RawResult)
        {
            Result = winrt::hstring(RawResult.get());
        }
        if (FAILED(hr))
        {
            winrt::hresult_error Exception;
            try
            {
                nlohmann::json ResultError = nlohmann::json::parse(
                    winrt::to_string(Result));
                Exception = winrt::hresult_error(
                    ResultError["Error"].get<std::int32_t>(),
                    winrt::to_hstring(
                        ResultError["ErrorMessage"].get<std::string>()));
            }
            catch (...)
            {
                Exception = winrt::hresult_error(hr, Result);
            }
            throw Exception;
        }

        return Result;
    }

    void CheckHcnCall(
        HRESULT RawErrorCode,
        winrt::cotaskmem_string const& RawErrorRecord)
    {
        winrt::hstring ErrorRecord;
        if (RawErrorRecord)
        {
            ErrorRecord = winrt::hstring(RawErrorRecord.get());
        }
        if (FAILED(RawErrorCode))
        {
            winrt::hresult_error Exception;
            try
            {
                nlohmann::json ResultError = nlohmann::json::parse(
                    winrt::to_string(ErrorRecord));
                Exception = winrt::hresult_error(
                    ResultError["ErrorCode"].get<std::int32_t>(),
                    winrt::to_hstring(
                        ResultError["Error"].get<std::string>()));
            }
            catch (...)
            {
                Exception = winrt::hresult_error(RawErrorCode, ErrorRecord);
            }
            throw Exception;
        }
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

void CALLBACK NanaBox::ComputeSystem::ComputeSystemCallback(
    HCS_EVENT* Event,
    void* Context)
{
    NanaBox::ComputeSystem* Object =
        reinterpret_cast<NanaBox::ComputeSystem*>(Context);

    switch (Event->Type)
    {
    case HcsEventSystemExited:
        Object->SystemExited(winrt::hstring(Event->EventData));
        break;
    case HcsEventSystemCrashInitiated:
        break;
    case HcsEventSystemCrashReport:
        break;
    case HcsEventSystemRdpEnhancedModeStateChanged:
        Object->SystemRdpEnhancedModeStateChanged();
        break;
    case HcsEventSystemGuestConnectionClosed:
        break;
    case HcsEventServiceDisconnect:
        break;
    default:
        break;
    }
}

winrt::hstring NanaBox::HcsGetServiceProperties(
    winrt::hstring const& PropertyQuery)
{
    winrt::hstring Result;

    winrt::hlocal_string RawResult;
    HRESULT hr = ::HcsGetServiceProperties(
        PropertyQuery.empty() ? nullptr : PropertyQuery.c_str(),
        RawResult.put());
    if (RawResult)
    {
        Result = winrt::hstring(RawResult.get());
    }
    if (FAILED(hr))
    {
        winrt::hresult_error Exception;
        try
        {
            nlohmann::json ResultError = nlohmann::json::parse(
                winrt::to_string(Result));
            Exception = winrt::hresult_error(
                ResultError["Error"].get<std::int32_t>(),
                winrt::to_hstring(
                    ResultError["ErrorMessage"].get<std::string>()));
        }
        catch (...)
        {
            Exception = winrt::hresult_error(hr, Result);
        }
        throw Exception;
    }

    return Result;
}

NanaBox::HcnNetwork NanaBox::HcnCreateNetwork(
    winrt::guid const& NetworkId,
    winrt::hstring const& Settings)
{
    NanaBox::HcnNetwork Result;
    winrt::cotaskmem_string RawErrorRecord;
    ::CheckHcnCall(
        ::HcnCreateNetwork(
            NetworkId,
            Settings.c_str(),
            Result.put(),
            RawErrorRecord.put()),
        RawErrorRecord);
    return Result;
}

NanaBox::HcnNetwork NanaBox::HcnOpenNetwork(
    winrt::guid const& NetworkId)
{
    NanaBox::HcnNetwork Result;

    winrt::cotaskmem_string RawErrorRecord;
    ::CheckHcnCall(
        ::HcnOpenNetwork(
            NetworkId,
            Result.put(),
            RawErrorRecord.put()),
        RawErrorRecord);

    return Result;
}

NanaBox::HcnEndpoint NanaBox::HcnCreateEndpoint(
    NanaBox::HcnNetwork const& NetworkHandle,
    winrt::guid const& EndpointId,
    winrt::hstring const& Settings)
{
    NanaBox::HcnEndpoint Result;

    winrt::cotaskmem_string RawErrorRecord;
    ::CheckHcnCall(
        ::HcnCreateEndpoint(
            NetworkHandle.get(),
            EndpointId,
            Settings.c_str(),
            Result.put(),
            RawErrorRecord.put()),
        RawErrorRecord);

    return Result;
}

void NanaBox::HcnDeleteEndpoint(
    winrt::guid const& EndpointId)
{
    winrt::cotaskmem_string RawErrorRecord;
    ::CheckHcnCall(
        ::HcnDeleteEndpoint(
            EndpointId,
            RawErrorRecord.put()),
        RawErrorRecord);
}

winrt::hstring NanaBox::HcnQueryEndpointProperties(
    NanaBox::HcnEndpoint const& EndpointHandle,
    winrt::hstring const& Query)
{
    winrt::hstring Result;

    winrt::cotaskmem_string RawResult;
    winrt::cotaskmem_string RawErrorRecord;
    ::CheckHcnCall(
        ::HcnQueryEndpointProperties(
            EndpointHandle.get(),
            Query.c_str(),
            RawResult.put(),
            RawErrorRecord.put()),
        RawErrorRecord);
    if (RawResult)
    {
        Result = winrt::hstring(RawResult.get());
    }

    return Result;
}
