#pragma once

#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
#include <atlhost.h>
#include <atltypes.h>

#define _WTL_NO_AUTOMATIC_NAMESPACE
#include <atlapp.h>
#include <atlcrack.h>

#include "RdpClient.h"
#include "RdpBase.h"
#include "HostCompute.h"
#include "ConfigurationManager.h"

#include "MainWindowControl.h"

namespace NanaBox
{
    namespace MainWindowTimerEvents
    {
        enum
        {
            SyncDisplaySettings = 1,
        };
    }

    enum class RdpClientMode : std::uint32_t
    {
        BasicSession = 0,
        EnhancedSession = 1,
        EnhancedVideoSyncedSession = 2
    };

    class MainWindow
        : public ATL::CWindowImpl<MainWindow>
    {
    public:

        DECLARE_WND_SUPERCLASS(
            L"NanaBoxMainWindow",
            L"Mile.Xaml.ContentWindow")

        BEGIN_MSG_MAP(MainWindow)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_COMMAND(OnCommand)
            MSG_WM_TIMER(OnTimer)
            MSG_WM_SIZE(OnSize)
            MSG_WM_SETFOCUS(OnSetFocus)
            MSG_WM_ACTIVATE(OnActivate)
            MSG_WM_CLOSE(OnClose)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_QUERYENDSESSION(OnQueryEndSession)
        END_MSG_MAP()

        MainWindow(
            std::wstring const& ConfigurationFilePath);

        int OnCreate(
            LPCREATESTRUCT lpCreateStruct);

        void OnCommand(
            UINT uNotifyCode,
            int nID,
            ATL::CWindow wndCtl);

        void OnTimer(
            UINT_PTR nIDEvent);

        void OnSize(
            UINT nType,
            CSize size);

        void OnSetFocus(
            ATL::CWindow wndOld);

        void OnActivate(
            UINT nState,
            BOOL bMinimized,
            ATL::CWindow wndOther);

        void OnClose();

        void OnDestroy();

        BOOL OnQueryEndSession(
            UINT nSource,
            UINT uLogOff);

    public:

        winrt::com_ptr<NanaBox::RdpClient> m_RdpClient;
        ATL::CAxWindow m_RdpClientWindow;

        void RdpClientOnDisconnected(
            _In_ LONG DisconnectReason);

    private:

        WTL::CIcon m_ApplicationIcon;
        winrt::com_ptr<IRDPENCPlatformContext> m_PlatformContext;
        winrt::com_ptr<IRDPENCNamedPipeDirectConnectorCallbacks> m_RdpNamedPipeCallbacks;
        winrt::com_ptr<IRDPENCNamedPipeDirectConnector> m_RdpNamedPipe;
        const int m_MainWindowControlHeight = 48;
        int m_RecommendedMainWindowControlHeight = m_MainWindowControlHeight;
        winrt::NanaBox::MainWindowControl m_MainWindowControl;
        std::wstring m_ConfigurationFilePath;
        NanaBox::VirtualMachineConfiguration m_Configuration;
        winrt::com_ptr<NanaBox::ComputeSystem> m_VirtualMachine;
        std::string m_VirtualMachineGuid;
        bool m_VirtualMachineRunning = false;
        bool m_VirtualMachineRestarting = false;
        RdpClientMode m_RdpClientMode = RdpClientMode::BasicSession;
        bool m_NeedRdpClientModeChange = false;
        CSize m_RecommendedDisplayResolution = CSize(1024, 768);
        std::uint32_t m_RecommendedZoomLevel = 100;
        CSize m_DisplayResolution = CSize(1024, 768);
        UINT64 m_SyncDisplaySettingsCheckPoint = 0;
        RECT m_RememberedMainWindowRect;
        LONG_PTR m_RememberedMainWindowStyle;
        std::wstring m_WindowTitle;
        bool m_MouseCaptureMode = false;

        void InitializeVirtualMachine();

        void TryReloadVirtualMachine();

        void RdpClientOnRemoteDesktopSizeChange(
            _In_ LONG Width,
            _In_ LONG Height);

        void RdpClientOnLoginComplete();

        void RdpClientOnRequestGoFullScreen();

        void RdpClientOnRequestLeaveFullScreen();

        void RdpClientOnRequestContainerMinimize();

        void RdpClientOnConfirmClose(
            _Out_ VARIANT_BOOL* pfAllowClose);

        void RdpClientOnFocusReleased(
            _In_ INT Direction);

        void RdpClientInitialize();

        void RdpClientUninitialize();

        void RdpClientConnect();
    };

    struct RdpNamedPipeCallbacks : winrt::implements<
        RdpNamedPipeCallbacks,
        IRDPENCNamedPipeDirectConnectorCallbacks>
    {
    public:

        RdpNamedPipeCallbacks(
            _In_ MainWindow* Instance);

        void STDMETHODCALLTYPE OnConnectionCompleted(
            _In_ IUnknown* pNetStream);

        void STDMETHODCALLTYPE OnConnectorError(
            _In_ HRESULT hrError);

    private:

        MainWindow* m_Instance;
    };
}
