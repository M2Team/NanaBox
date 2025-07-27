/*
 * PROJECT:    NanaBox
 * FILE:       RdpBase.h
 * PURPOSE:    Definition for the Remote Desktop Base API
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef RDPBASE_H
#define RDPBASE_H

#ifndef __cplusplus
#error "[RdpBase] You should use a C++ compiler."
#endif // !__cplusplus

#include <comdef.h>

MIDL_INTERFACE("D923EFE9-0A6D-4344-92B3-164229DB8D2D")
IRDPENCNamedPipeDirectConnectorCallbacks : IUnknown
{
    virtual void STDMETHODCALLTYPE OnConnectionCompleted(
        _In_ IUnknown* pNetStream) = 0;
    virtual void STDMETHODCALLTYPE OnConnectorError(
        _In_ HRESULT hrError) = 0;
};

MIDL_INTERFACE("4ACF942D-EADC-45BF-8EA8-793FE3CE31E8")
IRDPENCNamedPipeDirectConnector : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE InitializeInstance(
        _In_ IRDPENCNamedPipeDirectConnectorCallbacks* pCallback) = 0;
    virtual HRESULT STDMETHODCALLTYPE TerminateInstance() = 0;
    virtual HRESULT STDMETHODCALLTYPE StartConnect(
        _In_ LPCWSTR szServerPipeName) = 0;
};

MIDL_INTERFACE("FB332AE7-000E-4208-92B7-20410CA8382B")
IRDPENCPlatformContext : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE GetProperty(
        _In_ LPCWSTR pwszName,
        _Out_ LPVARIANT pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyBool(
        _In_ LPCWSTR pwszName,
        _Out_ PBOOL pfValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyUnsignedLong(
        _In_ LPCWSTR pwszName,
        _Out_ PUINT pulValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPropertyInterface(
        _In_ LPCWSTR pwszName,
        _In_ REFIID riid,
        _Out_ LPVOID* ppInstance) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetProperty(
        _In_ LPCWSTR pwszName,
        _In_ LPVARIANT pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyBool(
        _In_ LPCWSTR pwszName,
        _In_ BOOL fValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyUnsignedLong(
        _In_ LPCWSTR pwszName,
        _In_ UINT ulValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyInterface(
        _In_ LPCWSTR pwszName,
        _In_ IUnknown* pInstance) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetPropertyIfNotPresent(
        _In_ LPCWSTR pwszName,
        _In_ LPVARIANT pValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE InitializeInstance() = 0;

    virtual HRESULT STDMETHODCALLTYPE TerminateInstance() = 0;
};

EXTERN_GUID(
    CLSID_RDPENCNamedPipeDirectConnector,
    0xFB332AE7, 0x0088, 0x4208, 0x92, 0xB7, 0x20, 0x41, 0x0C, 0xA8, 0x38, 0x2B);
EXTERN_GUID(
    IID_IRDPENCNamedPipeDirectConnector,
    0x4ACF942D, 0xEADC, 0x45BF, 0x8E, 0xA8, 0x79, 0x3F, 0xE3, 0xCE, 0x31, 0xE8);
EXTERN_GUID(
    CLSID_RDPRuntimeSTAContext,
    0xFB332AE7, 0x0055, 0x4208, 0x92, 0xB7, 0x20, 0x41, 0x0C, 0xA8, 0x38, 0x2B);
EXTERN_GUID(
    IID_IRDPENCPlatformContext,
    0xFB332AE7, 0x000E, 0x4208, 0x92, 0xB7, 0x20, 0x41, 0x0C, 0xA8, 0x38, 0x2B);

EXTERN_C HRESULT WINAPI RDPBASE_CreateInstance(
    _In_ IUnknown* pPlatformContext,
    _In_ REFCLSID rclsid,
    _In_ REFIID riid,
    _Out_ LPVOID* ppvObject);

#endif // !RDPBASE_H
