/*
 * PROJECT:   NanaBox
 * FILE:      NanaBoxShellExtension.cpp
 * PURPOSE:   Implementation for NanaBox Shell Extension
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Windows.h>

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <shobjidl_core.h>

#include <winrt/Windows.Foundation.h>

struct DECLSPEC_UUID("9466E986-013C-420E-876B-42E4744E9893")
    ShellExtensionFactory : public winrt::implements<
        ShellExtensionFactory,
        IClassFactory>
{
public:

    HRESULT STDMETHODCALLTYPE CreateInstance(
        _In_opt_ IUnknown * pUnkOuter,
        _In_ REFIID riid,
        _COM_Outptr_ void** ppvObject) noexcept override
    {
        UNREFERENCED_PARAMETER(pUnkOuter);

        try
        {
            UNREFERENCED_PARAMETER(riid);
            UNREFERENCED_PARAMETER(ppvObject);
            return E_NOTIMPL;
        }
        catch (...)
        {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(
        _In_ BOOL fLock) noexcept override
    {
        if (fLock)
        {
            ++winrt::get_module_lock();
        }
        else
        {
            --winrt::get_module_lock();
        }

        return S_OK;
    }
};

EXTERN_C HRESULT STDAPICALLTYPE DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }

    winrt::clear_factory_cache();

    return S_OK;
}

EXTERN_C HRESULT STDAPICALLTYPE DllGetClassObject(
    _In_ REFCLSID rclsid,
    _In_ REFIID riid,
    _Outptr_ LPVOID* ppv)
{
    if (!ppv)
    {
        return E_POINTER;
    }

    if (riid != IID_IClassFactory && riid != IID_IUnknown)
    {
        return E_NOINTERFACE;
    }

    if (rclsid != __uuidof(ShellExtensionFactory))
    {
        return E_INVALIDARG;
    }

    try
    {
        return winrt::make<ShellExtensionFactory>()->QueryInterface(riid, ppv);
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
}

HINSTANCE g_hInstance = nullptr;

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD fdwReason,
    _In_ LPVOID lpvReserved)
{
    UNREFERENCED_PARAMETER(lpvReserved);

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hInstance = hinstDLL;
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
