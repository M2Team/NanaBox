/*
 * PROJECT:   NanaBox
 * FILE:      NanaBox.cpp
 * PURPOSE:   Implementation for NanaBox
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "RdpClient.h"

#include <cwchar>

//#include <atlbase.h>
//#include <atlhost.h>

namespace
{
    struct OleClientSite : public winrt::implements<
        OleClientSite,
        IOleClientSite,
        IOleInPlaceSite>
    {
    private:

        HWND m_WindowHandle;

    public:

        OleClientSite(_In_ HWND WindowHandle = nullptr)
            : m_WindowHandle(WindowHandle)
        {

        }

        HRESULT STDMETHODCALLTYPE SaveObject()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE GetMoniker(
            _In_ DWORD dwAssign,
            _In_ DWORD dwWhichMoniker,
            _Out_ IMoniker** ppmk)
        {
            UNREFERENCED_PARAMETER(dwAssign);
            UNREFERENCED_PARAMETER(dwWhichMoniker);
            *ppmk = nullptr;
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE GetContainer(
            _Out_ IOleContainer** ppContainer)
        {
            *ppContainer = nullptr;
            return E_NOTIMPL;
        }

        HRESULT STDMETHODCALLTYPE ShowObject()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnShowWindow(
            _In_ BOOL fShow)
        {
            UNREFERENCED_PARAMETER(fShow);
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE RequestNewObjectLayout()
        {
            return E_NOTIMPL;
        }

    public:

        HRESULT STDMETHODCALLTYPE GetWindow(
            _Out_ HWND* phwnd)
        {
            *phwnd = this->m_WindowHandle;
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
            _In_ BOOL fEnterMode)
        {
            UNREFERENCED_PARAMETER(fEnterMode);
            return S_OK;
        }

    public:

        HRESULT STDMETHODCALLTYPE CanInPlaceActivate()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnInPlaceActivate()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnUIActivate()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE GetWindowContext(
            _Out_ IOleInPlaceFrame** ppFrame,
            _Out_ IOleInPlaceUIWindow** ppDoc,
            _Out_ LPRECT lprcPosRect,
            _Out_ LPRECT lprcClipRect,
            _Inout_ LPOLEINPLACEFRAMEINFO lpFrameInfo)
        {
            *ppFrame = nullptr;
            *ppDoc = nullptr;

            RECT ClientRect;
            if (::GetClientRect(this->m_WindowHandle, &ClientRect))
            {
                int Width = ClientRect.right - ClientRect.left;
                int Height = ClientRect.bottom - ClientRect.top;       
                ::SetRect(lprcPosRect, 0, 0, Width, Height);
                ::SetRect(lprcClipRect, 0, 0, Width, Height);
            }

            lpFrameInfo->haccel = nullptr;

            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE Scroll(
            _In_ SIZE scrollExtant)
        {
            UNREFERENCED_PARAMETER(scrollExtant);
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnUIDeactivate(
            _In_ BOOL fUndoable)
        {
            UNREFERENCED_PARAMETER(fUndoable);
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DiscardUndoState()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DeactivateAndUndo()
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnPosRectChange(
            _In_ LPCRECT lprcPosRect)
        {
            UNREFERENCED_PARAMETER(lprcPosRect);
            return S_OK;
        }
    };

    winrt::com_ptr<NanaBox::RdpClient> g_RdpClient;

    winrt::com_ptr<IOleClientSite> g_OleClientSite; 

    static LRESULT CALLBACK NanaBoxMainWindowCallback(
        _In_ HWND   hWnd,
        _In_ UINT   uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CREATE:
        {
            // Note: Return -1 directly because WM_DESTROY message will be sent
            // when destroy the window automatically. We free the resource when
            // processing the WM_DESTROY message of this window.

            /*LPCREATESTRUCT CreateStruct =
                reinterpret_cast<LPCREATESTRUCT>(lParam);*/

            /*WNDCLASSEXW WindowClass;
            WindowClass.cbSize = sizeof(WNDCLASSEXW);
            WindowClass.style = 0;
            WindowClass.lpfnWndProc = ::DefWindowProcW;
            WindowClass.cbClsExtra = 0;
            WindowClass.cbWndExtra = 0;
            WindowClass.hInstance = ::GetModuleHandleW(nullptr);
            WindowClass.hIcon = nullptr;
            WindowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
            WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(
                ::GetStockObject(BLACK_BRUSH));
            WindowClass.lpszMenuName = nullptr;
            WindowClass.lpszClassName = L"NanaBoxHostWindow";
            WindowClass.hIconSm = nullptr;
            if (!::RegisterClassExW(&WindowClass))
            {
                return -1;
            }

            RECT ParentClientRect;
            winrt::check_bool(::GetClientRect(hWnd, &ParentClientRect));

            HWND WindowHandle = ::CreateWindowExW(
                0,
                WindowClass.lpszClassName,
                L"NanaBoxHost",
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                ParentClientRect.right - ParentClientRect.left,
                ParentClientRect.bottom - ParentClientRect.top,
                hWnd,
                nullptr,
                WindowClass.hInstance,
                nullptr);
            if (!WindowHandle)
            {
                return -1;
            }*/

            g_RdpClient = winrt::make_self<NanaBox::RdpClient>();

            RECT ClientRect;
            winrt::check_bool(::GetClientRect(hWnd, &ClientRect));

            winrt::com_ptr<IOleObject> OleObject =
                g_RdpClient->RawClient().as<IOleObject>();

            winrt::com_ptr<IOleInPlaceActiveObject> OleInPlaceActiveObject =
                g_RdpClient->RawClient().as<IOleInPlaceActiveObject>();

            winrt::com_ptr<IOleInPlaceObject> OleInPlaceObject =
                g_RdpClient->RawClient().as<IOleInPlaceObject>();

            g_OleClientSite =
                winrt::make<OleClientSite>(hWnd);

            winrt::check_hresult(OleObject->SetClientSite(
                g_OleClientSite.get()));

            winrt::check_hresult(OleObject->DoVerb(
                OLEIVERB_PRIMARY,
                nullptr,
                g_OleClientSite.get(),
                0,
                hWnd,
                &ClientRect));

            g_RdpClient->Server(L"localhost");
            g_RdpClient->RDPPort(2179);
            g_RdpClient->AuthenticationServiceClass(L"Microsoft Virtual Console Service");
            g_RdpClient->AuthenticationLevel(0);
            g_RdpClient->EnableCredSspSupport(true);
            g_RdpClient->NegotiateSecurityLayer(false);

            VARIANT Value;
            Value.vt = VT_BOOL;
            Value.boolVal = VARIANT_TRUE;
            g_RdpClient->Property(L"DisableCredentialsDelegation", Value);

            g_RdpClient->PCB(L"48781dff-90cc-4650-89c3-fe12e6210b19");

            g_RdpClient->Connect();

            return 0;
        }
        case WM_SETFOCUS:
        {
            break;
        }
        case WM_SIZE:
        {
            break;
        }
        case WM_DPICHANGED:
        {
            LPRECT NewWindowRectangle = reinterpret_cast<LPRECT>(lParam);

            ::SetWindowPos(
                hWnd,
                nullptr,
                NewWindowRectangle->left,
                NewWindowRectangle->top,
                NewWindowRectangle->right - NewWindowRectangle->left,
                NewWindowRectangle->bottom - NewWindowRectangle->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        case WM_MENUCHAR:
        {
            // Reference: https://github.com/microsoft/terminal
            //            /blob/756fd444b1d443320cf2ed6887d4b65aa67a9a03
            //            /scratch/ScratchIslandApp
            //            /WindowExe/SampleIslandWindow.cpp#L155
            // Return this LRESULT here to prevent the app from making a bell
            // when alt+key is pressed. A menu is active and the user presses a
            // key that does not correspond to any mnemonic or accelerator key.

            return MAKELRESULT(0, MNC_CLOSE);
        }
        case WM_SETTINGCHANGE:
        {
            if (lParam && 0 == std::wcscmp(
                reinterpret_cast<LPWSTR>(lParam),
                L"ImmersiveColorSet"))
            {
                
            }

            break;
        }
        case WM_DESTROY:
        {
            winrt::com_ptr<IOleObject> OleObject =
                g_RdpClient->RawClient().as<IOleObject>();

            OleObject->Close(OLECLOSE_NOSAVE);
            OleObject->SetClientSite(nullptr);

            ::PostQuitMessage(0);

            break;
        }
        default:
            return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);

    WNDCLASSEXW WindowClass;
    WindowClass.cbSize = sizeof(WNDCLASSEXW);
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = ::NanaBoxMainWindowCallback;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = hInstance;
    WindowClass.hIcon = nullptr;
    WindowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(
        ::GetStockObject(BLACK_BRUSH));
    WindowClass.lpszMenuName = nullptr;
    WindowClass.lpszClassName = L"NanaBoxMainWindow";
    WindowClass.hIconSm = nullptr;
    if (!::RegisterClassExW(&WindowClass))
    {
        return -1;
    }

    HWND WindowHandle = ::CreateWindowExW(
        0,
        WindowClass.lpszClassName,
        L"NanaBox",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        0,
        CW_USEDEFAULT,
        0,
        nullptr,
        nullptr,
        hInstance,
        nullptr);
    if (!WindowHandle)
    {
        return -1;
    }

    ::ShowWindow(WindowHandle, nShowCmd);
    ::UpdateWindow(WindowHandle);

    MSG Message;
    while (::GetMessageW(&Message, nullptr, 0, 0))
    {
        ::TranslateMessage(&Message);
        ::DispatchMessageW(&Message);
    }

    return static_cast<int>(Message.wParam);
}
