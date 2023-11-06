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

namespace
{
    HINSTANCE g_hInstance = nullptr;

    std::wstring GetNanaBoxPackageFolderPath()
    {
        // 32767 is the maximum path length without the terminating null
        // character.
        std::wstring Path(32767, L'\0');
        Path.resize(::GetModuleFileNameW(
            g_hInstance, &Path[0], static_cast<DWORD>(Path.size())));
        std::wcsrchr(&Path[0], L'\\')[0] = L'\0';
        Path.resize(std::wcslen(Path.c_str()));
        return Path;
    }
}

struct ExplorerCommandBase : public winrt::implements<
    ExplorerCommandBase,
    IExplorerCommand>
{
private:

    std::wstring m_Title;

    bool m_IsSeparator;

public:

    ExplorerCommandBase(
        std::wstring const& Title = std::wstring(),
        bool IsSeparator = false) :
        m_Title(Title),
        m_IsSeparator(IsSeparator)
    {

    }

#pragma region IExplorerCommand

    HRESULT STDMETHODCALLTYPE GetTitle(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszName)
    {
        UNREFERENCED_PARAMETER(psiItemArray);

        if (this->m_IsSeparator)
        {
            *ppszName = nullptr;
            return S_FALSE;
        }

        return ::SHStrDupW(this->m_Title.c_str(), ppszName);
    }

    HRESULT STDMETHODCALLTYPE GetIcon(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszIcon)
    {
        UNREFERENCED_PARAMETER(psiItemArray);

        *ppszIcon = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetToolTip(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszInfotip)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        *ppszInfotip = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetCanonicalName(
        _Out_ GUID* pguidCommandName)
    {
        *pguidCommandName = GUID_NULL;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetState(
        _In_opt_ IShellItemArray* psiItemArray,
        _In_ BOOL fOkToBeSlow,
        _Out_ EXPCMDSTATE* pCmdState)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        UNREFERENCED_PARAMETER(fOkToBeSlow);
        *pCmdState = ECS_ENABLED;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Invoke(
        _In_opt_ IShellItemArray* psiItemArray,
        _In_opt_ IBindCtx* pbc)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        UNREFERENCED_PARAMETER(pbc);

        if (this->m_IsSeparator)
        {
            return E_NOTIMPL;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetFlags(
        _Out_ EXPCMDFLAGS* pFlags)
    {
        *pFlags =
            this->m_IsSeparator
            ? ECF_ISSEPARATOR
            : ECF_DEFAULT;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnumSubCommands(
        _Outptr_ IEnumExplorerCommand** ppEnum)
    {
        *ppEnum = nullptr;
        return E_NOTIMPL;
    }

#pragma endregion
};

using SubCommandList = std::vector<winrt::com_ptr<IExplorerCommand>>;
using SubCommandListIterator = SubCommandList::const_iterator;

struct ExplorerCommandRoot : public winrt::implements<
    ExplorerCommandRoot,
    IExplorerCommand,
    IEnumExplorerCommand>
{
private:

    bool m_Initialized = false;
    SubCommandList m_SubCommands;
    SubCommandListIterator m_CurrentSubCommand;

    void Initialize(
        _In_opt_ IShellItemArray* psiItemArray)
    {
        if (m_Initialized)
        {
            return;
        }

        m_Initialized = true;

        UNREFERENCED_PARAMETER(psiItemArray);

        this->m_SubCommands.push_back(
            winrt::make<ExplorerCommandBase>(
                L"Hello World"));
    }

public:

    ExplorerCommandRoot() = default;

#pragma region IExplorerCommand

    HRESULT STDMETHODCALLTYPE GetTitle(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszName)
    {
        this->Initialize(psiItemArray);

        if (this->m_SubCommands.empty())
        {
            *ppszName = nullptr;
            return E_NOTIMPL;
        }

        return ::SHStrDupW(L"NanaBox Preview", ppszName);
    }

    HRESULT STDMETHODCALLTYPE GetIcon(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszIcon)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        std::wstring Icon = GetNanaBoxPackageFolderPath() + L"\\NanaBox.exe";
        Icon += L",-101";
        return ::SHStrDupW(Icon.c_str(), ppszIcon);
    }

    HRESULT STDMETHODCALLTYPE GetToolTip(
        _In_opt_ IShellItemArray* psiItemArray,
        _Outptr_ LPWSTR* ppszInfotip)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        *ppszInfotip = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetCanonicalName(
        _Out_ GUID* pguidCommandName)
    {
        *pguidCommandName = GUID_NULL;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetState(
        _In_opt_ IShellItemArray* psiItemArray,
        _In_ BOOL fOkToBeSlow,
        _Out_ EXPCMDSTATE* pCmdState)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        UNREFERENCED_PARAMETER(fOkToBeSlow);
        *pCmdState = ECS_ENABLED;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Invoke(
        _In_opt_ IShellItemArray* psiItemArray,
        _In_opt_ IBindCtx* pbc)
    {
        UNREFERENCED_PARAMETER(psiItemArray);
        UNREFERENCED_PARAMETER(pbc);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetFlags(
        _Out_ EXPCMDFLAGS* pFlags)
    {
        *pFlags = ECF_HASSUBCOMMANDS;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnumSubCommands(
        _Outptr_ IEnumExplorerCommand** ppEnum)
    {
        if (this->m_SubCommands.empty())
        {
            *ppEnum = nullptr;
            return E_NOTIMPL;
        }
        else
        {
            this->m_CurrentSubCommand = this->m_SubCommands.cbegin();
            this->AddRef();
            return this->QueryInterface(IID_PPV_ARGS(ppEnum));
        }
    }

#pragma endregion

#pragma region IEnumExplorerCommand

    HRESULT STDMETHODCALLTYPE Next(
        _In_ ULONG celt,
        _Out_ IExplorerCommand** pUICommand,
        _Out_opt_ ULONG* pceltFetched)
    {
        ULONG Fetched = 0;

        for (
            ULONG i = 0;
            (i < celt) &&
            (this->m_CurrentSubCommand != this->m_SubCommands.cend());
            ++i)
        {
            this->m_CurrentSubCommand->copy_to(&pUICommand[0]);
            ++this->m_CurrentSubCommand;
            ++Fetched;
        }

        if (pceltFetched)
        {
            *pceltFetched = Fetched;
        }

        return (Fetched == celt) ? S_OK : S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE Skip(
        _In_ ULONG celt)
    {
        UNREFERENCED_PARAMETER(celt);
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Reset()
    {
        this->m_CurrentSubCommand = this->m_SubCommands.cbegin();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Clone(
        _Out_ IEnumExplorerCommand** ppenum)
    {
        *ppenum = nullptr;
        return E_NOTIMPL;
    }

#pragma endregion
};

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
            return winrt::make<ExplorerCommandRoot>()->QueryInterface(
                riid, ppvObject);
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
