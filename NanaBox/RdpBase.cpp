/*
 * PROJECT:    NanaBox
 * FILE:       RdpBase.h
 * PURPOSE:    Implementation for the Remote Desktop Base API Wrapper
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "RdpBase.h"

namespace
{
    static HMODULE GetRdpBaseModuleHandle()
    {
        static HMODULE CachedResult = ::LoadLibraryExW(
            L"RdpBase.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
        return CachedResult;
    }

    static FARPROC GetRDPBASECreateInstanceProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetRdpBaseModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "RDPBASE_CreateInstance");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static HMODULE GetAtlModuleHandle()
    {
        static HMODULE CachedResult = ::LoadLibraryExW(
            L"atl.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);
        return CachedResult;
    }

    static FARPROC GetAtlAxWinInitProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetAtlModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "AtlAxWinInit");
            }
            return nullptr;
        }());

        return CachedResult;
    }

    static FARPROC GetAtlAxAttachControlProcAddress()
    {
        static FARPROC CachedResult = ([]() -> FARPROC
        {
            HMODULE ModuleHandle = ::GetAtlModuleHandle();
            if (ModuleHandle)
            {
                return ::GetProcAddress(
                    ModuleHandle,
                    "AtlAxAttachControl");
            }
            return nullptr;
        }());
        return CachedResult;
    }
}

EXTERN_C HRESULT WINAPI RDPBASE_CreateInstance(
    _In_ IUnknown* pPlatformContext,
    _In_ REFCLSID rclsid,
    _In_ REFIID riid,
    _Out_ LPVOID* ppvObject)
{
    using ProcType = decltype(::RDPBASE_CreateInstance)*;
    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetRDPBASECreateInstanceProcAddress());
    if (!ProcAddress)
    {
        return E_NOINTERFACE;
    }

    return ProcAddress(pPlatformContext, rclsid, riid, ppvObject);
}

EXTERN_C BOOL WINAPI SystemAtlAxWinInit()
{
    using ProcType = decltype(::SystemAtlAxWinInit)*;
    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetAtlAxWinInitProcAddress());
    if (!ProcAddress)
    {
        return FALSE;
    }

    return ProcAddress();
}

EXTERN_C HRESULT WINAPI SystemAtlAxAttachControl(
    _Inout_ IUnknown* pControl,
    _In_ HWND hWnd,
    _Out_opt_ IUnknown** ppUnkContainer)
{
    using ProcType = decltype(::SystemAtlAxAttachControl)*;
    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetAtlAxAttachControlProcAddress());
    if (!ProcAddress)
    {
        return E_NOINTERFACE;
    }

    return ProcAddress(pControl, hWnd, ppUnkContainer);
}
