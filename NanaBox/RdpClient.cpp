/*
 * PROJECT:   NanaBox
 * FILE:      RdpClient.cpp
 * PURPOSE:   Implementation for the Remote Desktop client C++/WinRT wrapper
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "RdpClient.h"

namespace winrt
{
    struct bstr_traits
    {
        using type = BSTR;

        static void close(type value) noexcept
        {
            ::SysFreeString(value);
        }

        static constexpr type invalid() noexcept
        {
            return nullptr;
        }
    };

    using bstr = handle_type<bstr_traits>;
}

NanaBox::RdpDevice::RdpDevice(
    winrt::com_ptr<IMsRdpDevice> const& Instance)
    : m_Instance(Instance.as<IMsRdpDeviceV2>())
{

}

winrt::hstring NanaBox::RdpDevice::DeviceInstanceId()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Instance->get_DeviceInstanceId(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

winrt::hstring NanaBox::RdpDevice::FriendlyName()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Instance->get_FriendlyName(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

winrt::hstring NanaBox::RdpDevice::DeviceDescription()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Instance->get_DeviceDescription(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

bool NanaBox::RdpDevice::RedirectionState()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_Instance->get_RedirectionState(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpDevice::RedirectionState(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_Instance->put_RedirectionState(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

winrt::hstring NanaBox::RdpDevice::DeviceText()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Instance->get_DeviceText(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

bool NanaBox::RdpDevice::IsUSBDevice()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_Instance->get_IsUSBDevice(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpDevice::IsCompositeDevice()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_Instance->get_IsCompositeDevice(
            &RawValue));
    return RawValue;
}

ULONG NanaBox::RdpDevice::DriveLetterBitmap()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_Instance->get_DriveLetterBitmap(
            &RawValue));
    return RawValue;
}

NanaBox::RdpDrive::RdpDrive(
    winrt::com_ptr<IMsRdpDrive> const& Instance)
    : m_Instance(Instance.as<IMsRdpDriveV2>())
{

}

winrt::hstring NanaBox::RdpDrive::Name()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Instance->get_Name(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

bool NanaBox::RdpDrive::RedirectionState()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_Instance->get_RedirectionState(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpDrive::RedirectionState(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_Instance->put_RedirectionState(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

ULONG NanaBox::RdpDrive::DriveLetterIndex()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_Instance->get_DriveLetterIndex(
            &RawValue));
    return RawValue;
}

NanaBox::RdpClient::RdpClient()
{
    this->m_RdpClient = winrt::create_instance<IMsRdpClient9>(
        CLSID_MsRdpClient9NotSafeForScripting);

    winrt::check_hresult(this->m_RdpClient->get_SecuredSettings3(
        this->m_SecuredSettings.put()));

    winrt::check_hresult(this->m_RdpClient->get_AdvancedSettings9(
        this->m_AdvancedSettings.put()));

    winrt::check_hresult(this->m_RdpClient->get_TransportSettings4(
        this->m_TransportSettings.put()));

    winrt::check_hresult(this->m_RdpClient->get_RemoteProgram2(
        this->m_RemoteProgram.put()));

    winrt::check_hresult(this->m_RdpClient->get_MsRdpClientShell(
        this->m_Shell.put()));

    this->m_RedirectionInfo =
        this->m_RdpClient.as<IMsRdpPreferredRedirectionInfo>();

    this->m_ExtendedSettings =
        this->m_RdpClient.as<IMsRdpExtendedSettings>();

    this->m_NonScriptable =
        this->m_RdpClient.as<IMsRdpClientNonScriptable5>();

    winrt::check_hresult(
        this->m_NonScriptable->get_DeviceCollection(
            this->m_DeviceCollection.put()));

    winrt::check_hresult(
        this->m_NonScriptable->get_DriveCollection(
            this->m_DriveCollection.put()));

    winrt::com_ptr<IConnectionPointContainer> ConnectionPointContainer =
        this->m_RdpClient.as<IConnectionPointContainer>();
    winrt::check_hresult(ConnectionPointContainer->FindConnectionPoint(
        DIID_IMsTscAxEvents,
        this->m_ConnectionPoint.put()));

    winrt::check_hresult(this->m_ConnectionPoint->Advise(
        this,
        &this->m_Cookie));
}

NanaBox::RdpClient::~RdpClient()
{
    winrt::check_hresult(this->m_ConnectionPoint->Unadvise(
        this->m_Cookie));
}

winrt::com_ptr<IMsRdpClient9> NanaBox::RdpClient::RawControl()
{
    return this->m_RdpClient;
}

#pragma region RdpClient

winrt::hstring NanaBox::RdpClient::Server()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_Server(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::Server(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_Server(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::Domain()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_Domain(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::Domain(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_Domain(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::UserName()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_UserName(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::UserName(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_UserName(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::DisconnectedText()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_DisconnectedText(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::DisconnectedText(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_DisconnectedText(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::ConnectingText()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_ConnectingText(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::ConnectingText(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_ConnectingText(
            RawValue.get()));
}

short NanaBox::RdpClient::Connected()
{
    short RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_Connected(
            &RawValue));
    return RawValue;
}

LONG NanaBox::RdpClient::DesktopWidth()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_DesktopWidth(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DesktopWidth(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_RdpClient->put_DesktopWidth(
            Value));
}

LONG NanaBox::RdpClient::DesktopHeight()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_DesktopHeight(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DesktopHeight(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_RdpClient->put_DesktopHeight(
            Value));
}

bool NanaBox::RdpClient::StartConnected()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_StartConnected(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::StartConnected(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_RdpClient->put_StartConnected(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::HorizontalScrollBarVisible()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_HorizontalScrollBarVisible(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpClient::VerticalScrollBarVisible()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_VerticalScrollBarVisible(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::FullScreenTitle(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_FullScreenTitle(
            RawValue.get()));
}

LONG NanaBox::RdpClient::CipherStrength()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_CipherStrength(
            &RawValue));
    return RawValue;
}

winrt::hstring NanaBox::RdpClient::Version()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_Version(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

bool NanaBox::RdpClient::SecuredSettingsEnabled()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_SecuredSettingsEnabled(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::Connect()
{
    winrt::check_hresult(
        this->m_RdpClient->Connect());
}

void NanaBox::RdpClient::Disconnect()
{
    winrt::check_hresult(
        this->m_RdpClient->Disconnect());
}

void NanaBox::RdpClient::CreateVirtualChannels(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->CreateVirtualChannels(
            RawValue.get()));
}

void NanaBox::RdpClient::SendOnVirtualChannel(
    winrt::hstring const& Name,
    winrt::hstring const& Data)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::bstr RawData;
    RawData.attach(::SysAllocString(Data.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->SendOnVirtualChannel(
            RawName.get(),
            RawData.get()));
}

LONG NanaBox::RdpClient::ColorDepth()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_ColorDepth(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ColorDepth(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_RdpClient->put_ColorDepth(
            Value));
}

ExtendedDisconnectReasonCode NanaBox::RdpClient::ExtendedDisconnectReason()
{
    ExtendedDisconnectReasonCode RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_ExtendedDisconnectReason(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpClient::FullScreen()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_FullScreen(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::FullScreen(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_RdpClient->put_FullScreen(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

void NanaBox::RdpClient::SetVirtualChannelOptions(
    winrt::hstring const& Name,
    LONG Options)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->SetVirtualChannelOptions(
            RawName.get(),
            Options));
}

LONG NanaBox::RdpClient::GetVirtualChannelOptions(
    winrt::hstring const& Name)
{
    LONG Options;
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->GetVirtualChannelOptions(
            RawName.get(),
            &Options));
    return Options;
}

ControlCloseStatus NanaBox::RdpClient::RequestClose()
{
    ControlCloseStatus CloseStatus;
    winrt::check_hresult(
        this->m_RdpClient->RequestClose(
            &CloseStatus));
    return CloseStatus;
}

winrt::hstring NanaBox::RdpClient::ConnectedStatusText()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->get_ConnectedStatusText(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::ConnectedStatusText(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->put_ConnectedStatusText(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::GetErrorDescription(
    UINT DisconnectReason,
    UINT ExtendedDisconnectReason)
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->GetErrorDescription(
            DisconnectReason,
            ExtendedDisconnectReason,
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

winrt::hstring NanaBox::RdpClient::GetStatusText(
    UINT StatusCode)
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_RdpClient->GetStatusText(
            StatusCode,
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::SendRemoteAction(
    RemoteSessionActionType ActionType)
{
    winrt::check_hresult(
        this->m_RdpClient->SendRemoteAction(
            ActionType));
}

ControlReconnectStatus NanaBox::RdpClient::Reconnect(
    ULONG Width,
    ULONG Height)
{
    ControlReconnectStatus RawValue;
    winrt::check_hresult(
        this->m_RdpClient->Reconnect(
            Width,
            Height,
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::SyncSessionDisplaySettings()
{
    winrt::check_hresult(
        this->m_RdpClient->SyncSessionDisplaySettings());
}

void NanaBox::RdpClient::UpdateSessionDisplaySettings(
    ULONG DesktopWidth,
    ULONG DesktopHeight,
    ULONG PhysicalWidth,
    ULONG PhysicalHeight,
    ULONG Orientation,
    ULONG DesktopScaleFactor,
    ULONG DeviceScaleFactor)
{
    winrt::check_hresult(
        this->m_RdpClient->UpdateSessionDisplaySettings(
            DesktopWidth,
            DesktopHeight,
            PhysicalWidth,
            PhysicalHeight,
            Orientation,
            DesktopScaleFactor,
            DeviceScaleFactor));
}

void NanaBox::RdpClient::AttachEvent(
    winrt::hstring const& Name,
    winrt::com_ptr<IDispatch> const& Callback)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->attachEvent(
            RawName.get(),
            Callback.get()));
}

void NanaBox::RdpClient::DetachEvent(
    winrt::hstring const& Name,
    winrt::com_ptr<IDispatch> const& Callback)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_RdpClient->detachEvent(
            RawName.get(),
            Callback.get()));
}

#pragma endregion

#pragma region SecuredSettings

winrt::hstring NanaBox::RdpClient::StartProgram()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_SecuredSettings->get_StartProgram(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::StartProgram(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_SecuredSettings->put_StartProgram(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::WorkDir()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_SecuredSettings->get_WorkDir(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::WorkDir(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_SecuredSettings->put_WorkDir(
            RawValue.get()));
}

LONG NanaBox::RdpClient::KeyboardHookMode()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_SecuredSettings->get_KeyboardHookMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::KeyboardHookMode(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_SecuredSettings->put_KeyboardHookMode(
            Value));
}

LONG NanaBox::RdpClient::AudioRedirectionMode()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_SecuredSettings->get_AudioRedirectionMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AudioRedirectionMode(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_SecuredSettings->put_AudioRedirectionMode(
            Value));
}

winrt::hstring NanaBox::RdpClient::PCB()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_SecuredSettings->get_PCB(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::PCB(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_SecuredSettings->put_PCB(
            RawValue.get()));
}

#pragma endregion

#pragma region AdvancedSettings

bool NanaBox::RdpClient::Compress()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_Compress(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::Compress(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_Compress(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::BitmapPeristence()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapPeristence(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapPeristence(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapPeristence(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::AllowBackgroundInput()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_allowBackgroundInput(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AllowBackgroundInput(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_allowBackgroundInput(
            Value ? TRUE : FALSE));
}

void NanaBox::RdpClient::KeyBoardLayoutStr(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_AdvancedSettings->put_KeyBoardLayoutStr(
            RawValue.get()));
}

void NanaBox::RdpClient::PluginDlls(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_AdvancedSettings->put_PluginDlls(
            RawValue.get()));
}

void NanaBox::RdpClient::IconFile(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_AdvancedSettings->put_IconFile(
            RawValue.get()));
}

void NanaBox::RdpClient::IconIndex(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_IconIndex(
            Value));
}

bool NanaBox::RdpClient::ContainerHandledFullScreen()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ContainerHandledFullScreen(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ContainerHandledFullScreen(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ContainerHandledFullScreen(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::DisableRdpdr()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_DisableRdpdr(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DisableRdpdr(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_DisableRdpdr(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::AcceleratorPassthrough()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AcceleratorPassthrough(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AcceleratorPassthrough(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_AcceleratorPassthrough(
            Value ? TRUE : FALSE));
}

LONG NanaBox::RdpClient::SasSequence()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_SasSequence(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::SasSequence(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_SasSequence(
            Value));
}

LONG NanaBox::RdpClient::RDPPort()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RDPPort(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RDPPort(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RDPPort(
            Value));
}

bool NanaBox::RdpClient::DisableCtrlAltDel()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_DisableCtrlAltDel(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DisableCtrlAltDel(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_DisableCtrlAltDel(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::EnableWindowsKey()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_EnableWindowsKey(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::EnableWindowsKey(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_EnableWindowsKey(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::DoubleClickDetect()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_DoubleClickDetect(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DoubleClickDetect(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_DoubleClickDetect(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::MaximizeShell()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_MaximizeShell(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::MaximizeShell(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_MaximizeShell(
            Value ? TRUE : FALSE));
}

LONG NanaBox::RdpClient::HotKeyFullScreen()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyFullScreen(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyFullScreen(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyFullScreen(
            Value));
}

LONG NanaBox::RdpClient::HotKeyCtrlEsc()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyCtrlEsc(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyCtrlEsc(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyCtrlEsc(
            Value));
}

LONG NanaBox::RdpClient::HotKeyAltEsc()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyAltEsc(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyAltEsc(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyAltEsc(
            Value));
}

LONG NanaBox::RdpClient::HotKeyAltTab()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyAltTab(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyAltTab(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyAltTab(
            Value));
}

LONG NanaBox::RdpClient::HotKeyAltShiftTab()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyAltShiftTab(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyAltShiftTab(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyAltShiftTab(
            Value));
}

LONG NanaBox::RdpClient::HotKeyAltSpace()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyAltSpace(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyAltSpace(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyAltSpace(
            Value));
}

LONG NanaBox::RdpClient::HotKeyCtrlAltDel()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyCtrlAltDel(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyCtrlAltDel(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyCtrlAltDel(
            Value));
}

LONG NanaBox::RdpClient::BitmapCacheSize()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapCacheSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapCacheSize(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapCacheSize(
            Value));
}

LONG NanaBox::RdpClient::BitmapVirtualCacheSize()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapVirtualCacheSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapVirtualCacheSize(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapVirtualCacheSize(
            Value));
}

bool NanaBox::RdpClient::ScaleBitmapCachesByBPP()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ScaleBitmapCachesByBPP(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ScaleBitmapCachesByBPP(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ScaleBitmapCachesByBPP(
            Value ? TRUE : FALSE));
}

bool NanaBox::RdpClient::CachePersistenceActive()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_CachePersistenceActive(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::CachePersistenceActive(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_CachePersistenceActive(
            Value ? TRUE : FALSE));
}

LONG NanaBox::RdpClient::MinInputSendInterval()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_minInputSendInterval(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::MinInputSendInterval(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_minInputSendInterval(
            Value));
}

LONG NanaBox::RdpClient::KeepAliveInterval()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_keepAliveInterval(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::KeepAliveInterval(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_keepAliveInterval(
            Value));
}

LONG NanaBox::RdpClient::ShutdownTimeout()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_shutdownTimeout(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ShutdownTimeout(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_shutdownTimeout(
            Value));
}

LONG NanaBox::RdpClient::OverallConnectionTimeout()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_overallConnectionTimeout(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::OverallConnectionTimeout(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_overallConnectionTimeout(
            Value));
}

LONG NanaBox::RdpClient::SingleConnectionTimeout()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_singleConnectionTimeout(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::SingleConnectionTimeout(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_singleConnectionTimeout(
            Value));
}

bool NanaBox::RdpClient::BitmapPersistence()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapPersistence(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapPersistence(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapPersistence(
            Value ? TRUE : FALSE));
}

LONG NanaBox::RdpClient::MinutesToIdleTimeout()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_MinutesToIdleTimeout(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::MinutesToIdleTimeout(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_MinutesToIdleTimeout(
            Value));
}

bool NanaBox::RdpClient::SmartSizing()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_SmartSizing(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::SmartSizing(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_SmartSizing(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::DisplayConnectionBar()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_DisplayConnectionBar(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DisplayConnectionBar(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_DisplayConnectionBar(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::PinConnectionBar()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_PinConnectionBar(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PinConnectionBar(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_PinConnectionBar(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::GrabFocusOnConnect()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_GrabFocusOnConnect(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GrabFocusOnConnect(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_GrabFocusOnConnect(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

winrt::hstring NanaBox::RdpClient::LoadBalanceInfo()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_LoadBalanceInfo(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::LoadBalanceInfo(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_AdvancedSettings->put_LoadBalanceInfo(
            RawValue.get()));
}

bool NanaBox::RdpClient::RedirectDrives()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectDrives(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectDrives(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectDrives(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectPrinters()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectPrinters(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectPrinters(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectPrinters(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectPorts()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectPorts(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectPorts(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectPorts(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectSmartCards()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectSmartCards(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectSmartCards(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectSmartCards(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

LONG NanaBox::RdpClient::BitmapVirtualCache16BppSize()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapVirtualCache16BppSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapVirtualCache16BppSize(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapVirtualCache16BppSize(
            Value));
}

LONG NanaBox::RdpClient::BitmapVirtualCache24BppSize()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapVirtualCache24BppSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapVirtualCache24BppSize(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapVirtualCache24BppSize(
            Value));
}

LONG NanaBox::RdpClient::PerformanceFlags()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_PerformanceFlags(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PerformanceFlags(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_PerformanceFlags(
            Value));
}

bool NanaBox::RdpClient::CanAutoReconnect()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_CanAutoReconnect(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpClient::EnableAutoReconnect()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_EnableAutoReconnect(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::EnableAutoReconnect(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_EnableAutoReconnect(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

LONG NanaBox::RdpClient::MaxReconnectAttempts()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_MaxReconnectAttempts(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::MaxReconnectAttempts(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_MaxReconnectAttempts(
            Value));
}

bool NanaBox::RdpClient::ConnectionBarShowMinimizeButton()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ConnectionBarShowMinimizeButton(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ConnectionBarShowMinimizeButton(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ConnectionBarShowMinimizeButton(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::ConnectionBarShowRestoreButton()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ConnectionBarShowRestoreButton(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ConnectionBarShowRestoreButton(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ConnectionBarShowRestoreButton(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

UINT NanaBox::RdpClient::AuthenticationLevel()
{
    UINT RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AuthenticationLevel(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AuthenticationLevel(
    UINT const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_AuthenticationLevel(
            Value));
}

bool NanaBox::RdpClient::RedirectClipboard()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectClipboard(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectClipboard(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectClipboard(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::ConnectionBarShowPinButton()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ConnectionBarShowPinButton(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ConnectionBarShowPinButton(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ConnectionBarShowPinButton(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::PublicMode()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_PublicMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PublicMode(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_PublicMode(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectDevices()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectDevices(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectDevices(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectDevices(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectPOSDevices()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RedirectPOSDevices(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectPOSDevices(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RedirectPOSDevices(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

LONG NanaBox::RdpClient::BitmapVirtualCache32BppSize()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BitmapVirtualCache32BppSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BitmapVirtualCache32BppSize(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BitmapVirtualCache32BppSize(
            Value));
}

bool NanaBox::RdpClient::RelativeMouseMode()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_RelativeMouseMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RelativeMouseMode(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_RelativeMouseMode(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

winrt::hstring NanaBox::RdpClient::AuthenticationServiceClass()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AuthenticationServiceClass(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::AuthenticationServiceClass(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_AdvancedSettings->put_AuthenticationServiceClass(
            RawValue.get()));
}

LONG NanaBox::RdpClient::HotKeyFocusReleaseLeft()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyFocusReleaseLeft(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyFocusReleaseLeft(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyFocusReleaseLeft(
            Value));
}

LONG NanaBox::RdpClient::HotKeyFocusReleaseRight()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_HotKeyFocusReleaseRight(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::HotKeyFocusReleaseRight(
    LONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_HotKeyFocusReleaseRight(
            Value));
}

UINT NanaBox::RdpClient::AuthenticationType()
{
    UINT RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AuthenticationType(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpClient::ConnectToAdministerServer()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ConnectToAdministerServer(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ConnectToAdministerServer(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ConnectToAdministerServer(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::AudioCaptureRedirectionMode()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AudioCaptureRedirectionMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AudioCaptureRedirectionMode(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_AudioCaptureRedirectionMode(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

UINT NanaBox::RdpClient::VideoPlaybackMode()
{
    UINT RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_VideoPlaybackMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::VideoPlaybackMode(
    UINT const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_VideoPlaybackMode(
            Value));
}

bool NanaBox::RdpClient::EnableSuperPan()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_EnableSuperPan(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::EnableSuperPan(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_EnableSuperPan(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

ULONG NanaBox::RdpClient::SuperPanAccelerationFactor()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_SuperPanAccelerationFactor(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::SuperPanAccelerationFactor(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_SuperPanAccelerationFactor(
            Value));
}

UINT NanaBox::RdpClient::AudioQualityMode()
{
    UINT RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_AudioQualityMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AudioQualityMode(
    UINT const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_AudioQualityMode(
            Value));
}

UINT NanaBox::RdpClient::NetworkConnectionType()
{
    UINT RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_NetworkConnectionType(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::NetworkConnectionType(
    UINT const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_NetworkConnectionType(
            Value));
}

bool NanaBox::RdpClient::BandwidthDetection()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_BandwidthDetection(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::BandwidthDetection(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_BandwidthDetection(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

ClientSpec NanaBox::RdpClient::ClientProtocolSpec()
{
    ClientSpec RawValue;
    winrt::check_hresult(
        this->m_AdvancedSettings->get_ClientProtocolSpec(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ClientProtocolSpec(
    ClientSpec const& Value)
{
    winrt::check_hresult(
        this->m_AdvancedSettings->put_ClientProtocolSpec(
            Value));
}

#pragma endregion

#pragma region TransportSettings

winrt::hstring NanaBox::RdpClient::GatewayHostname()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayHostname(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayHostname(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayHostname(
            RawValue.get()));
}

ULONG NanaBox::RdpClient::GatewayUsageMethod()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayUsageMethod(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayUsageMethod(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayUsageMethod(
            Value));
}

ULONG NanaBox::RdpClient::GatewayProfileUsageMethod()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayProfileUsageMethod(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayProfileUsageMethod(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayProfileUsageMethod(
            Value));
}

ULONG NanaBox::RdpClient::GatewayCredsSource()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayCredsSource(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayCredsSource(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayCredsSource(
            Value));
}

ULONG NanaBox::RdpClient::GatewayUserSelectedCredsSource()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayUserSelectedCredsSource(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayUserSelectedCredsSource(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayUserSelectedCredsSource(
            Value));
}

bool NanaBox::RdpClient::GatewayIsSupported()
{
    LONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayIsSupported(
            &RawValue));
    return RawValue;
}

ULONG NanaBox::RdpClient::GatewayDefaultUsageMethod()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayDefaultUsageMethod(
            &RawValue));
    return RawValue;
}

ULONG NanaBox::RdpClient::GatewayCredSharing()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayCredSharing(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayCredSharing(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayCredSharing(
            Value));
}

ULONG NanaBox::RdpClient::GatewayPreAuthRequirement()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayPreAuthRequirement(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayPreAuthRequirement(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayPreAuthRequirement(
            Value));
}

winrt::hstring NanaBox::RdpClient::GatewayPreAuthServerAddr()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayPreAuthServerAddr(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayPreAuthServerAddr(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayPreAuthServerAddr(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::GatewaySupportUrl()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewaySupportUrl(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewaySupportUrl(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewaySupportUrl(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::GatewayEncryptedOtpCookie()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayEncryptedOtpCookie(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayEncryptedOtpCookie(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayEncryptedOtpCookie(
            RawValue.get()));
}

ULONG NanaBox::RdpClient::GatewayEncryptedOtpCookieSize()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayEncryptedOtpCookieSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayEncryptedOtpCookieSize(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayEncryptedOtpCookieSize(
            Value));
}

winrt::hstring NanaBox::RdpClient::GatewayUsername()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayUsername(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayUsername(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayUsername(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::GatewayDomain()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayDomain(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayDomain(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayDomain(
            RawValue.get()));
}

void NanaBox::RdpClient::GatewayPassword(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayPassword(
            RawValue.get()));
}

ULONG NanaBox::RdpClient::GatewayCredSourceCookie()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayCredSourceCookie(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayCredSourceCookie(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayCredSourceCookie(
            Value));
}

winrt::hstring NanaBox::RdpClient::GatewayAuthCookieServerAddr()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayAuthCookieServerAddr(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayAuthCookieServerAddr(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayAuthCookieServerAddr(
            RawValue.get()));
}

winrt::hstring NanaBox::RdpClient::GatewayEncryptedAuthCookie()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayEncryptedAuthCookie(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayEncryptedAuthCookie(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayEncryptedAuthCookie(
            RawValue.get()));
}

ULONG NanaBox::RdpClient::GatewayEncryptedAuthCookieSize()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayEncryptedAuthCookieSize(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GatewayEncryptedAuthCookieSize(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayEncryptedAuthCookieSize(
            Value));
}

winrt::hstring NanaBox::RdpClient::GatewayAuthLoginPage()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_TransportSettings->get_GatewayAuthLoginPage(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::GatewayAuthLoginPage(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayAuthLoginPage(
            RawValue.get()));
}

void NanaBox::RdpClient::GatewayBrokeringType(
    ULONG const& Value)
{
    winrt::check_hresult(
        this->m_TransportSettings->put_GatewayBrokeringType(
            Value));
}

#pragma endregion

#pragma region RemoteProgram

bool NanaBox::RdpClient::RemoteProgramMode()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_RemoteProgram->get_RemoteProgramMode(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RemoteProgramMode(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_RemoteProgram->put_RemoteProgramMode(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

void NanaBox::RdpClient::ServerStartProgram(
    winrt::hstring const& ExecutablePath,
    winrt::hstring const& FilePath,
    winrt::hstring const& WorkingDirectory,
    bool const& ExpandEnvVarInWorkingDirectoryOnServer,
    winrt::hstring const& Arguments,
    bool const& ExpandEnvVarInArgumentsOnServer)
{
    winrt::bstr RawExecutablePath;
    RawExecutablePath.attach(::SysAllocString(ExecutablePath.c_str()));
    winrt::bstr RawFilePath;
    RawFilePath.attach(::SysAllocString(FilePath.c_str()));
    winrt::bstr RawWorkingDirectory;
    RawWorkingDirectory.attach(::SysAllocString(WorkingDirectory.c_str()));
    winrt::bstr RawArguments;
    RawArguments.attach(::SysAllocString(Arguments.c_str()));
    winrt::check_hresult(
        this->m_RemoteProgram->ServerStartProgram(
            RawExecutablePath.get(),
            RawFilePath.get(),
            RawWorkingDirectory.get(),
            ExpandEnvVarInWorkingDirectoryOnServer
            ? VARIANT_TRUE
            : VARIANT_FALSE,
            RawArguments.get(),
            ExpandEnvVarInArgumentsOnServer
            ? VARIANT_TRUE
            : VARIANT_FALSE));
}

void NanaBox::RdpClient::RemoteApplicationName(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RemoteProgram->put_RemoteApplicationName(
            RawValue.get()));
}

void NanaBox::RdpClient::RemoteApplicationProgram(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RemoteProgram->put_RemoteApplicationProgram(
            RawValue.get()));
}

void NanaBox::RdpClient::RemoteApplicationArgs(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_RemoteProgram->put_RemoteApplicationArgs(
            RawValue.get()));
}

#pragma endregion

#pragma region Shell

void NanaBox::RdpClient::Launch()
{
    winrt::check_hresult(
        this->m_Shell->Launch());
}

winrt::hstring NanaBox::RdpClient::RdpFileContents()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_Shell->get_RdpFileContents(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::RdpFileContents(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_Shell->put_RdpFileContents(
            RawValue.get()));
}

VARIANT NanaBox::RdpClient::RdpProperty(
    winrt::hstring const& Name)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    VARIANT RawValue;
    winrt::check_hresult(
        this->m_Shell->GetRdpProperty(
            RawName.get(),
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RdpProperty(
    winrt::hstring const& Name,
    VARIANT const& Value)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_Shell->SetRdpProperty(
            RawName.get(),
            Value));
}

bool NanaBox::RdpClient::IsRemoteProgramClientInstalled()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_Shell->get_IsRemoteProgramClientInstalled(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ShowTrustedSitesManagementDialog()
{
    winrt::check_hresult(
        this->m_Shell->ShowTrustedSitesManagementDialog());
}

#pragma endregion

#pragma region PreferredRedirectionInfo

bool NanaBox::RdpClient::UseRedirectionServerName()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_RedirectionInfo->get_UseRedirectionServerName(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::UseRedirectionServerName(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_RedirectionInfo->put_UseRedirectionServerName(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

#pragma endregion

#pragma region ExtendedSettings

VARIANT NanaBox::RdpClient::Property(
    winrt::hstring const& Name)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    VARIANT RawValue;
    winrt::check_hresult(
        this->m_ExtendedSettings->get_Property(
            RawName.get(),
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::Property(
    winrt::hstring const& Name,
    VARIANT const& Value)
{
    winrt::bstr RawName;
    RawName.attach(::SysAllocString(Name.c_str()));
    winrt::check_hresult(
        this->m_ExtendedSettings->put_Property(
            RawName.get(),
            const_cast<VARIANT*>(&Value)));
}

#pragma endregion

#pragma region NonScriptable

void NanaBox::RdpClient::ClearTextPassword(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_NonScriptable->put_ClearTextPassword(
            RawValue.get()));
}

void NanaBox::RdpClient::ResetPassword()
{
    winrt::check_hresult(
        this->m_NonScriptable->ResetPassword());
}

void NanaBox::RdpClient::NotifyRedirectDeviceChange(
    UINT_PTR wParam,
    LONG_PTR lParam)
{
    winrt::check_hresult(
        this->m_NonScriptable->NotifyRedirectDeviceChange(
            wParam,
            lParam));
}

void NanaBox::RdpClient::SendKeys(
    LONG numKeys,
    VARIANT_BOOL* pbArrayKeyUp,
    LONG* plKeyData)
{
    winrt::check_hresult(
        this->m_NonScriptable->SendKeys(
            numKeys,
            pbArrayKeyUp,
            plKeyData));
}

HWND NanaBox::RdpClient::UIParentWindowHandle()
{
    HWND RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_UIParentWindowHandle(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::UIParentWindowHandle(
    HWND const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_UIParentWindowHandle(
            Value));
}

bool NanaBox::RdpClient::ShowRedirectionWarningDialog()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_ShowRedirectionWarningDialog(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::ShowRedirectionWarningDialog(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_ShowRedirectionWarningDialog(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::PromptForCredentials()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_PromptForCredentials(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PromptForCredentials(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_PromptForCredentials(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::NegotiateSecurityLayer()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_NegotiateSecurityLayer(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::NegotiateSecurityLayer(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_NegotiateSecurityLayer(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::EnableCredSspSupport()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_EnableCredSspSupport(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::EnableCredSspSupport(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_EnableCredSspSupport(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectDynamicDrives()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_RedirectDynamicDrives(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectDynamicDrives(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_RedirectDynamicDrives(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::RedirectDynamicDevices()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_RedirectDynamicDevices(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectDynamicDevices(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_RedirectDynamicDevices(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

void NanaBox::RdpClient::RescanDevices(
    bool const& DynRedir)
{
    winrt::check_hresult(
        this->m_DeviceCollection->RescanDevices(
            DynRedir ? VARIANT_TRUE : VARIANT_FALSE));
}

NanaBox::RdpDevice NanaBox::RdpClient::DeviceByIndex(
    ULONG Index)
{
    winrt::com_ptr<IMsRdpDevice> Value;
    winrt::check_hresult(
        this->m_DeviceCollection->get_DeviceByIndex(
            Index,
            Value.put()));
    return NanaBox::RdpDevice(Value);
}

NanaBox::RdpDevice NanaBox::RdpClient::DeviceById(
    winrt::hstring const& DevInstanceId)
{
    winrt::bstr RawDevInstanceId;
    RawDevInstanceId.attach(::SysAllocString(DevInstanceId.c_str()));
    winrt::com_ptr<IMsRdpDevice> Value;
    winrt::check_hresult(
        this->m_DeviceCollection->get_DeviceById(
            RawDevInstanceId.get(),
            Value.put()));
    return NanaBox::RdpDevice(Value);
}

ULONG NanaBox::RdpClient::DeviceCount()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_DeviceCollection->get_DeviceCount(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RescanDrives(
    bool const& DynRedir)
{
    winrt::check_hresult(
        this->m_DriveCollection->RescanDrives(
            DynRedir ? VARIANT_TRUE : VARIANT_FALSE));
}

NanaBox::RdpDrive NanaBox::RdpClient::DriveByIndex(
    ULONG Index)
{
    winrt::com_ptr<IMsRdpDrive> Value;
    winrt::check_hresult(
        this->m_DriveCollection->get_DriveByIndex(
            Index,
            Value.put()));
    return NanaBox::RdpDrive(Value);
}

ULONG NanaBox::RdpClient::DriveCount()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_DriveCollection->get_DriveCount(
            &RawValue));
    return RawValue;
}

bool NanaBox::RdpClient::WarnAboutSendingCredentials()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_WarnAboutSendingCredentials(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::WarnAboutSendingCredentials(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_WarnAboutSendingCredentials(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::WarnAboutClipboardRedirection()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_WarnAboutClipboardRedirection(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::WarnAboutClipboardRedirection(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_WarnAboutClipboardRedirection(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

winrt::hstring NanaBox::RdpClient::ConnectionBarText()
{
    winrt::bstr RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_ConnectionBarText(
            RawValue.put()));
    return winrt::hstring(RawValue.get());
}

void NanaBox::RdpClient::ConnectionBarText(
    winrt::hstring const& Value)
{
    winrt::bstr RawValue;
    RawValue.attach(::SysAllocString(Value.c_str()));
    winrt::check_hresult(
        this->m_NonScriptable->put_ConnectionBarText(
            RawValue.get()));
}

RedirectionWarningType NanaBox::RdpClient::RedirectionWarningTypeSetting()
{
    RedirectionWarningType RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_RedirectionWarningType(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::RedirectionWarningTypeSetting(
    RedirectionWarningType const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_RedirectionWarningType(
            Value));
}

bool NanaBox::RdpClient::MarkRdpSettingsSecure()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_MarkRdpSettingsSecure(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::MarkRdpSettingsSecure(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_MarkRdpSettingsSecure(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

VARIANT NanaBox::RdpClient::PublisherCertificateChain()
{
    VARIANT RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_PublisherCertificateChain(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PublisherCertificateChain(
    VARIANT const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_PublisherCertificateChain(
            const_cast<VARIANT*>(&Value)));
}

bool NanaBox::RdpClient::WarnAboutPrinterRedirection()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_WarnAboutPrinterRedirection(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::WarnAboutPrinterRedirection(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_WarnAboutPrinterRedirection(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::AllowCredentialSaving()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_AllowCredentialSaving(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AllowCredentialSaving(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_AllowCredentialSaving(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::PromptForCredsOnClient()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_PromptForCredsOnClient(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::PromptForCredsOnClient(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_PromptForCredsOnClient(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::LaunchedViaClientShellInterface()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_LaunchedViaClientShellInterface(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::LaunchedViaClientShellInterface(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_LaunchedViaClientShellInterface(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::TrustedZoneSite()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_TrustedZoneSite(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::TrustedZoneSite(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_TrustedZoneSite(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::UseMultimon()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_UseMultimon(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::UseMultimon(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_UseMultimon(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

ULONG NanaBox::RdpClient::RemoteMonitorCount()
{
    ULONG RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_RemoteMonitorCount(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::GetRemoteMonitorsBoundingBox(
    LONG* pLeft,
    LONG* pTop,
    LONG* pRight,
    LONG* pBottom)
{
    winrt::check_hresult(
        this->m_NonScriptable->GetRemoteMonitorsBoundingBox(
            pLeft,
            pTop,
            pRight,
            pBottom));
}

bool NanaBox::RdpClient::RemoteMonitorLayoutMatchesLocal()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_RemoteMonitorLayoutMatchesLocal(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DisableConnectionBar(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_DisableConnectionBar(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::DisableRemoteAppCapsCheck()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_DisableRemoteAppCapsCheck(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::DisableRemoteAppCapsCheck(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_DisableRemoteAppCapsCheck(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

bool NanaBox::RdpClient::AllowPromptingForCredentials()
{
    VARIANT_BOOL RawValue;
    winrt::check_hresult(
        this->m_NonScriptable->get_AllowPromptingForCredentials(
            &RawValue));
    return RawValue;
}

void NanaBox::RdpClient::AllowPromptingForCredentials(
    bool const& Value)
{
    winrt::check_hresult(
        this->m_NonScriptable->put_AllowPromptingForCredentials(
            Value ? VARIANT_TRUE : VARIANT_FALSE));
}

#pragma endregion

#pragma region Events

winrt::event_token NanaBox::RdpClient::OnConnecting(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnConnecting.add(Handler);
}

void NanaBox::RdpClient::OnConnecting(
    winrt::event_token const& Token)
{
    this->m_OnConnecting.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnConnected(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnConnected.add(Handler);
}

void NanaBox::RdpClient::OnConnected(
    winrt::event_token const& Token)
{
    this->m_OnConnected.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnLoginComplete(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnLoginComplete.add(Handler);
}

void NanaBox::RdpClient::OnLoginComplete(
    winrt::event_token const& Token)
{
    this->m_OnLoginComplete.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnDisconnected(
    NanaBox::OnDisconnectedType const& Handler)
{
    return this->m_OnDisconnected.add(Handler);
}

void NanaBox::RdpClient::OnDisconnected(
    winrt::event_token const& Token)
{
    this->m_OnDisconnected.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnEnterFullScreenMode(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnEnterFullScreenMode.add(Handler);
}

void NanaBox::RdpClient::OnEnterFullScreenMode(
    winrt::event_token const& Token)
{
    this->m_OnEnterFullScreenMode.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnLeaveFullScreenMode(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnLeaveFullScreenMode.add(Handler);
}

void NanaBox::RdpClient::OnLeaveFullScreenMode(
    winrt::event_token const& Token)
{
    this->m_OnLeaveFullScreenMode.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnChannelReceivedData(
    NanaBox::OnChannelReceivedDataType const& Handler)
{
    return this->m_OnChannelReceivedData.add(Handler);
}

void NanaBox::RdpClient::OnChannelReceivedData(
    winrt::event_token const& Token)
{
    this->m_OnChannelReceivedData.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRequestGoFullScreen(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnRequestGoFullScreen.add(Handler);
}

void NanaBox::RdpClient::OnRequestGoFullScreen(
    winrt::event_token const& Token)
{
    this->m_OnRequestGoFullScreen.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRequestLeaveFullScreen(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnRequestLeaveFullScreen.add(Handler);
}

void NanaBox::RdpClient::OnRequestLeaveFullScreen(
    winrt::event_token const& Token)
{
    this->m_OnRequestLeaveFullScreen.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnFatalError(
    NanaBox::OnFatalErrorType const& Handler)
{
    return this->m_OnFatalError.add(Handler);
}

void NanaBox::RdpClient::OnFatalError(
    winrt::event_token const& Token)
{
    this->m_OnFatalError.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnWarning(
    NanaBox::OnWarningType const& Handler)
{
    return this->m_OnWarning.add(Handler);
}

void NanaBox::RdpClient::OnWarning(
    winrt::event_token const& Token)
{
    this->m_OnWarning.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRemoteDesktopSizeChange(
    NanaBox::OnRemoteDesktopSizeChangeType const& Handler)
{
    return this->m_OnRemoteDesktopSizeChange.add(Handler);
}

void NanaBox::RdpClient::OnRemoteDesktopSizeChange(
    winrt::event_token const& Token)
{
    this->m_OnRemoteDesktopSizeChange.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnIdleTimeoutNotification(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnIdleTimeoutNotification.add(Handler);
}

void NanaBox::RdpClient::OnIdleTimeoutNotification(
    winrt::event_token const& Token)
{
    this->m_OnIdleTimeoutNotification.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRequestContainerMinimize(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnRequestContainerMinimize.add(Handler);
}

void NanaBox::RdpClient::OnRequestContainerMinimize(
    winrt::event_token const& Token)
{
    this->m_OnRequestContainerMinimize.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnConfirmClose(
    NanaBox::OnConfirmCloseType const& Handler)
{
    return this->m_OnConfirmClose.add(Handler);
}

void NanaBox::RdpClient::OnConfirmClose(
    winrt::event_token const& Token)
{
    this->m_OnConfirmClose.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnReceivedTSPublicKey(
    NanaBox::OnReceivedTSPublicKeyType const& Handler)
{
    return this->m_OnReceivedTSPublicKey.add(Handler);
}

void NanaBox::RdpClient::OnReceivedTSPublicKey(
    winrt::event_token const& Token)
{
    this->m_OnReceivedTSPublicKey.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnAutoReconnecting(
    NanaBox::OnAutoReconnectingType const& Handler)
{
    return this->m_OnAutoReconnecting.add(Handler);
}

void NanaBox::RdpClient::OnAutoReconnecting(
    winrt::event_token const& Token)
{
    this->m_OnAutoReconnecting.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnAuthenticationWarningDisplayed(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnAuthenticationWarningDisplayed.add(Handler);
}

void NanaBox::RdpClient::OnAuthenticationWarningDisplayed(
    winrt::event_token const& Token)
{
    this->m_OnAuthenticationWarningDisplayed.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnAuthenticationWarningDismissed(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnAuthenticationWarningDismissed.add(Handler);
}

void NanaBox::RdpClient::OnAuthenticationWarningDismissed(
    winrt::event_token const& Token)
{
    this->m_OnAuthenticationWarningDismissed.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRemoteProgramResult(
    NanaBox::OnRemoteProgramResultType const& Handler)
{
    return this->m_OnRemoteProgramResult.add(Handler);
}

void NanaBox::RdpClient::OnRemoteProgramResult(
    winrt::event_token const& Token)
{
    this->m_OnRemoteProgramResult.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRemoteProgramDisplayed(
    NanaBox::OnRemoteProgramDisplayedType const& Handler)
{
    return this->m_OnRemoteProgramDisplayed.add(Handler);
}

void NanaBox::RdpClient::OnRemoteProgramDisplayed(
    winrt::event_token const& Token)
{
    this->m_OnRemoteProgramDisplayed.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnRemoteWindowDisplayed(
    NanaBox::OnRemoteWindowDisplayedType const& Handler)
{
    return this->m_OnRemoteWindowDisplayed.add(Handler);
}

void NanaBox::RdpClient::OnRemoteWindowDisplayed(
    winrt::event_token const& Token)
{
    this->m_OnRemoteWindowDisplayed.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnLogonError(
    NanaBox::OnLogonErrorType const& Handler)
{
    return this->m_OnLogonError.add(Handler);
}

void NanaBox::RdpClient::OnLogonError(
    winrt::event_token const& Token)
{
    this->m_OnLogonError.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnFocusReleased(
    NanaBox::OnFocusReleasedType const& Handler)
{
    return this->m_OnFocusReleased.add(Handler);
}

void NanaBox::RdpClient::OnFocusReleased(
    winrt::event_token const& Token)
{
    this->m_OnFocusReleased.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnUserNameAcquired(
    NanaBox::OnUserNameAcquiredType const& Handler)
{
    return this->m_OnUserNameAcquired.add(Handler);
}

void NanaBox::RdpClient::OnUserNameAcquired(
    winrt::event_token const& Token)
{
    this->m_OnUserNameAcquired.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnMouseInputModeChanged(
    NanaBox::OnMouseInputModeChangedType const& Handler)
{
    return this->m_OnMouseInputModeChanged.add(Handler);
}

void NanaBox::RdpClient::OnMouseInputModeChanged(
    winrt::event_token const& Token)
{
    this->m_OnMouseInputModeChanged.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnServiceMessageReceived(
    NanaBox::OnServiceMessageReceivedType const& Handler)
{
    return this->m_OnServiceMessageReceived.add(Handler);
}

void NanaBox::RdpClient::OnServiceMessageReceived(
    winrt::event_token const& Token)
{
    this->m_OnServiceMessageReceived.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnConnectionBarPullDown(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnConnectionBarPullDown.add(Handler);
}

void NanaBox::RdpClient::OnConnectionBarPullDown(
    winrt::event_token const& Token)
{
    this->m_OnConnectionBarPullDown.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnNetworkStatusChanged(
    NanaBox::OnNetworkStatusChangedType const& Handler)
{
    return this->m_OnNetworkStatusChanged.add(Handler);
}

void NanaBox::RdpClient::OnNetworkStatusChanged(
    winrt::event_token const& Token)
{
    this->m_OnNetworkStatusChanged.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnDevicesButtonPressed(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnDevicesButtonPressed.add(Handler);
}

void NanaBox::RdpClient::OnDevicesButtonPressed(
    winrt::event_token const& Token)
{
    this->m_OnDevicesButtonPressed.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnAutoReconnected(
    NanaBox::OnCallbackType const& Handler)
{
    return this->m_OnAutoReconnected.add(Handler);
}

void NanaBox::RdpClient::OnAutoReconnected(
    winrt::event_token const& Token)
{
    this->m_OnAutoReconnected.remove(Token);
}

winrt::event_token NanaBox::RdpClient::OnAutoReconnecting2(
    NanaBox::OnAutoReconnecting2Type const& Handler)
{
    return this->m_OnAutoReconnecting2.add(Handler);
}

void NanaBox::RdpClient::OnAutoReconnecting2(
    winrt::event_token const& Token)
{
    this->m_OnAutoReconnecting2.remove(Token);
}

HRESULT STDMETHODCALLTYPE NanaBox::RdpClient::GetTypeInfoCount(
    _Out_ UINT* pctinfo)
{
    *pctinfo = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE NanaBox::RdpClient::GetTypeInfo(
    _In_ UINT iTInfo,
    _In_ LCID lcid,
    _Out_ ITypeInfo** ppTInfo)
{
    UNREFERENCED_PARAMETER(iTInfo);
    UNREFERENCED_PARAMETER(lcid);
    *ppTInfo = NULL;
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE NanaBox::RdpClient::GetIDsOfNames(
    _In_ REFIID riid,
    _In_ LPOLESTR* rgszNames,
    _In_ UINT cNames,
    _In_ LCID lcid,
    _Out_ DISPID* rgDispId)
{
    UNREFERENCED_PARAMETER(riid);
    UNREFERENCED_PARAMETER(rgszNames);
    UNREFERENCED_PARAMETER(cNames);
    UNREFERENCED_PARAMETER(lcid);
    UNREFERENCED_PARAMETER(rgDispId);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE NanaBox::RdpClient::Invoke(
    _In_ DISPID dispIdMember,
    _In_ REFIID riid,
    _In_ LCID lcid,
    _In_ WORD wFlags,
    _In_ DISPPARAMS* pDispParams,
    _Out_opt_ VARIANT* pVarResult,
    _Out_opt_ EXCEPINFO* pExcepInfo,
    _Out_opt_ UINT* puArgErr)
{
    UNREFERENCED_PARAMETER(riid);
    UNREFERENCED_PARAMETER(lcid);
    UNREFERENCED_PARAMETER(wFlags);
    UNREFERENCED_PARAMETER(pVarResult);
    UNREFERENCED_PARAMETER(pExcepInfo);
    UNREFERENCED_PARAMETER(puArgErr);

    try
    {
        switch (dispIdMember)
        {
        case MSTSCAXEVENTS_DISPID_CONNECTING:
        {
            this->m_OnConnecting();
            break;
        }
        case MSTSCAXEVENTS_DISPID_CONNECTED:
        {
            this->m_OnConnected();
            break;
        }
        case MSTSCAXEVENTS_DISPID_LOGINCOMPLETE:
        {
            this->m_OnLoginComplete();
            break;
        }
        case MSTSCAXEVENTS_DISPID_DISCONNECTED:
        {
            this->m_OnDisconnected(
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_ENTERFULLSCREENMODE:
        {
            this->m_OnEnterFullScreenMode();
            break;
        }
        case MSTSCAXEVENTS_DISPID_LEAVEFULLSCREENMODE:
        {
            this->m_OnLeaveFullScreenMode();
            break;
        }
        case MSTSCAXEVENTS_DISPID_CHANNELRECEIVEDDATA:
        {
            this->m_OnChannelReceivedData(
                winrt::hstring(pDispParams->rgvarg[1].bstrVal),
                winrt::hstring(pDispParams->rgvarg[0].bstrVal));
            break;
        }
        case MSTSCAXEVENTS_DISPID_REQUESTGOFULLSCREEN:
        {
            this->m_OnRequestGoFullScreen();
            break;
        }
        case MSTSCAXEVENTS_DISPID_REQUESTLEAVEFULLSCREEN:
        {
            this->m_OnRequestLeaveFullScreen();
            break;
        }
        case MSTSCAXEVENTS_DISPID_FATALERROR:
        {
            this->m_OnFatalError(
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_WARNING:
        {
            this->m_OnWarning(
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_REMOTEDESKTOPSIZECHANGE:
        {
            this->m_OnRemoteDesktopSizeChange(
                pDispParams->rgvarg[1].lVal,
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_IDLETIMEOUTNOTIFICATION:
        {
            this->m_OnIdleTimeoutNotification();
            break;
        }
        case MSTSCAXEVENTS_DISPID_REQUESTCONTAINERMINIMIZE:
        {
            this->m_OnRequestContainerMinimize();
            break;
        }
        case MSTSCAXEVENTS_DISPID_CONFIRMCLOSE:
        {
            this->m_OnConfirmClose(
                pDispParams->rgvarg[0].pboolVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_RECEIVEDTSPUBLICKEY:
        {
            this->m_OnReceivedTSPublicKey(
                winrt::hstring(pDispParams->rgvarg[1].bstrVal),
                pDispParams->rgvarg[0].pboolVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_AUTORECONNECTING:
        {
            this->m_OnAutoReconnecting(
                pDispParams->rgvarg[2].lVal,
                pDispParams->rgvarg[1].lVal,
                reinterpret_cast<AutoReconnectContinueState*>(
                    pDispParams->rgvarg[0].plVal));
            break;
        }
        case MSTSCAXEVENTS_DISPID_AUTHENTICATIONWARNINGDISPLAYED:
        {
            this->m_OnAuthenticationWarningDisplayed();
            break;
        }
        case MSTSCAXEVENTS_DISPID_AUTHENTICATIONWARNINGDISMISSED:
        {
            this->m_OnAuthenticationWarningDismissed();
            break;
        }
        case MSTSCAXEVENTS_DISPID_REMOTEPROGRAMRESULT:
        {
            this->m_OnRemoteProgramResult(
                winrt::hstring(pDispParams->rgvarg[2].bstrVal),
                static_cast<RemoteProgramResult>(
                    pDispParams->rgvarg[1].lVal),
                pDispParams->rgvarg[0].boolVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_REMOTEPROGRAMDISPLAYED:
        {
            this->m_OnRemoteProgramDisplayed(
                pDispParams->rgvarg[1].boolVal,
                pDispParams->rgvarg[0].ulVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_REMOTEWINDOWDISPLAYED:
        {
            this->m_OnRemoteWindowDisplayed(
                pDispParams->rgvarg[2].boolVal,
                reinterpret_cast<HWND>(pDispParams->rgvarg[1].byref),
                static_cast<RemoteWindowDisplayedAttribute>(
                    pDispParams->rgvarg[0].lVal));
            break;
        }
        case MSTSCAXEVENTS_DISPID_LOGONERROR:
        {
            this->m_OnLogonError(
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_FOCUSRELEASED:
        {
            this->m_OnFocusReleased(
                pDispParams->rgvarg[0].uintVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_USERNAMEACQUIRED:
        {
            this->m_OnUserNameAcquired(
                winrt::hstring(pDispParams->rgvarg[0].bstrVal));
            break;
        }
        case MSTSCAXEVENTS_DISPID_MOUSEINPUTMODECHANGED:
        {
            this->m_OnMouseInputModeChanged(
                pDispParams->rgvarg[0].boolVal);
        }
        case MSTSCAXEVENTS_DISPID_SERVICEMESSAGERECEIVED:
        {
            this->m_OnServiceMessageReceived(
                winrt::hstring(pDispParams->rgvarg[0].bstrVal));
            break;
        }
        case MSTSCAXEVENTS_DISPID_CONNECTIONBARPULLDOWN:
        {
            this->m_OnConnectionBarPullDown();
            break;
        }
        case MSTSCAXEVENTS_DISPID_NETWORKSTATUSCHANGED:
        {
            this->m_OnNetworkStatusChanged(
                pDispParams->rgvarg[2].ulVal,
                pDispParams->rgvarg[1].lVal,
                pDispParams->rgvarg[0].lVal);
            break;
        }
        case MSTSCAXEVENTS_DISPID_DEVICESBUTTONPRESSED:
        {
            this->m_OnDevicesButtonPressed();
            break;
        }
        case MSTSCAXEVENTS_DISPID_AUTORECONNECTED:
        {
            this->m_OnAutoReconnected();
            break;
        }
        case MSTSCAXEVENTS_DISPID_AUTORECONNECTING2:
        {
            this->m_OnAutoReconnecting2(
                pDispParams->rgvarg[3].lVal,
                pDispParams->rgvarg[2].boolVal,
                pDispParams->rgvarg[1].lVal,
                pDispParams->rgvarg[0].lVal);
            break;
        }
        default:
            break;
        }

        return S_OK;
    }
    catch (winrt::hresult_error const& ex)
    {
        return ex.code();
    }
}

#pragma endregion
