/*
 * PROJECT:   NanaBox
 * FILE:      MsTscAx.h
 * PURPOSE:   Definition for the Remote Desktop ActiveX control interfaces
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#ifndef MSTSCAX_H
#define MSTSCAX_H

#ifndef __cplusplus
#error "[MsTscAx] You should use a C++ compiler."
#endif // !__cplusplus

// #import "MsTscAx.dll" \
//     no_namespace, \
//     raw_interfaces_only, \
//     raw_dispinterfaces, \
//     no_smart_pointers, \
//     named_guids

#include <comdef.h>

enum AutoReconnectContinueState
{
    autoReconnectContinueAutomatic = 0,
    autoReconnectContinueStop = 1,
    autoReconnectContinueManual = 2
};

enum RemoteProgramResult
{
    remoteAppResultOk = 0,
    remoteAppResultLocked = 1,
    remoteAppResultProtocolError = 2,
    remoteAppResultNotInWhitelist = 3,
    remoteAppResultNetworkPathDenied = 4,
    remoteAppResultFileNotFound = 5,
    remoteAppResultFailure = 6,
    remoteAppResultHookNotLoaded = 7
};

enum RemoteWindowDisplayedAttribute
{
    remoteAppWindowNone = 0,
    remoteAppWindowDisplayed = 1,
    remoteAppShellIconDisplayed = 2
};

enum DisconnectReasonCode
{
    disconnectReasonNoInfo = 0,
    disconnectReasonLocalNotError = 1,
    disconnectReasonRemoteByUser = 2,
    disconnectReasonByServer = 3,
    disconnectReasonDNSLookupFailed = 260,
    disconnectReasonOutOfMemory = 262,
    disconnectReasonConnectionTimedOut = 264,
    disconnectReasonSocketConnectFailed = 516,
    disconnectReasonOutOfMemory2 = 518,
    disconnectReasonHostNotFound = 520,
    disconnectReasonWinsockSendFailed = 772,
    disconnectReasonOutOfMemory3 = 774,
    disconnectReasonInvalidIPAddr = 776,
    disconnectReasonSocketRecvFailed = 1028,
    disconnectReasonInvalidSecurityData = 1030,
    disconnectReasonInternalError = 1032,
    disconnectReasonInvalidEncryption = 1286,
    disconnectReasonDNSLookupFailed2 = 1288,
    disconnectReasonGetHostByNameFailed = 1540,
    disconnectReasonInvalidServerSecurityInfo = 1542,
    disconnectReasonTimerError = 1544,
    disconnectReasonTimeoutOccurred = 1796,
    disconnectReasonServerCertificateUnpackErr = 1798,
    disconnectReasonInvalidIP = 2052,
    SSL_ERR_LOGON_FAILURE = 2055,
    disconnectReasonLicensingFailed = 2056,
    disconnectReasonAtClientWinsockFDCLOSE = 2308,
    disconnectReasonInternalSecurityError = 2310,
    disconnectReasonLicensingTimeout = 2312,
    disconnectReasonInternalSecurityError2 = 2566,
    SSL_ERR_NO_SUCH_USER = 2567,
    disconnectReasonEncryptionError = 2822,
    SSL_ERR_ACCOUNT_DISABLED = 2823,
    disconnectReasonDecryptionError = 3078,
    SSL_ERR_ACCOUNT_RESTRICTION = 3079,
    disconnectReasonClientDecompressionError = 3080,
    SSL_ERR_ACCOUNT_LOCKED_OUT = 3335,
    SSL_ERR_ACCOUNT_EXPIRED = 3591,
    SSL_ERR_PASSWORD_EXPIRED = 3847,
    SSL_ERR_PASSWORD_MUST_CHANGE = 4615,
    SSL_ERR_DELEGATION_POLICY = 5639,
    SSL_ERR_POLICY_NTLM_ONLY = 5895,
    SSL_ERR_CERT_EXPIRED = 6919,
    SSL_ERR_NO_AUTHENTICATING_AUTHORITY = 6151,
    SSL_ERR_SMARTCARD_WRONG_PIN = 7175,
    SSL_ERR_FRESH_CRED_REQUIRED_BY_SERVER = 8455,
    SSL_ERR_SMARTCARD_CARD_BLOCKED = 8711
};

enum ExtendedDisconnectReasonCode
{
    exDiscReasonNoInfo = 0,
    exDiscReasonAPIInitiatedDisconnect = 1,
    exDiscReasonAPIInitiatedLogoff = 2,
    exDiscReasonServerIdleTimeout = 3,
    exDiscReasonServerLogonTimeout = 4,
    exDiscReasonReplacedByOtherConnection = 5,
    exDiscReasonOutOfMemory = 6,
    exDiscReasonServerDeniedConnection = 7,
    exDiscReasonServerDeniedConnectionFips = 8,
    exDiscReasonServerInsufficientPrivileges = 9,
    exDiscReasonServerFreshCredsRequired = 10,
    exDiscReasonRpcInitiatedDisconnectByUser = 11,
    exDiscReasonLogoffByUser = 12,
    exDiscReasonShutdown = 25,
    exDiscReasonReboot = 26,
    exDiscReasonLicenseInternal = 256,
    exDiscReasonLicenseNoLicenseServer = 257,
    exDiscReasonLicenseNoLicense = 258,
    exDiscReasonLicenseErrClientMsg = 259,
    exDiscReasonLicenseHwidDoesntMatchLicense = 260,
    exDiscReasonLicenseErrClientLicense = 261,
    exDiscReasonLicenseCantFinishProtocol = 262,
    exDiscReasonLicenseClientEndedProtocol = 263,
    exDiscReasonLicenseErrClientEncryption = 264,
    exDiscReasonLicenseCantUpgradeLicense = 265,
    exDiscReasonLicenseNoRemoteConnections = 266,
    exDiscReasonLicenseCreatingLicStoreAccDenied = 267,
    exDiscReasonRdpEncInvalidCredentials = 768,
    exDiscReasonProtocolRangeStart = 4096,
    exDiscReasonProtocolRangeEnd = 32767
};

enum ControlCloseStatus
{
    controlCloseCanProceed = 0,
    controlCloseWaitForEvents = 1
};

enum RedirectionWarningType
{
    RedirectionWarningTypeDefault = 0,
    RedirectionWarningTypeUnsigned = 1,
    RedirectionWarningTypeUnknown = 2,
    RedirectionWarningTypeUser = 3,
    RedirectionWarningTypeThirdPartySigned = 4,
    RedirectionWarningTypeTrusted = 5,
    RedirectionWarningTypeMax = 5
};

enum RemoteSessionActionType
{
    RemoteSessionActionCharms = 0,
    RemoteSessionActionAppbar = 1,
    RemoteSessionActionSnap = 2,
    RemoteSessionActionStartScreen = 3,
    RemoteSessionActionAppSwitch = 4,
    RemoteSessionActionActionCenter = 5
};

enum ClientSpec
{
    FullMode = 0,
    ThinClientMode = 1,
    SmallCacheMode = 2
};

enum ControlReconnectStatus
{
    controlReconnectStarted = 0,
    controlReconnectBlocked = 1
};

enum CameraRedirEncodingQuality
{
    encodingQualityLow = 0,
    encodingQualityMedium = 1,
    encodingQualityHigh = 2
};

enum RemoteActionType
{
    RemoteActionCharms = 0,
    RemoteActionAppbar = 1,
    RemoteActionSnap = 2,
    RemoteActionStartScreen = 3,
    RemoteActionAppSwitch = 4
};

enum SnapshotEncodingType
{
    SnapshotEncodingDataUri = 0
};

enum SnapshotFormatType
{
    SnapshotFormatPng = 0,
    SnapshotFormatJpeg = 1,
    SnapshotFormatBmp = 2
};

const int TS_PERF_DISABLE_NOTHING = 0x00000000;
const int TS_PERF_DISABLE_WALLPAPER = 0x00000001;
const int TS_PERF_DISABLE_FULLWINDOWDRAG = 0x00000002;
const int TS_PERF_DISABLE_MENUANIMATIONS = 0x00000004;
const int TS_PERF_DISABLE_THEMING = 0x00000008;
const int TS_PERF_ENABLE_ENHANCED_GRAPHICS = 0x00000010;
const int TS_PERF_DISABLE_CURSOR_SHADOW = 0x00000020;
const int TS_PERF_DISABLE_CURSORSETTINGS = 0x00000040;
const int TS_PERF_ENABLE_FONT_SMOOTHING = 0x00000080;
const int TS_PERF_ENABLE_DESKTOP_COMPOSITION = 0x00000100;
const int TS_PERF_DEFAULT_NONPERFCLIENT_SETTING = 0x40000000;
const int TS_PERF_RESERVED1 = 0x80000000;

const int MSTSCAXEVENTS_DISPID_CONNECTING = 0x1;
const int MSTSCAXEVENTS_DISPID_CONNECTED = 0x2;
const int MSTSCAXEVENTS_DISPID_LOGINCOMPLETE = 0x3;
const int MSTSCAXEVENTS_DISPID_DISCONNECTED = 0x4;
const int MSTSCAXEVENTS_DISPID_ENTERFULLSCREENMODE = 0x5;
const int MSTSCAXEVENTS_DISPID_LEAVEFULLSCREENMODE = 0x6;
const int MSTSCAXEVENTS_DISPID_CHANNELRECEIVEDDATA = 0x7;
const int MSTSCAXEVENTS_DISPID_REQUESTGOFULLSCREEN = 0x8;
const int MSTSCAXEVENTS_DISPID_REQUESTLEAVEFULLSCREEN = 0x9;
const int MSTSCAXEVENTS_DISPID_FATALERROR = 0xa;
const int MSTSCAXEVENTS_DISPID_WARNING = 0xb;
const int MSTSCAXEVENTS_DISPID_REMOTEDESKTOPSIZECHANGE = 0xc;
const int MSTSCAXEVENTS_DISPID_IDLETIMEOUTNOTIFICATION = 0xd;
const int MSTSCAXEVENTS_DISPID_REQUESTCONTAINERMINIMIZE = 0xe;
const int MSTSCAXEVENTS_DISPID_CONFIRMCLOSE = 0xf;
const int MSTSCAXEVENTS_DISPID_RECEIVEDTSPUBLICKEY = 0x10;
const int MSTSCAXEVENTS_DISPID_AUTORECONNECTING = 0x11;
const int MSTSCAXEVENTS_DISPID_AUTHENTICATIONWARNINGDISPLAYED = 0x12;
const int MSTSCAXEVENTS_DISPID_AUTHENTICATIONWARNINGDISMISSED = 0x13;
const int MSTSCAXEVENTS_DISPID_REMOTEPROGRAMRESULT = 0x14;
const int MSTSCAXEVENTS_DISPID_REMOTEPROGRAMDISPLAYED = 0x15;
const int MSTSCAXEVENTS_DISPID_REMOTEWINDOWDISPLAYED = 0x1d;
const int MSTSCAXEVENTS_DISPID_LOGONERROR = 0x16;
const int MSTSCAXEVENTS_DISPID_FOCUSRELEASED = 0x17;
const int MSTSCAXEVENTS_DISPID_USERNAMEACQUIRED = 0x18;
const int MSTSCAXEVENTS_DISPID_MOUSEINPUTMODECHANGED = 0x1a;
const int MSTSCAXEVENTS_DISPID_SERVICEMESSAGERECEIVED = 0x1c;
const int MSTSCAXEVENTS_DISPID_CONNECTIONBARPULLDOWN = 0x1e;
const int MSTSCAXEVENTS_DISPID_NETWORKSTATUSCHANGED = 0x20;
const int MSTSCAXEVENTS_DISPID_DEVICESBUTTONPRESSED = 0x23;
const int MSTSCAXEVENTS_DISPID_AUTORECONNECTED = 0x21;
const int MSTSCAXEVENTS_DISPID_AUTORECONNECTING2 = 0x22;

const int REMOTEDESKTOPCLIENTEVENTS_DISPID_CONNECTING = 0x2ee;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_CONNECTED = 0x2ef;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_LOGINCOMPLETED = 0x2f0;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_DISCONNECTED = 0x2f1;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_STATUSCHANGED = 0x2f2;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_AUTORECONNECTING = 0x2f3;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_AUTORECONNECTED = 0x2f4;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_DIALOGDISPLAYING = 0x2f5;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_DIALOGDISMISSED = 0x2f6;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_NETWORKSTATUSCHANGED = 0x2f7;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_ADMINMESSAGERECEIVED = 0x2f8;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_KEYCOMBINATIONPRESSED = 0x2f9;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_REMOTEDESKTOPSIZECHANGED = 0x2fa;
const int REMOTEDESKTOPCLIENTEVENTS_DISPID_TOUCHPOINTERCURSORMOVED = 0x320;

MIDL_INTERFACE("336d5562-efa8-482e-8cb3-c5c0fc7a7db6")
IMsTscAxEvents : IDispatch
{
    // void OnConnecting();
    // void OnConnected();
    // void OnLoginComplete();
    // void OnDisconnected(
    //     _In_ LONG discReason);
    // void OnEnterFullScreenMode();
    // void OnLeaveFullScreenMode();
    // void OnChannelReceivedData(
    //     _In_ BSTR chanName,
    //     _In_ BSTR data);
    // void OnRequestGoFullScreen();
    // void OnRequestLeaveFullScreen();
    // void OnFatalError(
    //     _In_ LONG errorCode);
    // void OnWarning(
    //     _In_ LONG warningCode);
    // void OnRemoteDesktopSizeChange(
    //     _In_ LONG width,
    //     _In_ LONG height);
    // void OnIdleTimeoutNotification();
    // void OnRequestContainerMinimize();
    // void OnConfirmClose(
    //     _Out_ VARIANT_BOOL* pfAllowClose);
    // void OnReceivedTSPublicKey(
    //     _In_ BSTR publicKey,
    //     _Out_ VARIANT_BOOL* pfContinueLogon);
    // void OnAutoReconnecting(
    //     _In_ LONG disconnectReason,
    //     _In_ LONG attemptCount,
    //     _Out_ AutoReconnectContinueState* pArcContinueStatus);
    // void OnAuthenticationWarningDisplayed();
    // void OnAuthenticationWarningDismissed();
    // void OnRemoteProgramResult(
    //     _In_ BSTR bstrRemoteProgram,
    //     _In_ RemoteProgramResult lError,
    //     _In_ VARIANT_BOOL vbIsExecutable);
    // void OnRemoteProgramDisplayed(
    //     _In_ VARIANT_BOOL vbDisplayed,
    //     _In_ ULONG uDisplayInformation);
    // void OnRemoteWindowDisplayed(
    //     _In_ VARIANT_BOOL vbDisplayed,
    //     _In_ HWND hwnd,
    //     _In_ RemoteWindowDisplayedAttribute windowAttribute);
    // void OnLogonError(
    //     _In_ LONG lError);
    // void OnFocusReleased(
    //     _In_ INT iDirection);
    // void OnUserNameAcquired(
    //     _In_ BSTR bstrUserName);
    // void OnMouseInputModeChanged(
    //     _In_ VARIANT_BOOL fMouseModeRelative);
    // void OnServiceMessageReceived(
    //     _In_ BSTR serviceMessage);
    // void OnConnectionBarPullDown();
    // void OnNetworkStatusChanged(
    //     _In_ ULONG qualityLevel,
    //     _In_ LONG bandwidth,
    //     _In_ LONG rtt);
    // void OnDevicesButtonPressed();
    // void OnAutoReconnected();
    // void OnAutoReconnecting2(
    //     _In_ LONG disconnectReason,
    //     _In_ VARIANT_BOOL networkAvailable,
    //     _In_ LONG attemptCount,
    //     _In_ LONG maxAttemptCount);
};

MIDL_INTERFACE("c9d65442-a0f9-45b2-8f73-d61d2db8cbb6")
IMsTscSecuredSettings : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_StartProgram(
        _In_ BSTR pStartProgram) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartProgram(
        _Out_ BSTR* pStartProgram) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WorkDir(
        _In_ BSTR pWorkDir) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WorkDir(
        _Out_ BSTR* pWorkDir) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FullScreen(
        _In_ LONG pfFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FullScreen(
        _Out_ LONG* pfFullScreen) = 0;
};

MIDL_INTERFACE("809945cc-4b3b-4a92-a6b0-dbf9b5f2ef2d")
IMsTscAdvancedSettings : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_Compress(
        _In_ LONG pcompress) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Compress(
        _Out_ LONG* pcompress) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapPeristence(
        _In_ LONG pbitmapPeristence) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapPeristence(
        _Out_ LONG* pbitmapPeristence) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_allowBackgroundInput(
        _In_ LONG pallowBackgroundInput) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_allowBackgroundInput(
        _Out_ LONG* pallowBackgroundInput) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_KeyBoardLayoutStr(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PluginDlls(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IconFile(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IconIndex(
        _In_ LONG _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ContainerHandledFullScreen(
        _In_ LONG pContainerHandledFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ContainerHandledFullScreen(
        _Out_ LONG* pContainerHandledFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisableRdpdr(
        _In_ LONG pDisableRdpdr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisableRdpdr(
        _Out_ LONG* pDisableRdpdr) = 0;
};

MIDL_INTERFACE("209d0eb9-6254-47b1-9033-a98dae55bb27")
IMsTscDebug : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_HatchBitmapPDU(
        _In_ LONG phatchBitmapPDU) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HatchBitmapPDU(
        _Out_ LONG* phatchBitmapPDU) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HatchSSBOrder(
        _In_ LONG phatchSSBOrder) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HatchSSBOrder(
        _Out_ LONG* phatchSSBOrder) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HatchMembltOrder(
        _In_ LONG phatchMembltOrder) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HatchMembltOrder(
        _Out_ LONG* phatchMembltOrder) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HatchIndexPDU(
        _In_ LONG phatchIndexPDU) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HatchIndexPDU(
        _Out_ LONG* phatchIndexPDU) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LabelMemblt(
        _In_ LONG plabelMemblt) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LabelMemblt(
        _Out_ LONG* plabelMemblt) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapCacheMonitor(
        _In_ LONG pbitmapCacheMonitor) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapCacheMonitor(
        _Out_ LONG* pbitmapCacheMonitor) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MallocFailuresPercent(
        _In_ LONG pmallocFailuresPercent) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MallocFailuresPercent(
        _Out_ LONG* pmallocFailuresPercent) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MallocHugeFailuresPercent(
        _In_ LONG pmallocHugeFailuresPercent) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MallocHugeFailuresPercent(
        _Out_ LONG* pmallocHugeFailuresPercent) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NetThroughput(
        _In_ LONG NetThroughput) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NetThroughput(
        _Out_ LONG* NetThroughput) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CLXCmdLine(
        _In_ BSTR pCLXCmdLine) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CLXCmdLine(
        _Out_ BSTR* pCLXCmdLine) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CLXDll(
        _In_ BSTR pCLXDll) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CLXDll(
        _Out_ BSTR* pCLXDll) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramsHatchVisibleRegion(
        _In_ LONG pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramsHatchVisibleRegion(
        _Out_ LONG* pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramsHatchVisibleNoDataRegion(
        _In_ LONG pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramsHatchVisibleNoDataRegion(
        _Out_ LONG* pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramsHatchNonVisibleRegion(
        _In_ LONG pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramsHatchNonVisibleRegion(
        _Out_ LONG* pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramsHatchWindow(
        _In_ LONG pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramsHatchWindow(
        _Out_ LONG* pcbHatch) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramsStayConnectOnBadCaps(
        _In_ LONG pcbStayConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramsStayConnectOnBadCaps(
        _Out_ LONG* pcbStayConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlType(
        _Out_ UINT* pControlType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DecodeGfx(
        _In_ VARIANT_BOOL _arg1) = 0;
};

MIDL_INTERFACE("8c11efae-92c3-11d1-bc1e-00c04fa31489")
IMsTscAx : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_Server(
        _In_ BSTR pServer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Server(
        _Out_ BSTR* pServer) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Domain(
        _In_ BSTR pDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Domain(
        _Out_ BSTR* pDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UserName(
        _In_ BSTR pUserName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserName(
        _Out_ BSTR* pUserName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisconnectedText(
        _In_ BSTR pDisconnectedText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisconnectedText(
        _Out_ BSTR* pDisconnectedText) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectingText(
        _In_ BSTR pConnectingText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectingText(
        _Out_ BSTR* pConnectingText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Connected(
        _Out_ short* pIsConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DesktopWidth(
        _In_ LONG pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DesktopWidth(
        _Out_ LONG* pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DesktopHeight(
        _In_ LONG pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DesktopHeight(
        _Out_ LONG* pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartConnected(
        _In_ LONG pfStartConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartConnected(
        _Out_ LONG* pfStartConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HorizontalScrollBarVisible(
        _Out_ LONG* pfHScrollVisible) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VerticalScrollBarVisible(
        _Out_ LONG* pfVScrollVisible) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FullScreenTitle(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CipherStrength(
        _Out_ LONG* pCipherStrength) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Version(
        _Out_ BSTR* pVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecuredSettingsEnabled(
        _Out_ LONG* pSecuredSettingsEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecuredSettings(
        _Out_ struct IMsTscSecuredSettings** ppSecuredSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings(
        _Out_ struct IMsTscAdvancedSettings** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Debugger(
        _Out_ struct IMsTscDebug** ppDebugger) = 0;
    virtual HRESULT STDMETHODCALLTYPE Connect() = 0;
    virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateVirtualChannels(
        _In_ BSTR newVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendOnVirtualChannel(
        _In_ BSTR chanName,
        _In_ BSTR ChanData) = 0;
};

MIDL_INTERFACE("3c65b4ab-12b3-465b-acd4-b8dad3bff9e2")
IMsRdpClientAdvancedSettings : IMsTscAdvancedSettings
{
    virtual HRESULT STDMETHODCALLTYPE put_SmoothScroll(
        _In_ LONG psmoothScroll) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SmoothScroll(
        _Out_ LONG* psmoothScroll) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AcceleratorPassthrough(
        _In_ LONG pacceleratorPassthrough) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AcceleratorPassthrough(
        _Out_ LONG* pacceleratorPassthrough) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ShadowBitmap(
        _In_ LONG pshadowBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ShadowBitmap(
        _Out_ LONG* pshadowBitmap) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TransportType(
        _In_ LONG ptransportType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TransportType(
        _Out_ LONG* ptransportType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SasSequence(
        _In_ LONG psasSequence) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SasSequence(
        _Out_ LONG* psasSequence) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EncryptionEnabled(
        _In_ LONG pencryptionEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EncryptionEnabled(
        _Out_ LONG* pencryptionEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DedicatedTerminal(
        _In_ LONG pdedicatedTerminal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DedicatedTerminal(
        _Out_ LONG* pdedicatedTerminal) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RDPPort(
        _In_ LONG prdpPort) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RDPPort(
        _Out_ LONG* prdpPort) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableMouse(
        _In_ LONG penableMouse) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableMouse(
        _Out_ LONG* penableMouse) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisableCtrlAltDel(
        _In_ LONG pdisableCtrlAltDel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisableCtrlAltDel(
        _Out_ LONG* pdisableCtrlAltDel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableWindowsKey(
        _In_ LONG penableWindowsKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableWindowsKey(
        _Out_ LONG* penableWindowsKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DoubleClickDetect(
        _In_ LONG pdoubleClickDetect) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DoubleClickDetect(
        _Out_ LONG* pdoubleClickDetect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaximizeShell(
        _In_ LONG pmaximizeShell) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaximizeShell(
        _Out_ LONG* pmaximizeShell) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyFullScreen(
        _In_ LONG photKeyFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyFullScreen(
        _Out_ LONG* photKeyFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyCtrlEsc(
        _In_ LONG photKeyCtrlEsc) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyCtrlEsc(
        _Out_ LONG* photKeyCtrlEsc) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyAltEsc(
        _In_ LONG photKeyAltEsc) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyAltEsc(
        _Out_ LONG* photKeyAltEsc) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyAltTab(
        _In_ LONG photKeyAltTab) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyAltTab(
        _Out_ LONG* photKeyAltTab) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyAltShiftTab(
        _In_ LONG photKeyAltShiftTab) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyAltShiftTab(
        _Out_ LONG* photKeyAltShiftTab) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyAltSpace(
        _In_ LONG photKeyAltSpace) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyAltSpace(
        _Out_ LONG* photKeyAltSpace) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyCtrlAltDel(
        _In_ LONG photKeyCtrlAltDel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyCtrlAltDel(
        _Out_ LONG* photKeyCtrlAltDel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_orderDrawThreshold(
        _In_ LONG porderDrawThreshold) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_orderDrawThreshold(
        _Out_ LONG* porderDrawThreshold) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapCacheSize(
        _In_ LONG pbitmapCacheSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapCacheSize(
        _Out_ LONG* pbitmapCacheSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapVirtualCacheSize(
        _In_ LONG pbitmapVirtualCacheSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapVirtualCacheSize(
        _Out_ LONG* pbitmapVirtualCacheSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ScaleBitmapCachesByBPP(
        _In_ LONG pbScale) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ScaleBitmapCachesByBPP(
        _Out_ LONG* pbScale) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NumBitmapCaches(
        _In_ LONG pnumBitmapCaches) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NumBitmapCaches(
        _Out_ LONG* pnumBitmapCaches) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CachePersistenceActive(
        _In_ LONG pcachePersistenceActive) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CachePersistenceActive(
        _Out_ LONG* pcachePersistenceActive) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PersistCacheDirectory(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_brushSupportLevel(
        _In_ LONG pbrushSupportLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_brushSupportLevel(
        _Out_ LONG* pbrushSupportLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_minInputSendInterval(
        _In_ LONG pminInputSendInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_minInputSendInterval(
        _Out_ LONG* pminInputSendInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_InputEventsAtOnce(
        _In_ LONG pinputEventsAtOnce) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InputEventsAtOnce(
        _Out_ LONG* pinputEventsAtOnce) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_maxEventCount(
        _In_ LONG pmaxEventCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_maxEventCount(
        _Out_ LONG* pmaxEventCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_keepAliveInterval(
        _In_ LONG pkeepAliveInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_keepAliveInterval(
        _Out_ LONG* pkeepAliveInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_shutdownTimeout(
        _In_ LONG pshutdownTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_shutdownTimeout(
        _Out_ LONG* pshutdownTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_overallConnectionTimeout(
        _In_ LONG poverallConnectionTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_overallConnectionTimeout(
        _Out_ LONG* poverallConnectionTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_singleConnectionTimeout(
        _In_ LONG psingleConnectionTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_singleConnectionTimeout(
        _Out_ LONG* psingleConnectionTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_KeyboardType(
        _In_ LONG pkeyboardType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_KeyboardType(
        _Out_ LONG* pkeyboardType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_KeyboardSubType(
        _In_ LONG pkeyboardSubType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_KeyboardSubType(
        _Out_ LONG* pkeyboardSubType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_KeyboardFunctionKey(
        _In_ LONG pkeyboardFunctionKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_KeyboardFunctionKey(
        _Out_ LONG* pkeyboardFunctionKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WinceFixedPalette(
        _In_ LONG pwinceFixedPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WinceFixedPalette(
        _Out_ LONG* pwinceFixedPalette) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectToServerConsole(
        _In_ VARIANT_BOOL pConnectToConsole) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectToServerConsole(
        _Out_ VARIANT_BOOL* pConnectToConsole) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapPersistence(
        _In_ LONG pbitmapPersistence) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapPersistence(
        _Out_ LONG* pbitmapPersistence) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MinutesToIdleTimeout(
        _In_ LONG pminutesToIdleTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinutesToIdleTimeout(
        _Out_ LONG* pminutesToIdleTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SmartSizing(
        _In_ VARIANT_BOOL pfSmartSizing) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SmartSizing(
        _Out_ VARIANT_BOOL* pfSmartSizing) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RdpdrLocalPrintingDocName(
        _In_ BSTR pLocalPrintingDocName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RdpdrLocalPrintingDocName(
        _Out_ BSTR* pLocalPrintingDocName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RdpdrClipCleanTempDirString(
        _In_ BSTR clipCleanTempDirString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RdpdrClipCleanTempDirString(
        _Out_ BSTR* clipCleanTempDirString) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RdpdrClipPasteInfoString(
        _In_ BSTR clipPasteInfoString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RdpdrClipPasteInfoString(
        _Out_ BSTR* clipPasteInfoString) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ClearTextPassword(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisplayConnectionBar(
        _In_ VARIANT_BOOL pDisplayConnectionBar) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayConnectionBar(
        _Out_ VARIANT_BOOL* pDisplayConnectionBar) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PinConnectionBar(
        _In_ VARIANT_BOOL pPinConnectionBar) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PinConnectionBar(
        _Out_ VARIANT_BOOL* pPinConnectionBar) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GrabFocusOnConnect(
        _In_ VARIANT_BOOL pfGrabFocusOnConnect) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GrabFocusOnConnect(
        _Out_ VARIANT_BOOL* pfGrabFocusOnConnect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LoadBalanceInfo(
        _In_ BSTR pLBInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LoadBalanceInfo(
        _Out_ BSTR* pLBInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectDrives(
        _In_ VARIANT_BOOL pRedirectDrives) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectDrives(
        _Out_ VARIANT_BOOL* pRedirectDrives) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectPrinters(
        _In_ VARIANT_BOOL pRedirectPrinters) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectPrinters(
        _Out_ VARIANT_BOOL* pRedirectPrinters) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectPorts(
        _In_ VARIANT_BOOL pRedirectPorts) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectPorts(
        _Out_ VARIANT_BOOL* pRedirectPorts) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectSmartCards(
        _In_ VARIANT_BOOL pRedirectSmartCards) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectSmartCards(
        _Out_ VARIANT_BOOL* pRedirectSmartCards) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapVirtualCache16BppSize(
        _In_ LONG pBitmapVirtualCache16BppSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapVirtualCache16BppSize(
        _Out_ LONG* pBitmapVirtualCache16BppSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapVirtualCache24BppSize(
        _In_ LONG pBitmapVirtualCache24BppSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapVirtualCache24BppSize(
        _Out_ LONG* pBitmapVirtualCache24BppSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PerformanceFlags(
        _In_ LONG pDisableList) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PerformanceFlags(
        _Out_ LONG* pDisableList) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectWithEndpoint(
        _In_ VARIANT* _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NotifyTSPublicKey(
        _In_ VARIANT_BOOL pfNotify) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NotifyTSPublicKey(
        _Out_ VARIANT_BOOL* pfNotify) = 0;
};

MIDL_INTERFACE("605befcf-39c1-45cc-a811-068fb7be346d")
IMsRdpClientSecuredSettings : IMsTscSecuredSettings
{
    virtual HRESULT STDMETHODCALLTYPE put_KeyboardHookMode(
        _In_ LONG pkeyboardHookMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_KeyboardHookMode(
        _Out_ LONG* pkeyboardHookMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AudioRedirectionMode(
        _In_ LONG pAudioRedirectionMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AudioRedirectionMode(
        _Out_ LONG* pAudioRedirectionMode) = 0;
};

MIDL_INTERFACE("92b4a539-7115-4b7c-a5a9-e5d9efc2780a")
IMsRdpClient : IMsTscAx
{
    virtual HRESULT STDMETHODCALLTYPE put_ColorDepth(
        _In_ LONG pcolorDepth) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ColorDepth(
        _Out_ LONG* pcolorDepth) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings2(
        _Out_ struct IMsRdpClientAdvancedSettings** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecuredSettings2(
        _Out_ struct IMsRdpClientSecuredSettings** ppSecuredSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ExtendedDisconnectReason(
        _Out_ ExtendedDisconnectReasonCode* pExtendedDisconnectReason) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FullScreen(
        _In_ VARIANT_BOOL pfFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FullScreen(
        _Out_ VARIANT_BOOL* pfFullScreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVirtualChannelOptions(
        _In_ BSTR chanName,
        _In_ LONG chanOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVirtualChannelOptions(
        _In_ BSTR chanName,
        _Out_ LONG* pChanOptions) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestClose(
        _Out_ ControlCloseStatus* pCloseStatus) = 0;
};

MIDL_INTERFACE("c1e6743a-41c1-4a74-832a-0dd06c1c7a0e")
IMsTscNonScriptable : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE put_ClearTextPassword(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PortablePassword(
        _In_ BSTR pPortablePass) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PortablePassword(
        _Out_ BSTR* pPortablePass) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PortableSalt(
        _In_ BSTR pPortableSalt) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PortableSalt(
        _Out_ BSTR* pPortableSalt) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BinaryPassword(
        _In_ BSTR pBinaryPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BinaryPassword(
        _Out_ BSTR* pBinaryPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BinarySalt(
        _In_ BSTR pSalt) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BinarySalt(
        _Out_ BSTR* pSalt) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetPassword() = 0;
};

MIDL_INTERFACE("2f079c4c-87b2-4afd-97ab-20cdb43038ae")
IMsRdpClientNonScriptable : IMsTscNonScriptable
{
    virtual HRESULT STDMETHODCALLTYPE NotifyRedirectDeviceChange(
        _In_ UINT_PTR wParam,
        _In_ LONG_PTR lParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendKeys(
        _In_ LONG numKeys,
        _In_ VARIANT_BOOL* pbArrayKeyUp,
        _In_ LONG* plKeyData) = 0;
};

MIDL_INTERFACE("9ac42117-2b76-4320-aa44-0e616ab8437b")
IMsRdpClientAdvancedSettings2 : IMsRdpClientAdvancedSettings
{
    virtual HRESULT STDMETHODCALLTYPE get_CanAutoReconnect(
        _Out_ VARIANT_BOOL* pfCanAutoReconnect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableAutoReconnect(
        _In_ VARIANT_BOOL pfEnableAutoReconnect) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableAutoReconnect(
        _Out_ VARIANT_BOOL* pfEnableAutoReconnect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxReconnectAttempts(
        _In_ LONG pMaxReconnectAttempts) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxReconnectAttempts(
        _Out_ LONG* pMaxReconnectAttempts) = 0;
};

MIDL_INTERFACE("e7e17dc4-3b71-4ba7-a8e6-281ffadca28f")
IMsRdpClient2 : IMsRdpClient
{
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings3(
        _Out_ struct IMsRdpClientAdvancedSettings2** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectedStatusText(
        _In_ BSTR pConnectedStatusText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectedStatusText(
        _Out_ BSTR* pConnectedStatusText) = 0;
};

MIDL_INTERFACE("19cd856b-c542-4c53-acee-f127e3be1a59")
IMsRdpClientAdvancedSettings3 : IMsRdpClientAdvancedSettings2
{
    virtual HRESULT STDMETHODCALLTYPE put_ConnectionBarShowMinimizeButton(
        _In_ VARIANT_BOOL pfShowMinimize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectionBarShowMinimizeButton(
        _Out_ VARIANT_BOOL* pfShowMinimize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectionBarShowRestoreButton(
        _In_ VARIANT_BOOL pfShowRestore) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectionBarShowRestoreButton(
        _Out_ VARIANT_BOOL* pfShowRestore) = 0;
};

MIDL_INTERFACE("91b7cbc5-a72e-4fa0-9300-d647d7e897ff")
IMsRdpClient3 : IMsRdpClient2
{
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings4(
        _Out_ struct IMsRdpClientAdvancedSettings3** ppAdvSettings) = 0;
};

MIDL_INTERFACE("fba7f64e-7345-4405-ae50-fa4a763dc0de")
IMsRdpClientAdvancedSettings4 : IMsRdpClientAdvancedSettings3
{
    virtual HRESULT STDMETHODCALLTYPE put_AuthenticationLevel(
        _In_ UINT puiAuthLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AuthenticationLevel(
        _Out_ UINT* puiAuthLevel) = 0;
};

MIDL_INTERFACE("095e0738-d97d-488b-b9f6-dd0e8d66c0de")
IMsRdpClient4 : IMsRdpClient3
{
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings5(
        _Out_ struct IMsRdpClientAdvancedSettings4** ppAdvSettings) = 0;
};

MIDL_INTERFACE("17a5e535-4072-4fa4-af32-c8d0d47345e9")
IMsRdpClientNonScriptable2 : IMsRdpClientNonScriptable
{
    virtual HRESULT STDMETHODCALLTYPE put_UIParentWindowHandle(
        _In_ HWND phwndUIParentWindowHandle) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UIParentWindowHandle(
        _Out_ HWND* phwndUIParentWindowHandle) = 0;
};

MIDL_INTERFACE("720298c0-a099-46f5-9f82-96921bae4701")
IMsRdpClientTransportSettings : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_GatewayHostname(
        _In_ BSTR pProxyHostname) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayHostname(
        _Out_ BSTR* pProxyHostname) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayUsageMethod(
        _In_ ULONG pulProxyUsageMethod) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayUsageMethod(
        _Out_ ULONG* pulProxyUsageMethod) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayProfileUsageMethod(
        _In_ ULONG pulProxyProfileUsageMethod) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayProfileUsageMethod(
        _Out_ ULONG* pulProxyProfileUsageMethod) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayCredsSource(
        _In_ ULONG pulProxyCredsSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayCredsSource(
        _Out_ ULONG* pulProxyCredsSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayUserSelectedCredsSource(
        _In_ ULONG pulProxyCredsSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayUserSelectedCredsSource(
        _Out_ ULONG* pulProxyCredsSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayIsSupported(
        _Out_ LONG* pfProxyIsSupported) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayDefaultUsageMethod(
        _Out_ ULONG* pulProxyDefaultUsageMethod) = 0;
};

MIDL_INTERFACE("fba7f64e-6783-4405-da45-fa4a763dabd0")
IMsRdpClientAdvancedSettings5 : IMsRdpClientAdvancedSettings4
{
    virtual HRESULT STDMETHODCALLTYPE put_RedirectClipboard(
        _In_ VARIANT_BOOL pfRedirectClipboard) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectClipboard(
        _Out_ VARIANT_BOOL* pfRedirectClipboard) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AudioRedirectionMode(
        _In_ UINT puiAudioRedirectionMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AudioRedirectionMode(
        _Out_ UINT* puiAudioRedirectionMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectionBarShowPinButton(
        _In_ VARIANT_BOOL pfShowPin) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectionBarShowPinButton(
        _Out_ VARIANT_BOOL* pfShowPin) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PublicMode(
        _In_ VARIANT_BOOL pfPublicMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PublicMode(
        _Out_ VARIANT_BOOL* pfPublicMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectDevices(
        _In_ VARIANT_BOOL pfRedirectPnPDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectDevices(
        _Out_ VARIANT_BOOL* pfRedirectPnPDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectPOSDevices(
        _In_ VARIANT_BOOL pfRedirectPOSDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectPOSDevices(
        _Out_ VARIANT_BOOL* pfRedirectPOSDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BitmapVirtualCache32BppSize(
        _In_ LONG pBitmapVirtualCache32BppSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BitmapVirtualCache32BppSize(
        _Out_ LONG* pBitmapVirtualCache32BppSize) = 0;
};

MIDL_INTERFACE("fdd029f9-467a-4c49-8529-64b521dbd1b4")
ITSRemoteProgram : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_RemoteProgramMode(
        _In_ VARIANT_BOOL pvboolRemoteProgramMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgramMode(
        _Out_ VARIANT_BOOL* pvboolRemoteProgramMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE ServerStartProgram(
        _In_ BSTR bstrExecutablePath,
        _In_ BSTR bstrFilePath,
        _In_ BSTR bstrWorkingDirectory,
        _In_ VARIANT_BOOL vbExpandEnvVarInWorkingDirectoryOnServer,
        _In_ BSTR bstrArguments,
        _In_ VARIANT_BOOL vbExpandEnvVarInArgumentsOnServer) = 0;
};

MIDL_INTERFACE("d012ae6d-c19a-4bfe-b367-201f8911f134")
IMsRdpClientShell : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE Launch() = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RdpFileContents(
        _In_ BSTR pszRdpFile) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RdpFileContents(
        _Out_ BSTR* pszRdpFile) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRdpProperty(
        _In_ BSTR szProperty,
        _In_ VARIANT Value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRdpProperty(
        _In_ BSTR szProperty,
        _Out_ VARIANT* pValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRemoteProgramClientInstalled(
        _Out_ VARIANT_BOOL* pbClientInstalled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PublicMode(
        _In_ VARIANT_BOOL pfPublicMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PublicMode(
        _Out_ VARIANT_BOOL* pfPublicMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowTrustedSitesManagementDialog() = 0;
};

MIDL_INTERFACE("4eb5335b-6429-477d-b922-e06a28ecd8bf")
IMsRdpClient5 : IMsRdpClient4
{
    virtual HRESULT STDMETHODCALLTYPE get_TransportSettings(
        _Out_ struct IMsRdpClientTransportSettings** ppXportSet) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings6(
        _Out_ struct IMsRdpClientAdvancedSettings5** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrorDescription(
        _In_ UINT disconnectReason,
        _In_ UINT ExtendedDisconnectReason,
        _Out_ BSTR* pBstrErrorMsg) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgram(
        _Out_ struct ITSRemoteProgram** ppRemoteProgram) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MsRdpClientShell(
        _Out_ struct IMsRdpClientShell** ppLauncher) = 0;
};

MIDL_INTERFACE("60c3b9c8-9e92-4f5e-a3e7-604a912093ea")
IMsRdpDevice : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_DeviceInstanceId(
        _Out_ BSTR* pDevInstanceId) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FriendlyName(
        _Out_ BSTR* pFriendlyName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceDescription(
        _Out_ BSTR* pDeviceDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectionState(
        _In_ VARIANT_BOOL pvboolRedirState) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectionState(
        _Out_ VARIANT_BOOL* pvboolRedirState) = 0;
};

MIDL_INTERFACE("5fb94466-7661-42a8-98b7-01904c11668f")
IMsRdpDeviceV2 : IMsRdpDevice
{
    virtual HRESULT STDMETHODCALLTYPE get_DeviceText(
        _Out_ BSTR* pDeviceText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsUSBDevice(
        _Out_ VARIANT_BOOL* pvboolUSBDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsCompositeDevice(
        _Out_ VARIANT_BOOL* pvboolCompositeDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DriveLetterBitmap(
        _Out_ ULONG* pDriveLetterBitmap) = 0;
};

MIDL_INTERFACE("56540617-d281-488c-8738-6a8fdf64a118")
IMsRdpDeviceCollection : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE RescanDevices(
        _In_ VARIANT_BOOL vboolDynRedir) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceByIndex(
        _In_ ULONG index,
        _Out_ struct IMsRdpDevice** ppDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceById(
        _In_ BSTR devInstanceId,
        _Out_ struct IMsRdpDevice** ppDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceCount(
        _Out_ ULONG* pDeviceCount) = 0;
};

MIDL_INTERFACE("d28b5458-f694-47a8-8e61-40356a767e46")
IMsRdpDrive : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_Name(
        _Out_ BSTR* pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectionState(
        _In_ VARIANT_BOOL pvboolRedirState) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectionState(
        _Out_ VARIANT_BOOL* pvboolRedirState) = 0;
};

MIDL_INTERFACE("3e05417c-2721-4008-9d80-4edf1539c817")
IMsRdpDriveV2 : IMsRdpDrive
{
    virtual HRESULT STDMETHODCALLTYPE get_DriveLetterIndex(
        _Out_ ULONG* pDriveLetterIndex) = 0;
};

MIDL_INTERFACE("7ff17599-da2c-4677-ad35-f60c04fe1585")
IMsRdpDriveCollection : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE RescanDrives(
        VARIANT_BOOL vboolDynRedir) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DriveByIndex(
        _In_ ULONG index,
        _Out_ struct IMsRdpDrive** ppDevice) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DriveCount(
        _Out_ ULONG* pDriveCount) = 0;
};

MIDL_INTERFACE("b3378d90-0728-45c7-8ed7-b6159fb92219")
IMsRdpClientNonScriptable3 : IMsRdpClientNonScriptable2
{
    virtual HRESULT STDMETHODCALLTYPE put_ShowRedirectionWarningDialog(
        _In_ VARIANT_BOOL pfShowRdrDlg) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ShowRedirectionWarningDialog(
        _Out_ VARIANT_BOOL* pfShowRdrDlg) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PromptForCredentials(
        _In_ VARIANT_BOOL pfPrompt) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PromptForCredentials(
        _Out_ VARIANT_BOOL* pfPrompt) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NegotiateSecurityLayer(
        _In_ VARIANT_BOOL pfNegotiate) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NegotiateSecurityLayer(
        _Out_ VARIANT_BOOL* pfNegotiate) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableCredSspSupport(
        _In_ VARIANT_BOOL pfEnableSupport) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableCredSspSupport(
        _Out_ VARIANT_BOOL* pfEnableSupport) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectDynamicDrives(
        _In_ VARIANT_BOOL pfRedirectDynamicDrives) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectDynamicDrives(
        _Out_ VARIANT_BOOL* pfRedirectDynamicDrives) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectDynamicDevices(
        _In_ VARIANT_BOOL pfRedirectDynamicDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectDynamicDevices(
        _Out_ VARIANT_BOOL* pfRedirectDynamicDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceCollection(
        _Out_ struct IMsRdpDeviceCollection** ppDeviceCollection) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DriveCollection(
        _Out_ struct IMsRdpDriveCollection** ppDeviceCollection) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WarnAboutSendingCredentials(
        _In_ VARIANT_BOOL pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WarnAboutSendingCredentials(
        _Out_ VARIANT_BOOL* pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WarnAboutClipboardRedirection(
        _In_ VARIANT_BOOL pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WarnAboutClipboardRedirection(
        _Out_ VARIANT_BOOL* pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectionBarText(
        _In_ BSTR pConnectionBarText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectionBarText(
        _Out_ BSTR* pConnectionBarText) = 0;
};

MIDL_INTERFACE("222c4b5d-45d9-4df0-a7c6-60cf9089d285")
IMsRdpClientAdvancedSettings6 : IMsRdpClientAdvancedSettings5
{
    virtual HRESULT STDMETHODCALLTYPE put_RelativeMouseMode(
        _In_ VARIANT_BOOL pfRelativeMouseMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RelativeMouseMode(
        _Out_ VARIANT_BOOL* pfRelativeMouseMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AuthenticationServiceClass(
        _Out_ BSTR* pbstrAuthServiceClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AuthenticationServiceClass(
        _In_ BSTR pbstrAuthServiceClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PCB(
        _Out_ BSTR* bstrPCB) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PCB(
        _In_ BSTR bstrPCB) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyFocusReleaseLeft(
        _In_ LONG HotKeyFocusReleaseLeft) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyFocusReleaseLeft(
        _Out_ LONG* HotKeyFocusReleaseLeft) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HotKeyFocusReleaseRight(
        _In_ LONG HotKeyFocusReleaseRight) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HotKeyFocusReleaseRight(
        _Out_ LONG* HotKeyFocusReleaseRight) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableCredSspSupport(
        _In_ VARIANT_BOOL pfEnableSupport) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableCredSspSupport(
        _Out_ VARIANT_BOOL* pfEnableSupport) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AuthenticationType(
        _Out_ UINT* puiAuthType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ConnectToAdministerServer(
        _In_ VARIANT_BOOL pConnectToAdministerServer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectToAdministerServer(
        _Out_ VARIANT_BOOL* pConnectToAdministerServer) = 0;
};

MIDL_INTERFACE("67341688-d606-4c73-a5d2-2e0489009319")
IMsRdpClientTransportSettings2 : IMsRdpClientTransportSettings
{
    virtual HRESULT STDMETHODCALLTYPE put_GatewayCredSharing(
        _In_ ULONG pulProxyCredSharing) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayCredSharing(
        _Out_ ULONG* pulProxyCredSharing) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayPreAuthRequirement(
        _In_ ULONG pulProxyPreAuthRequirement) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayPreAuthRequirement(
        _Out_ ULONG* pulProxyPreAuthRequirement) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayPreAuthServerAddr(
        _In_ BSTR pbstrProxyPreAuthServerAddr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayPreAuthServerAddr(
        _Out_ BSTR* pbstrProxyPreAuthServerAddr) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewaySupportUrl(
        _In_ BSTR pbstrProxySupportUrl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewaySupportUrl(
        _Out_ BSTR* pbstrProxySupportUrl) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayEncryptedOtpCookie(
        _In_ BSTR pbstrEncryptedOtpCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayEncryptedOtpCookie(
        _Out_ BSTR* pbstrEncryptedOtpCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayEncryptedOtpCookieSize(
        _In_ ULONG pulEncryptedOtpCookieSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayEncryptedOtpCookieSize(
        _Out_ ULONG* pulEncryptedOtpCookieSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayUsername(
        _In_ BSTR pProxyUsername) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayUsername(
        _Out_ BSTR* pProxyUsername) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayDomain(
        _In_ BSTR pProxyDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayDomain(
        _Out_ BSTR* pProxyDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayPassword(
        _In_ BSTR _arg1) = 0;
};

MIDL_INTERFACE("d43b7d80-8517-4b6d-9eac-96ad6800d7f2")
IMsRdpClient6 : IMsRdpClient5
{
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings7(
        _Out_ struct IMsRdpClientAdvancedSettings6** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TransportSettings2(
        _Out_ struct IMsRdpClientTransportSettings2** ppXportSet2) = 0;
};

MIDL_INTERFACE("f50fa8aa-1c7d-4f59-b15c-a90cacae1fcb")
IMsRdpClientNonScriptable4 : IMsRdpClientNonScriptable3
{
    virtual HRESULT STDMETHODCALLTYPE put_RedirectionWarningType(
        _In_ RedirectionWarningType pWrnType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectionWarningType(
        _Out_ RedirectionWarningType* pWrnType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MarkRdpSettingsSecure(
        _In_ VARIANT_BOOL pfRdpSecure) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MarkRdpSettingsSecure(
        _Out_ VARIANT_BOOL* pfRdpSecure) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PublisherCertificateChain(
        _In_ VARIANT* pVarCert) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PublisherCertificateChain(
        _Out_ VARIANT* pVarCert) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WarnAboutPrinterRedirection(
        _In_ VARIANT_BOOL pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WarnAboutPrinterRedirection(
        _Out_ VARIANT_BOOL* pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AllowCredentialSaving(
        _In_ VARIANT_BOOL pfAllowSave) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AllowCredentialSaving(
        _Out_ VARIANT_BOOL* pfAllowSave) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PromptForCredsOnClient(
        _In_ VARIANT_BOOL pfPromptForCredsOnClient) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PromptForCredsOnClient(
        _Out_ VARIANT_BOOL* pfPromptForCredsOnClient) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LaunchedViaClientShellInterface(
        _In_ VARIANT_BOOL pfLaunchedViaClientShellInterface) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LaunchedViaClientShellInterface(
        _Out_ VARIANT_BOOL* pfLaunchedViaClientShellInterface) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TrustedZoneSite(
        _In_ VARIANT_BOOL pfIsTrustedZone) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TrustedZoneSite(
        _Out_ VARIANT_BOOL* pfIsTrustedZone) = 0;
};

MIDL_INTERFACE("26036036-4010-4578-8091-0db9a1edf9c3")
IMsRdpClientAdvancedSettings7 : IMsRdpClientAdvancedSettings6
{
    virtual HRESULT STDMETHODCALLTYPE put_AudioCaptureRedirectionMode(
        _In_ VARIANT_BOOL pfRedir) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AudioCaptureRedirectionMode(
        _Out_ VARIANT_BOOL* pfRedir) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VideoPlaybackMode(
        _In_ UINT pVideoPlaybackMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VideoPlaybackMode(
        _Out_ UINT* pVideoPlaybackMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EnableSuperPan(
        _In_ VARIANT_BOOL pfEnableSuperPan) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnableSuperPan(
        _Out_ VARIANT_BOOL* pfEnableSuperPan) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SuperPanAccelerationFactor(
        _In_ ULONG puAccelFactor) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SuperPanAccelerationFactor(
        _Out_ ULONG* puAccelFactor) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NegotiateSecurityLayer(
        _In_ VARIANT_BOOL pfNegotiate) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NegotiateSecurityLayer(
        _Out_ VARIANT_BOOL* pfNegotiate) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AudioQualityMode(
        _In_ UINT pAudioQualityMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AudioQualityMode(
        _Out_ UINT* pAudioQualityMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectDirectX(
        _In_ VARIANT_BOOL pfRedirectDirectX) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectDirectX(
        _Out_ VARIANT_BOOL* pfRedirectDirectX) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NetworkConnectionType(
        _In_ UINT pConnectionType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NetworkConnectionType(
        _Out_ UINT* pConnectionType) = 0;
};

MIDL_INTERFACE("3d5b21ac-748d-41de-8f30-e15169586bd4")
IMsRdpClientTransportSettings3 : IMsRdpClientTransportSettings2
{
    virtual HRESULT STDMETHODCALLTYPE put_GatewayCredSourceCookie(
        _In_ ULONG pulProxyCredSourceCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayCredSourceCookie(
        _Out_ ULONG* pulProxyCredSourceCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayAuthCookieServerAddr(
        _In_ BSTR pbstrProxyAuthCookieServerAddr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayAuthCookieServerAddr(
        _Out_ BSTR* pbstrProxyAuthCookieServerAddr) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayEncryptedAuthCookie(
        _In_ BSTR pbstrEncryptedAuthCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayEncryptedAuthCookie(
        _Out_ BSTR* pbstrEncryptedAuthCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayEncryptedAuthCookieSize(
        _In_ ULONG pulEncryptedAuthCookieSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayEncryptedAuthCookieSize(
        _Out_ ULONG* pulEncryptedAuthCookieSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GatewayAuthLoginPage(
        _In_ BSTR pbstrProxyAuthLoginPage) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GatewayAuthLoginPage(
        _Out_ BSTR* pbstrProxyAuthLoginPage) = 0;
};

MIDL_INTERFACE("25f2ce20-8b1d-4971-a7cd-549dae201fc0")
IMsRdpClientSecuredSettings2 : IMsRdpClientSecuredSettings
{
    virtual HRESULT STDMETHODCALLTYPE get_PCB(
        _Out_ BSTR* bstrPCB) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PCB(
        _In_ BSTR bstrPCB) = 0;
};

MIDL_INTERFACE("92c38a7d-241a-418c-9936-099872c9af20")
ITSRemoteProgram2 : ITSRemoteProgram
{
    virtual HRESULT STDMETHODCALLTYPE put_RemoteApplicationName(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteApplicationProgram(
        _In_ BSTR _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteApplicationArgs(
        _In_ BSTR _arg1) = 0;
};

MIDL_INTERFACE("b2a5b5ce-3461-444a-91d4-add26d070638")
IMsRdpClient7 : IMsRdpClient6
{
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings8(
        _Out_ struct IMsRdpClientAdvancedSettings7** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TransportSettings3(
        _Out_ struct IMsRdpClientTransportSettings3** ppXportSet3) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStatusText(
        _In_ UINT statusCode,
        _Out_ BSTR* pBstrStatusText) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecuredSettings3(
        _Out_ struct IMsRdpClientSecuredSettings2** ppSecuredSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgram2(
        _Out_ struct ITSRemoteProgram2** ppRemoteProgram) = 0;
};

MIDL_INTERFACE("4f6996d5-d7b1-412c-b0ff-063718566907")
IMsRdpClientNonScriptable5 : IMsRdpClientNonScriptable4
{
    virtual HRESULT STDMETHODCALLTYPE put_UseMultimon(
        _In_ VARIANT_BOOL pfUseMultimon) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UseMultimon(
        _Out_ VARIANT_BOOL* pfUseMultimon) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteMonitorCount(
        _Out_ ULONG* pcRemoteMonitors) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRemoteMonitorsBoundingBox(
        _Out_ LONG* pLeft,
        _Out_ LONG* pTop,
        _Out_ LONG* pRight,
        _Out_ LONG* pBottom) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RemoteMonitorLayoutMatchesLocal(
        _Out_ VARIANT_BOOL* pfRemoteMatchesLocal) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisableConnectionBar(
        _In_ VARIANT_BOOL _arg1) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisableRemoteAppCapsCheck(
        _In_ VARIANT_BOOL pfDisableRemoteAppCapsCheck) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisableRemoteAppCapsCheck(
        _Out_ VARIANT_BOOL* pfDisableRemoteAppCapsCheck) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WarnAboutDirectXRedirection(
        _In_ VARIANT_BOOL pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WarnAboutDirectXRedirection(
        _Out_ VARIANT_BOOL* pfWarn) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AllowPromptingForCredentials(
        _In_ VARIANT_BOOL pfAllow) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AllowPromptingForCredentials(
        _Out_ VARIANT_BOOL* pfAllow) = 0;
};

MIDL_INTERFACE("fdd029f9-9574-4def-8529-64b521cccaa4")
IMsRdpPreferredRedirectionInfo : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE put_UseRedirectionServerName(
        _In_ VARIANT_BOOL pVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UseRedirectionServerName(
        _Out_ VARIANT_BOOL* pVal) = 0;
};

MIDL_INTERFACE("302d8188-0052-4807-806a-362b628f9ac5")
IMsRdpExtendedSettings : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE put_Property(
        _In_ BSTR bstrPropertyName,
        _In_ VARIANT* pValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Property(
        _In_ BSTR bstrPropertyName,
        _Out_ VARIANT* pValue) = 0;
};

MIDL_INTERFACE("89acb528-2557-4d16-8625-226a30e97e9a")
IMsRdpClientAdvancedSettings8 : IMsRdpClientAdvancedSettings7
{
    virtual HRESULT STDMETHODCALLTYPE put_BandwidthDetection(
        _In_ VARIANT_BOOL pfAutodetect) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BandwidthDetection(
        _Out_ VARIANT_BOOL* pfAutodetect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ClientProtocolSpec(
        _In_ ClientSpec pClientMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ClientProtocolSpec(
        _Out_ ClientSpec* pClientMode) = 0;
};

MIDL_INTERFACE("4247e044-9271-43a9-bc49-e2ad9e855d62")
IMsRdpClient8 : IMsRdpClient7
{
    virtual HRESULT STDMETHODCALLTYPE SendRemoteAction(
        _In_ RemoteSessionActionType actionType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AdvancedSettings9(
        _Out_ struct IMsRdpClientAdvancedSettings8** ppAdvSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reconnect(
        _In_ ULONG ulWidth,
        _In_ ULONG ulHeight,
        _Out_ ControlReconnectStatus* pReconnectStatus) = 0;
};

MIDL_INTERFACE("011c3236-4d81-4515-9143-067ab630d299")
IMsRdpClientTransportSettings4 : IMsRdpClientTransportSettings3
{
    virtual HRESULT STDMETHODCALLTYPE put_GatewayBrokeringType(
        _In_ ULONG _arg1) = 0;
};

MIDL_INTERFACE("28904001-04b6-436c-a55b-0af1a0883dc9")
IMsRdpClient9 : IMsRdpClient8
{
    virtual HRESULT STDMETHODCALLTYPE get_TransportSettings4(
        _Out_ struct IMsRdpClientTransportSettings4** ppXportSet4) = 0;
    virtual HRESULT STDMETHODCALLTYPE SyncSessionDisplaySettings() = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateSessionDisplaySettings(
        _In_ ULONG ulDesktopWidth,
        _In_ ULONG ulDesktopHeight,
        _In_ ULONG ulPhysicalWidth,
        _In_ ULONG ulPhysicalHeight,
        _In_ ULONG ulOrientation,
        _In_ ULONG ulDesktopScaleFactor,
        _In_ ULONG ulDeviceScaleFactor) = 0;
    virtual HRESULT STDMETHODCALLTYPE attachEvent(
        _In_ BSTR eventName,
        _In_ IDispatch* callback) = 0;
    virtual HRESULT STDMETHODCALLTYPE detachEvent(
        _In_ BSTR eventName,
        _In_ IDispatch* callback) = 0;
};

MIDL_INTERFACE("4b84ea77-acea-418c-881a-4a8c28ab1510")
ITSRemoteProgram3 : ITSRemoteProgram2
{
    virtual HRESULT STDMETHODCALLTYPE ServerStartApp(
        _In_ BSTR bstrAppUserModelId,
        _In_ BSTR bstrArguments,
        _In_ VARIANT_BOOL vbExpandEnvVarInArgumentsOnServer) = 0;
};

MIDL_INTERFACE("7ed92c39-eb38-4927-a70a-708ac5a59321")
IMsRdpClient10 : IMsRdpClient9
{
    virtual HRESULT STDMETHODCALLTYPE get_RemoteProgram3(
        _Out_ struct ITSRemoteProgram3** ppRemoteProgram) = 0;
};

MIDL_INTERFACE("05293249-b28b-4bd8-be64-1b2f496b910e")
IMsRdpClientNonScriptable6 : IMsRdpClientNonScriptable5
{
    virtual HRESULT STDMETHODCALLTYPE SendLocation2D(
        _In_ double latitude,
        _In_ double LONGitude) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendLocation3D(
        _In_ double latitude,
        _In_ double LONGitude,
        _In_ int altitude) = 0;
};

MIDL_INTERFACE("09750604-d625-47c1-9fcd-f09f735705d7")
IMsRdpCameraRedirConfig : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_FriendlyName(
        _Out_ BSTR* pFriendlyName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SymbolicLink(
        _Out_ BSTR* pSymbolicLink) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InstanceId(
        _Out_ BSTR* pInstanceId) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ParentInstanceId(
        _Out_ BSTR* pParentInstanceId) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Redirected(
        _In_ VARIANT_BOOL pfRedirected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Redirected(
        _Out_ VARIANT_BOOL* pfRedirected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeviceExists(
        _Out_ VARIANT_BOOL* pfExists) = 0;
};

MIDL_INTERFACE("ae45252b-aaab-4504-b681-649d6073a37a")
IMsRdpCameraRedirConfigCollection : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Rescan() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Count(
        _Out_ ULONG* pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ByIndex(
        _In_ ULONG index,
        _Out_ struct IMsRdpCameraRedirConfig** ppConfig) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BySymbolicLink(
        _In_ BSTR SymbolicLink,
        _Out_ struct IMsRdpCameraRedirConfig** ppConfig) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ByInstanceId(
        _In_ BSTR InstanceId,
        _Out_ struct IMsRdpCameraRedirConfig** ppConfig) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddConfig(
        _In_ BSTR SymbolicLink,
        _In_ VARIANT_BOOL fRedirected) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RedirectByDefault(
        _In_ VARIANT_BOOL pfRedirect) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RedirectByDefault(
        _Out_ VARIANT_BOOL* pfRedirect) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EncodeVideo(
        _In_ VARIANT_BOOL pfEncode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EncodeVideo(
        _Out_ VARIANT_BOOL* pfEncode) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EncodingQuality(
        _In_ CameraRedirEncodingQuality pEncodingQuality) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EncodingQuality(
        _Out_ CameraRedirEncodingQuality* pEncodingQuality) = 0;
};

MIDL_INTERFACE("2e769ee8-00c7-43dc-afd9-235d75b72a40")
IMsRdpClipboard : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CanSyncLocalClipboardToRemoteSession(
        _Out_ VARIANT_BOOL* pfSync) = 0;
    virtual HRESULT STDMETHODCALLTYPE SyncLocalClipboardToRemoteSession() = 0;
    virtual HRESULT STDMETHODCALLTYPE CanSyncRemoteClipboardToLocalSession(
        _Out_ VARIANT_BOOL* pfSync) = 0;
    virtual HRESULT STDMETHODCALLTYPE SyncRemoteClipboardToLocalSession() = 0;
};

MIDL_INTERFACE("71b4a60a-fe21-46d8-a39b-8e32ba0c5ecc")
IMsRdpClientNonScriptable7 : IMsRdpClientNonScriptable6
{
    virtual HRESULT STDMETHODCALLTYPE get_CameraRedirConfigCollection(
        _Out_ struct IMsRdpCameraRedirConfigCollection** ppCameraCollection) = 0;
    virtual HRESULT STDMETHODCALLTYPE DisableDpiCursorScalingForProcess() = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Clipboard(
        _Out_ struct IMsRdpClipboard** ppClipboard) = 0;
};

MIDL_INTERFACE("079863b7-6d47-4105-8bfe-0cdcb360e67d")
IRemoteDesktopClientEvents : IDispatch
{
    // void OnConnecting();
    // void OnConnected();
    // void OnLoginCompleted();
    // void OnDisconnected(
    //     _In_ LONG disconnectReason,
    //     _In_ LONG ExtendedDisconnectReason,
    //     _In_ BSTR disconnectErrorMessage);
    // void OnStatusChanged(
    //     _In_ LONG statusCode,
    //     _In_ BSTR statusMessage);
    // void OnAutoReconnecting(
    //     _In_ LONG disconnectReason,
    //     _In_ LONG ExtendedDisconnectReason,
    //     _In_ BSTR disconnectErrorMessage,
    //     _In_ VARIANT_BOOL networkAvailable,
    //     _In_ LONG attemptCount,
    //     _In_ LONG maxAttemptCount);
    // void OnAutoReconnected();
    // void OnDialogDisplaying();
    // void OnDialogDismissed();
    // void OnNetworkStatusChanged(
    //     _In_ ULONG qualityLevel,
    //     _In_ LONG bandwidth,
    //     _In_ LONG rtt);
    // void OnAdminMessageReceived(
    //     _In_ BSTR adminMessage);
    // void OnKeyCombinationPressed(
    //     _In_ LONG keyCombination);
    // void OnRemoteDesktopSizeChanged(
    //     _In_ LONG width,
    //     _In_ LONG height);
    // void OnTouchPointerCursorMoved(
    //     _In_ LONG x,
    //     _In_ LONG y);
};

MIDL_INTERFACE("48a0f2a7-2713-431f-bbac-6f4558e7d64d")
IRemoteDesktopClientSettings : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE ApplySettings(
        _In_ BSTR RdpFileContents) = 0;
    virtual HRESULT STDMETHODCALLTYPE RetrieveSettings(
        _Out_ BSTR* RdpFileContents) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRdpProperty(
        _In_ BSTR propertyName,
        _Out_ VARIANT* Value) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRdpProperty(
        _In_ BSTR propertyName,
        _In_ VARIANT Value) = 0;
};

MIDL_INTERFACE("7d54bc4e-1028-45d4-8b0a-b9b6bffba176")
IRemoteDesktopClientActions : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE SuspendScreenUpdates() = 0;
    virtual HRESULT STDMETHODCALLTYPE ResumeScreenUpdates() = 0;
    virtual HRESULT STDMETHODCALLTYPE ExecuteRemoteAction(
        _In_ RemoteActionType remoteAction) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSnapshot(
        _In_ SnapshotEncodingType snapshotEncoding,
        _In_ SnapshotFormatType snapshotFormat,
        _In_ ULONG snapshotWidth,
        _In_ ULONG snapshotHeight,
        _Out_ BSTR* snapshotData) = 0;
};

MIDL_INTERFACE("260ec22d-8cbc-44b5-9e88-2a37f6c93ae9")
IRemoteDesktopClientTouchPointer : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(
        _In_ VARIANT_BOOL Enabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(
        _Out_ VARIANT_BOOL* Enabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EventsEnabled(
        _In_ VARIANT_BOOL EventsEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EventsEnabled(
        _Out_ VARIANT_BOOL* EventsEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PointerSpeed(
        _In_ ULONG PointerSpeed) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PointerSpeed(
        _Out_ ULONG* PointerSpeed) = 0;
};

MIDL_INTERFACE("57d25668-625a-4905-be4e-304caa13f89c")
IRemoteDesktopClient : IDispatch
{
    virtual HRESULT STDMETHODCALLTYPE Connect() = 0;
    virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
    virtual HRESULT STDMETHODCALLTYPE Reconnect(
        _In_ ULONG width,
        _In_ ULONG height) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Settings(
        _Out_ struct IRemoteDesktopClientSettings** Settings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Actions(
        _Out_ struct IRemoteDesktopClientActions** Actions) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TouchPointer(
        _Out_ struct IRemoteDesktopClientTouchPointer** TouchPointer) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteSavedCredentials(
        _In_ BSTR serverName) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateSessionDisplaySettings(
        _In_ ULONG width,
        _In_ ULONG height) = 0;
    virtual HRESULT STDMETHODCALLTYPE attachEvent(
        _In_ BSTR eventName,
        _In_ IDispatch* callback) = 0;
    virtual HRESULT STDMETHODCALLTYPE detachEvent(
        _In_ BSTR eventName,
        _In_ IDispatch* callback) = 0;
};

//
// Named GUID constants initializations
//

EXTERN_GUID(
    LIBID_MSTSCLib,
    0x8c11efa1, 0x92c3, 0x11d1, 0xbc, 0x1e, 0x00, 0xc0, 0x4f, 0xa3, 0x14, 0x89);
EXTERN_GUID(
    DIID_IMsTscAxEvents,
    0x336d5562, 0xefa8, 0x482e, 0x8c, 0xb3, 0xc5, 0xc0, 0xfc, 0x7a, 0x7d, 0xb6);
EXTERN_GUID(
    CLSID_MsTscAxNotSafeForScripting,
    0xa41a4187, 0x5a86, 0x4e26, 0xb4, 0x0a, 0x85, 0x6f, 0x90, 0x35, 0xd9, 0xcb);
EXTERN_GUID(
    IID_IMsTscAx_Redist,
    0x327bb5cd, 0x834e, 0x4400, 0xae, 0xf2, 0xb3, 0x0e, 0x15, 0xe5, 0xd6, 0x82);
EXTERN_GUID(
    IID_IMsTscSecuredSettings,
    0xc9d65442, 0xa0f9, 0x45b2, 0x8f, 0x73, 0xd6, 0x1d, 0x2d, 0xb8, 0xcb, 0xb6);
EXTERN_GUID(
    IID_IMsTscAdvancedSettings,
    0x809945cc, 0x4b3b, 0x4a92, 0xa6, 0xb0, 0xdb, 0xf9, 0xb5, 0xf2, 0xef, 0x2d);
EXTERN_GUID(
    IID_IMsTscDebug,
    0x209d0eb9, 0x6254, 0x47b1, 0x90, 0x33, 0xa9, 0x8d, 0xae, 0x55, 0xbb, 0x27);
EXTERN_GUID(
    IID_IMsTscAx,
    0x8c11efae, 0x92c3, 0x11d1, 0xbc, 0x1e, 0x00, 0xc0, 0x4f, 0xa3, 0x14, 0x89);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings,
    0x3c65b4ab, 0x12b3, 0x465b, 0xac, 0xd4, 0xb8, 0xda, 0xd3, 0xbf, 0xf9, 0xe2);
EXTERN_GUID(
    IID_IMsRdpClientSecuredSettings,
    0x605befcf, 0x39c1, 0x45cc, 0xa8, 0x11, 0x06, 0x8f, 0xb7, 0xbe, 0x34, 0x6d);
EXTERN_GUID(
    IID_IMsRdpClient,
    0x92b4a539, 0x7115, 0x4b7c, 0xa5, 0xa9, 0xe5, 0xd9, 0xef, 0xc2, 0x78, 0x0a);
EXTERN_GUID(
    IID_IMsTscNonScriptable,
    0xc1e6743a, 0x41c1, 0x4a74, 0x83, 0x2a, 0x0d, 0xd0, 0x6c, 0x1c, 0x7a, 0x0e);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable,
    0x2f079c4c, 0x87b2, 0x4afd, 0x97, 0xab, 0x20, 0xcd, 0xb4, 0x30, 0x38, 0xae);
EXTERN_GUID(
    CLSID_MsTscAx,
    0x1fb464c8, 0x09bb, 0x4017, 0xa2, 0xf5, 0xeb, 0x74, 0x2f, 0x04, 0x39, 0x2f);
EXTERN_GUID(
    CLSID_MsRdpClientNotSafeForScripting,
    0x7cacbd7b, 0x0d99, 0x468f, 0xac, 0x33, 0x22, 0xe4, 0x95, 0xc0, 0xaf, 0xe5);
EXTERN_GUID(
    CLSID_MsRdpClient,
    0x791fa017, 0x2de3, 0x492e, 0xac, 0xc5, 0x53, 0xc6, 0x7a, 0x2b, 0x94, 0xd0);
EXTERN_GUID(
    CLSID_MsRdpClient2NotSafeForScripting,
    0x3523c2fb, 0x4031, 0x44e4, 0x9a, 0x3b, 0xf1, 0xe9, 0x49, 0x86, 0xee, 0x7f);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings2,
    0x9ac42117, 0x2b76, 0x4320, 0xaa, 0x44, 0x0e, 0x61, 0x6a, 0xb8, 0x43, 0x7b);
EXTERN_GUID(
    IID_IMsRdpClient2,
    0xe7e17dc4, 0x3b71, 0x4ba7, 0xa8, 0xe6, 0x28, 0x1f, 0xfa, 0xdc, 0xa2, 0x8f);
EXTERN_GUID(
    CLSID_MsRdpClient2,
    0x9059f30f, 0x4eb1, 0x4bd2, 0x9f, 0xdc, 0x36, 0xf4, 0x3a, 0x21, 0x8f, 0x4a);
EXTERN_GUID(
    CLSID_MsRdpClient2a,
    0x971127bb, 0x259f, 0x48c2, 0xbd, 0x75, 0x5f, 0x97, 0xa3, 0x33, 0x15, 0x51);
EXTERN_GUID(
    CLSID_MsRdpClient3NotSafeForScripting,
    0xace575fd, 0x1fcf, 0x4074, 0x94, 0x01, 0xeb, 0xab, 0x99, 0x0f, 0xa9, 0xde);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings3,
    0x19cd856b, 0xc542, 0x4c53, 0xac, 0xee, 0xf1, 0x27, 0xe3, 0xbe, 0x1a, 0x59);
EXTERN_GUID(
    IID_IMsRdpClient3,
    0x91b7cbc5, 0xa72e, 0x4fa0, 0x93, 0x00, 0xd6, 0x47, 0xd7, 0xe8, 0x97, 0xff);
EXTERN_GUID(
    CLSID_MsRdpClient3,
    0x7584c670, 0x2274, 0x4efb, 0xb0, 0x0b, 0xd6, 0xaa, 0xba, 0x6d, 0x38, 0x50);
EXTERN_GUID(
    CLSID_MsRdpClient3a,
    0x6a6f4b83, 0x45c5, 0x4ca9, 0xbd, 0xd9, 0x0d, 0x81, 0xc1, 0x22, 0x95, 0xe4);
EXTERN_GUID(
    CLSID_MsRdpClient4NotSafeForScripting,
    0x6ae29350, 0x321b, 0x42be, 0xbb, 0xe5, 0x12, 0xfb, 0x52, 0x70, 0xc0, 0xde);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings4,
    0xfba7f64e, 0x7345, 0x4405, 0xae, 0x50, 0xfa, 0x4a, 0x76, 0x3d, 0xc0, 0xde);
EXTERN_GUID(
    IID_IMsRdpClient4,
    0x095e0738, 0xd97d, 0x488b, 0xb9, 0xf6, 0xdd, 0x0e, 0x8d, 0x66, 0xc0, 0xde);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable2,
    0x17a5e535, 0x4072, 0x4fa4, 0xaf, 0x32, 0xc8, 0xd0, 0xd4, 0x73, 0x45, 0xe9);
EXTERN_GUID(
    CLSID_MsRdpClient4,
    0x4edcb26c, 0xd24c, 0x4e72, 0xaf, 0x07, 0xb5, 0x76, 0x69, 0x9a, 0xc0, 0xde);
EXTERN_GUID(
    CLSID_MsRdpClient4a,
    0x54ce37e0, 0x9834, 0x41ae, 0x98, 0x96, 0x4d, 0xab, 0x69, 0xdc, 0x02, 0x2b);
EXTERN_GUID(
    CLSID_MsRdpClient5NotSafeForScripting,
    0x4eb2f086, 0xc818, 0x447e, 0xb3, 0x2c, 0xc5, 0x1c, 0xe2, 0xb3, 0x0d, 0x31);
EXTERN_GUID(
    IID_IMsRdpClientTransportSettings,
    0x720298c0, 0xa099, 0x46f5, 0x9f, 0x82, 0x96, 0x92, 0x1b, 0xae, 0x47, 0x01);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings5,
    0xfba7f64e, 0x6783, 0x4405, 0xda, 0x45, 0xfa, 0x4a, 0x76, 0x3d, 0xab, 0xd0);
EXTERN_GUID(
    IID_ITSRemoteProgram,
    0xfdd029f9, 0x467a, 0x4c49, 0x85, 0x29, 0x64, 0xb5, 0x21, 0xdb, 0xd1, 0xb4);
EXTERN_GUID(
    IID_IMsRdpClientShell,
    0xd012ae6d, 0xc19a, 0x4bfe, 0xb3, 0x67, 0x20, 0x1f, 0x89, 0x11, 0xf1, 0x34);
EXTERN_GUID(
    IID_IMsRdpClient5,
    0x4eb5335b, 0x6429, 0x477d, 0xb9, 0x22, 0xe0, 0x6a, 0x28, 0xec, 0xd8, 0xbf);
EXTERN_GUID(
    IID_IMsRdpDevice,
    0x60c3b9c8, 0x9e92, 0x4f5e, 0xa3, 0xe7, 0x60, 0x4a, 0x91, 0x20, 0x93, 0xea);
EXTERN_GUID(
    IID_IMsRdpDeviceCollection,
    0x56540617, 0xd281, 0x488c, 0x87, 0x38, 0x6a, 0x8f, 0xdf, 0x64, 0xa1, 0x18);
EXTERN_GUID(
    IID_IMsRdpDrive,
    0xd28b5458, 0xf694, 0x47a8, 0x8e, 0x61, 0x40, 0x35, 0x6a, 0x76, 0x7e, 0x46);
EXTERN_GUID(
    IID_IMsRdpDriveCollection,
    0x7ff17599, 0xda2c, 0x4677, 0xad, 0x35, 0xf6, 0x0c, 0x04, 0xfe, 0x15, 0x85);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable3,
    0xb3378d90, 0x0728, 0x45c7, 0x8e, 0xd7, 0xb6, 0x15, 0x9f, 0xb9, 0x22, 0x19);
EXTERN_GUID(
    CLSID_MsRdpClient5,
    0x4eb89ff4, 0x7f78, 0x4a0f, 0x8b, 0x8d, 0x2b, 0xf0, 0x2e, 0x94, 0xe4, 0xb2);
EXTERN_GUID(
    CLSID_MsRdpClient6NotSafeForScripting,
    0xd2ea46a7, 0xc2bf, 0x426b, 0xaf, 0x24, 0xe1, 0x9c, 0x44, 0x45, 0x63, 0x99);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings6,
    0x222c4b5d, 0x45d9, 0x4df0, 0xa7, 0xc6, 0x60, 0xcf, 0x90, 0x89, 0xd2, 0x85);
EXTERN_GUID(
    IID_IMsRdpClientTransportSettings2,
    0x67341688, 0xd606, 0x4c73, 0xa5, 0xd2, 0x2e, 0x04, 0x89, 0x00, 0x93, 0x19);
EXTERN_GUID(
    IID_IMsRdpClient6,
    0xd43b7d80, 0x8517, 0x4b6d, 0x9e, 0xac, 0x96, 0xad, 0x68, 0x00, 0xd7, 0xf2);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable4,
    0xf50fa8aa, 0x1c7d, 0x4f59, 0xb1, 0x5c, 0xa9, 0x0c, 0xac, 0xae, 0x1f, 0xcb);
EXTERN_GUID(
    CLSID_MsRdpClient6,
    0x7390f3d8, 0x0439, 0x4c05, 0x91, 0xe3, 0xcf, 0x5c, 0xb2, 0x90, 0xc3, 0xd0);
EXTERN_GUID(
    CLSID_MsRdpClient7NotSafeForScripting,
    0x54d38bf7, 0xb1ef, 0x4479, 0x96, 0x74, 0x1b, 0xd6, 0xea, 0x46, 0x52, 0x58);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings7,
    0x26036036, 0x4010, 0x4578, 0x80, 0x91, 0x0d, 0xb9, 0xa1, 0xed, 0xf9, 0xc3);
EXTERN_GUID(
    IID_IMsRdpClientTransportSettings3,
    0x3d5b21ac, 0x748d, 0x41de, 0x8f, 0x30, 0xe1, 0x51, 0x69, 0x58, 0x6b, 0xd4);
EXTERN_GUID(
    IID_IMsRdpClientSecuredSettings2,
    0x25f2ce20, 0x8b1d, 0x4971, 0xa7, 0xcd, 0x54, 0x9d, 0xae, 0x20, 0x1f, 0xc0);
EXTERN_GUID(
    IID_ITSRemoteProgram2,
    0x92c38a7d, 0x241a, 0x418c, 0x99, 0x36, 0x09, 0x98, 0x72, 0xc9, 0xaf, 0x20);
EXTERN_GUID(
    IID_IMsRdpClient7,
    0xb2a5b5ce, 0x3461, 0x444a, 0x91, 0xd4, 0xad, 0xd2, 0x6d, 0x07, 0x06, 0x38);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable5,
    0x4f6996d5, 0xd7b1, 0x412c, 0xb0, 0xff, 0x06, 0x37, 0x18, 0x56, 0x69, 0x07);
EXTERN_GUID(
    IID_IMsRdpPreferredRedirectionInfo,
    0xfdd029f9, 0x9574, 0x4def, 0x85, 0x29, 0x64, 0xb5, 0x21, 0xcc, 0xca, 0xa4);
EXTERN_GUID(
    IID_IMsRdpExtendedSettings,
    0x302d8188, 0x0052, 0x4807, 0x80, 0x6a, 0x36, 0x2b, 0x62, 0x8f, 0x9a, 0xc5);
EXTERN_GUID(
    CLSID_MsRdpClient7,
    0xa9d7038d, 0xb5ed, 0x472e, 0x9c, 0x47, 0x94, 0xbe, 0xa9, 0x0a, 0x59, 0x10);
EXTERN_GUID(
    CLSID_MsRdpClient8NotSafeForScripting,
    0xa3bc03a0, 0x041d, 0x42e3, 0xad, 0x22, 0x88, 0x2b, 0x78, 0x65, 0xc9, 0xc5);
EXTERN_GUID(
    IID_IMsRdpClientAdvancedSettings8,
    0x89acb528, 0x2557, 0x4d16, 0x86, 0x25, 0x22, 0x6a, 0x30, 0xe9, 0x7e, 0x9a);
EXTERN_GUID(
    IID_IMsRdpClient8,
    0x4247e044, 0x9271, 0x43a9, 0xbc, 0x49, 0xe2, 0xad, 0x9e, 0x85, 0x5d, 0x62);
EXTERN_GUID(
    CLSID_MsRdpClient8,
    0x5f681803, 0x2900, 0x4c43, 0xa1, 0xcc, 0xcf, 0x40, 0x54, 0x04, 0xa6, 0x76);
EXTERN_GUID(
    CLSID_MsRdpClient9NotSafeForScripting,
    0x8b918b82, 0x7985, 0x4c24, 0x89, 0xdf, 0xc3, 0x3a, 0xd2, 0xbb, 0xfb, 0xcd);
EXTERN_GUID(
    IID_IMsRdpClientTransportSettings4,
    0x011c3236, 0x4d81, 0x4515, 0x91, 0x43, 0x06, 0x7a, 0xb6, 0x30, 0xd2, 0x99);
EXTERN_GUID(
    IID_IMsRdpClient9,
    0x28904001, 0x04b6, 0x436c, 0xa5, 0x5b, 0x0a, 0xf1, 0xa0, 0x88, 0x3d, 0xc9);
EXTERN_GUID(
    CLSID_MsRdpClient9,
    0x301b94ba, 0x5d25, 0x4a12, 0xbf, 0xfe, 0x3b, 0x6e, 0x7a, 0x61, 0x65, 0x85);
EXTERN_GUID(
    CLSID_MsRdpClient10NotSafeForScripting,
    0xa0c63c30, 0xf08d, 0x4ab4, 0x90, 0x7c, 0x34, 0x90, 0x5d, 0x77, 0x0c, 0x7d);
EXTERN_GUID(
    IID_ITSRemoteProgram3,
    0x4b84ea77, 0xacea, 0x418c, 0x88, 0x1a, 0x4a, 0x8c, 0x28, 0xab, 0x15, 0x10);
EXTERN_GUID(
    IID_IMsRdpClient10,
    0x7ed92c39, 0xeb38, 0x4927, 0xa7, 0x0a, 0x70, 0x8a, 0xc5, 0xa5, 0x93, 0x21);
EXTERN_GUID(
    CLSID_MsRdpClient10,
    0xc0efa91a, 0xeeb7, 0x41c7, 0x97, 0xfa, 0xf0, 0xed, 0x64, 0x5e, 0xfb, 0x24);
EXTERN_GUID(
    CLSID_MsRdpClient11NotSafeForScripting,
    0x1df7c823, 0xb2d4, 0x4b54, 0x97, 0x5a, 0xf2, 0xac, 0x5d, 0x7c, 0xf8, 0xb8);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable6,
    0x05293249, 0xb28b, 0x4bd8, 0xbe, 0x64, 0x1b, 0x2f, 0x49, 0x6b, 0x91, 0x0e);
EXTERN_GUID(
    IID_IMsRdpCameraRedirConfig,
    0x09750604, 0xd625, 0x47c1, 0x9f, 0xcd, 0xf0, 0x9f, 0x73, 0x57, 0x05, 0xd7);
EXTERN_GUID(
    IID_IMsRdpCameraRedirConfigCollection,
    0xae45252b, 0xaaab, 0x4504, 0xb6, 0x81, 0x64, 0x9d, 0x60, 0x73, 0xa3, 0x7a);
EXTERN_GUID(
    IID_IMsRdpClipboard,
    0x2e769ee8, 0x00c7, 0x43dc, 0xaf, 0xd9, 0x23, 0x5d, 0x75, 0xb7, 0x2a, 0x40);
EXTERN_GUID(
    IID_IMsRdpClientNonScriptable7,
    0x71b4a60a, 0xfe21, 0x46d8, 0xa3, 0x9b, 0x8e, 0x32, 0xba, 0x0c, 0x5e, 0xcc);
EXTERN_GUID(
    CLSID_MsRdpClient11,
    0x22a7e88c, 0x5bf5, 0x4de6, 0xb6, 0x87, 0x60, 0xf7, 0x33, 0x1d, 0xf1, 0x90);
EXTERN_GUID(
    DIID_IRemoteDesktopClientEvents,
    0x079863b7, 0x6d47, 0x4105, 0x8b, 0xfe, 0x0c, 0xdc, 0xb3, 0x60, 0xe6, 0x7d);
EXTERN_GUID(
    CLSID_RemoteDesktopClient,
    0xeab16c5d, 0xeed1, 0x4e95, 0x86, 0x8b, 0x0f, 0xba, 0x1b, 0x42, 0xc0, 0x92);
EXTERN_GUID(
    IID_IRemoteDesktopClientSettings,
    0x48a0f2a7, 0x2713, 0x431f, 0xbb, 0xac, 0x6f, 0x45, 0x58, 0xe7, 0xd6, 0x4d);
EXTERN_GUID(
    IID_IRemoteDesktopClientActions,
    0x7d54bc4e, 0x1028, 0x45d4, 0x8b, 0x0a, 0xb9, 0xb6, 0xbf, 0xfb, 0xa1, 0x76);
EXTERN_GUID(
    IID_IRemoteDesktopClientTouchPointer,
    0x260ec22d, 0x8cbc, 0x44b5, 0x9e, 0x88, 0x2a, 0x37, 0xf6, 0xc9, 0x3a, 0xe9);
EXTERN_GUID(
    IID_IRemoteDesktopClient,
    0x57d25668, 0x625a, 0x4905, 0xbe, 0x4e, 0x30, 0x4c, 0xaa, 0x13, 0xf8, 0x9c);

#endif // !MSTSCAX_H
