/*
 * PROJECT:   NanaBox
 * FILE:      ConfigurationManager.cpp
 * PURPOSE:   Implementation for the Virtual Machine Configuration Manager
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "ConfigurationManager.h"

#include "Utils.h"

nlohmann::json NanaBox::MakeHcsComPortConfiguration(
    std::string const& NamedPipe)
{
    nlohmann::json Result;

    Result["NamedPipe"] = NamedPipe;

    return Result;
}

nlohmann::json NanaBox::MakeHcsNetworkAdapterConfiguration(
    NanaBox::NetworkAdapterConfiguration const& Configuration)
{
    nlohmann::json Result;

    if (!Configuration.Connected)
    {
        Result["ConnectionState"] = "Disabled";
    }
    Result["EndpointId"] = Configuration.EndpointId;
    Result["MacAddress"] = Configuration.MacAddress;

    return Result;
}

void NanaBox::ComputeSystemUpdateMemorySize(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint64_t const& MemorySize)
{
    nlohmann::json Result;

    Result["ResourcePath"] = "VirtualMachine/ComputeTopology/Memory/SizeInMB";
    Result["RequestType"] = "Update";
    Result["Settings"] = MemorySize;

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemUpdateComPort1(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::ComPortsConfiguration const& Configuration)
{
    if (!Configuration.ComPort1.empty())
    {
        nlohmann::json Result;

        Result["ResourcePath"] = Mile::FormatString(
            "VirtualMachine/Devices/ComPorts/%d",
            0);
        Result["RequestType"] = "Update";
        Result["Settings"] = NanaBox::MakeHcsComPortConfiguration(
            Configuration.ComPort1);

        Instance->Modify(winrt::to_hstring(Result.dump()));
    }
}

void NanaBox::ComputeSystemUpdateComPort2(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::ComPortsConfiguration const& Configuration)
{
    if (!Configuration.ComPort2.empty())
    {
        nlohmann::json Result;

        Result["ResourcePath"] = Mile::FormatString(
            "VirtualMachine/Devices/ComPorts/%d",
            1);
        Result["RequestType"] = "Update";
        Result["Settings"] = NanaBox::MakeHcsComPortConfiguration(
            Configuration.ComPort2);

        Instance->Modify(winrt::to_hstring(Result.dump()));
    }
}

void NanaBox::ComputeSystemAddNetworkAdapter(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::NetworkAdapterConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/NetworkAdapters/%s",
        Configuration.EndpointId.c_str());
    Result["RequestType"] = "Add";
    Result["Settings"] = NanaBox::MakeHcsNetworkAdapterConfiguration(
        Configuration);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemRemoveNetworkAdapter(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::NetworkAdapterConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/NetworkAdapters/%s",
        Configuration.EndpointId.c_str());
    Result["RequestType"] = "Remove";
    Result["Settings"] = NanaBox::MakeHcsNetworkAdapterConfiguration(
        Configuration);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemUpdateGpu(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::GpuConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = "VirtualMachine/ComputeTopology/Gpu";
    Result["RequestType"] = "Update";

    Result["Settings"]["AssignmentMode"] = "Disabled";
    if (NanaBox::GpuAssignmentMode::Default == Configuration.AssignmentMode)
    {
        Result["Settings"]["AssignmentMode"] = "Default";
    }
    else if (NanaBox::GpuAssignmentMode::Mirror == Configuration.AssignmentMode)
    {
        Result["Settings"]["AssignmentMode"] = "Mirror";
    }
    else if (NanaBox::GpuAssignmentMode::List == Configuration.AssignmentMode)
    {
        if (!Configuration.SelectedDevices.empty())
        {
            Result["Settings"]["AssignmentMode"] = "List";
            nlohmann::json Devices;
            for (std::string const& Device : Configuration.SelectedDevices)
            {
                Devices[Device] = 0xffff;
            }
            Result["Settings"]["AssignmentRequest"] = Devices;
        }
    }
    Result["Settings"]["AllowVendorExtension"] = true;

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemPrepareResourcesForNetworkAdapters(
    std::string const& Owner,
    std::vector<NanaBox::NetworkAdapterConfiguration>& Configuration)
{
    NanaBox::HcnNetwork NetworkHandle = NanaBox::HcnOpenNetwork(
        NanaBox::DefaultSwitchId);

    std::string DefaultSwitchIdString = winrt::to_string(
        ::FromGuid(NanaBox::DefaultSwitchId));

    if (!Configuration.empty())
    {
        for (NanaBox::NetworkAdapterConfiguration& NetworkAdapter
            : Configuration)
        {
            GUID EndpointId;
            try
            {
                if (!NetworkAdapter.EndpointId.empty())
                {
                    EndpointId = winrt::guid(NetworkAdapter.EndpointId);
                }
            }
            catch (...)
            {

            }
            if (NetworkAdapter.EndpointId.empty())
            {
                winrt::check_hresult(::CoCreateGuid(&EndpointId));
                NetworkAdapter.EndpointId =
                    winrt::to_string(::FromGuid(EndpointId));
            }

            try
            {
                NanaBox::HcnDeleteEndpoint(EndpointId);
            }
            catch (...)
            {

            }

            NanaBox::HcnEndpoint EndpointHandle;
            nlohmann::json Settings;
            Settings["SchemaVersion"]["Major"] = 2;
            Settings["SchemaVersion"]["Minor"] = 0;
            Settings["Owner"] = Owner;
            Settings["HostComputeNetwork"] = DefaultSwitchIdString;
            if (!NetworkAdapter.MacAddress.empty())
            {
                Settings["MacAddress"] = NetworkAdapter.MacAddress;
            }

            EndpointHandle = NanaBox::HcnCreateEndpoint(
                NetworkHandle,
                EndpointId,
                winrt::to_hstring(Settings.dump()));

            nlohmann::json Properties = nlohmann::json::parse(winrt::to_string(
                NanaBox::HcnQueryEndpointProperties(EndpointHandle)));
            NetworkAdapter.MacAddress = Properties["MacAddress"];
        }
    }
}

void NanaBox::RemoteDesktopUpdateKeyboardConfiguration(
    winrt::com_ptr<NanaBox::RdpClient> const& Instance,
    NanaBox::KeyboardConfiguration& Configuration)
{
    try
    {
        Instance->KeyboardHookMode(
            Configuration.RedirectKeyCombinations ? 1 : 2);
    }
    catch (...)
    {
        Configuration.RedirectKeyCombinations =
            (Instance->KeyboardHookMode() == 1);
    }

    try
    {
        Instance->HotKeyFullScreen(
            Configuration.FullScreenHotkey);
    }
    catch (...)
    {
        Configuration.FullScreenHotkey =
            Instance->HotKeyFullScreen();
    }

    try
    {
        Instance->HotKeyCtrlEsc(
            Configuration.CtrlEscHotkey);
    }
    catch (...)
    {
        Configuration.CtrlEscHotkey =
            Instance->HotKeyCtrlEsc();
    }

    try
    {
        Instance->HotKeyAltEsc(
            Configuration.AltEscHotkey);
    }
    catch (...)
    {
        Configuration.AltEscHotkey =
            Instance->HotKeyAltEsc();
    }

    try
    {
        Instance->HotKeyAltTab(
            Configuration.AltTabHotkey);
    }
    catch (...)
    {
        Configuration.AltTabHotkey =
            Instance->HotKeyAltTab();
    }

    try
    {
        Instance->HotKeyAltShiftTab(
            Configuration.AltShiftTabHotkey);
    }
    catch (...)
    {
        Configuration.AltShiftTabHotkey =
            Instance->HotKeyAltShiftTab();
    }

    try
    {
        Instance->HotKeyAltSpace(
            Configuration.AltSpaceHotkey);
    }
    catch (...)
    {
        Configuration.AltSpaceHotkey =
            Instance->HotKeyAltSpace();
    }

    try
    {
        Instance->HotKeyCtrlAltDel(
            Configuration.CtrlAltDelHotkey);
    }
    catch (...)
    {
        Configuration.CtrlAltDelHotkey =
            Instance->HotKeyCtrlAltDel();
    }

    try
    {
        Instance->HotKeyFocusReleaseLeft(
            Configuration.FocusReleaseLeftHotkey);
    }
    catch (...)
    {
        Configuration.FocusReleaseLeftHotkey =
            Instance->HotKeyFocusReleaseLeft();
    }

    try
    {
        Instance->HotKeyFocusReleaseRight(
            Configuration.FocusReleaseRightHotkey);
    }
    catch (...)
    {
        Configuration.FocusReleaseRightHotkey =
            Instance->HotKeyFocusReleaseRight();
    }
}

void NanaBox::RemoteDesktopUpdateEnhancedSessionConfiguration(
    winrt::com_ptr<NanaBox::RdpClient> const& Instance,
    NanaBox::EnhancedSessionConfiguration& Configuration)
{
    try
    {
        Instance->AudioRedirectionMode(
            Configuration.RedirectAudio ? 0 : 2);
    }
    catch (...)
    {
        Configuration.RedirectAudio =
            (Instance->AudioRedirectionMode() == 0);
    }


    try
    {
        Instance->AudioCaptureRedirectionMode(
            Configuration.RedirectAudioCapture);
    }
    catch (...)
    {
        Configuration.RedirectAudioCapture =
            Instance->AudioCaptureRedirectionMode();
    }

    try
    {
        Instance->RedirectDrives(
            Configuration.RedirectDrives);
    }
    catch (...)
    {
        Configuration.RedirectDrives =
            Instance->RedirectDrives();
    }

    try
    {
        Instance->RedirectPrinters(
            Configuration.RedirectPrinters);
    }
    catch (...)
    {
        Configuration.RedirectPrinters =
            Instance->RedirectPrinters();
    }

    try
    {
        Instance->RedirectPorts(
            Configuration.RedirectPorts);
    }
    catch (...)
    {
        Configuration.RedirectPorts =
            Instance->RedirectPorts();
    }

    try
    {
        Instance->RedirectSmartCards(
            Configuration.RedirectSmartCards);
    }
    catch (...)
    {
        Configuration.RedirectSmartCards =
            Instance->RedirectSmartCards();
    }

    try
    {
        Instance->RedirectClipboard(
            Configuration.RedirectClipboard);
    }
    catch (...)
    {
        Configuration.RedirectClipboard =
            Instance->RedirectClipboard();
    }

    try
    {
        Instance->RedirectDevices(
            Configuration.RedirectDevices);
    }
    catch (...)
    {
        Configuration.RedirectDevices =
            Instance->RedirectDevices();
    }

    try
    {
        Instance->RedirectPOSDevices(
            Configuration.RedirectPOSDevices);
    }
    catch (...)
    {
        Configuration.RedirectPOSDevices =
            Instance->RedirectPOSDevices();
    }

    try
    {
        Instance->RedirectDynamicDrives(
            Configuration.RedirectDynamicDrives);
    }
    catch (...)
    {
        Configuration.RedirectDynamicDrives =
            Instance->RedirectDynamicDrives();
    }

    try
    {
        Instance->RedirectDynamicDevices(
            Configuration.RedirectDynamicDevices);
    }
    catch (...)
    {
        Configuration.RedirectDynamicDevices =
            Instance->RedirectDynamicDevices();
    }

    try
    {
        ULONG Count = Instance->DriveCount();
        for (ULONG i = 0; i < Count; ++i)
        {
            try
            {
                NanaBox::RdpDrive Current = Instance->DriveByIndex(i);
                for (std::string const& Drive : Configuration.Drives)
                {
                    if ((Drive[0] - 'A') == static_cast<char>(
                        Current.DriveLetterIndex()))
                    {
                        Current.RedirectionState(true);
                        break;
                    }
                }
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }

    try
    {
        ULONG Count = Instance->DeviceCount();
        for (ULONG i = 0; i < Count; ++i)
        {
            try
            {
                NanaBox::RdpDevice Current = Instance->DeviceByIndex(i);
                for (std::string const& Device : Configuration.Devices)
                {
                    if (Device == winrt::to_string(Current.DeviceInstanceId()))
                    {
                        Current.RedirectionState(true);
                        break;
                    }
                }
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }
}

void NanaBox::DeserializeKeyboardConfiguration(
    nlohmann::json const& Input,
    NanaBox::KeyboardConfiguration& Output)
{
    try
    {
        Output.RedirectKeyCombinations =
            Input.at("RedirectKeyCombinations").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FullScreenHotkey =
            Input.at("FullScreenHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.CtrlEscHotkey =
            Input.at("CtrlEscHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltEscHotkey =
            Input.at("AltEscHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltTabHotkey =
            Input.at("AltTabHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltShiftTabHotkey =
            Input.at("AltShiftTabHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltSpaceHotkey =
            Input.at("AltSpaceHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.CtrlAltDelHotkey =
            Input.at("CtrlAltDelHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FocusReleaseLeftHotkey =
            Input.at("FocusReleaseLeftHotkey").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FocusReleaseRightHotkey =
            Input.at("FocusReleaseRightHotkey").get<bool>();
    }
    catch (...)
    {

    }
}

nlohmann::json NanaBox::SerializeKeyboardConfiguration(
    NanaBox::KeyboardConfiguration const& Input)
{
    nlohmann::json Output;

    if (!Input.RedirectKeyCombinations)
    {
        Output["RedirectKeyCombinations"] = false;
    }

    if (VK_CANCEL != Input.FullScreenHotkey)
    {
        Output["FullScreenHotkey"] = Input.FullScreenHotkey;
    }

    if (VK_HOME != Input.CtrlEscHotkey)
    {
        Output["CtrlEscHotkey"] = Input.CtrlEscHotkey;
    }

    if (VK_INSERT != Input.AltEscHotkey)
    {
        Output["AltEscHotkey"] = Input.AltEscHotkey;
    }

    if (VK_PRIOR != Input.AltTabHotkey)
    {
        Output["AltTabHotkey"] = Input.AltTabHotkey;
    }

    if (VK_NEXT != Input.AltShiftTabHotkey)
    {
        Output["AltShiftTabHotkey"] = Input.AltShiftTabHotkey;
    }

    if (VK_DELETE != Input.AltSpaceHotkey)
    {
        Output["AltSpaceHotkey"] = Input.AltSpaceHotkey;
    }

    if (VK_END != Input.CtrlAltDelHotkey)
    {
        Output["CtrlAltDelHotkey"] = Input.CtrlAltDelHotkey;
    }

    if (VK_LEFT != Input.FocusReleaseLeftHotkey)
    {
        Output["FocusReleaseLeftHotkey"] = Input.FocusReleaseLeftHotkey;
    }

    if (VK_RIGHT != Input.FocusReleaseRightHotkey)
    {
        Output["FocusReleaseRightHotkey"] = Input.FocusReleaseRightHotkey;
    }

    return Output;
}

void NanaBox::DeserializeEnhancedSessionConfiguration(
    nlohmann::json const& Input,
    NanaBox::EnhancedSessionConfiguration& Output)
{
    try
    {
        Output.RedirectAudio =
            Input.at("RedirectAudio").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectAudioCapture =
            Input.at("RedirectAudioCapture").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectDrives =
            Input.at("RedirectDrives").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectPrinters =
            Input.at("RedirectPrinters").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectPorts =
            Input.at("RedirectPorts").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectSmartCards =
            Input.at("RedirectSmartCards").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectClipboard =
            Input.at("RedirectClipboard").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectDevices =
            Input.at("RedirectDevices").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectPOSDevices =
            Input.at("RedirectPOSDevices").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectDynamicDrives =
            Input.at("RedirectDynamicDrives").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.RedirectDynamicDevices =
            Input.at("RedirectDynamicDevices").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json Drives = Input.at("Drives");

        for (nlohmann::json const& Drive : Drives)
        {
            std::string DriveString = Drive.get<std::string>();
            DriveString.resize(1);
            DriveString[0] = static_cast<char>(std::toupper(DriveString[0]));

            if (DriveString[0] < 'A' || DriveString[0] > 'Z')
            {
                continue;
            }

            try
            {
                Output.Drives.push_back(DriveString);
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json Devices = Input.at("Devices");

        for (nlohmann::json const& Device : Devices)
        {
            try
            {
                Output.Devices.push_back(Device.get<std::string>());
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }
}

nlohmann::json NanaBox::SerializeEnhancedSessionConfiguration(
    NanaBox::EnhancedSessionConfiguration const& Input)
{
    nlohmann::json Output;

    if (!Input.RedirectAudio)
    {
        Output["RedirectAudio"] = false;
    }

    if (Input.RedirectAudioCapture)
    {
        Output["RedirectAudioCapture"] = true;
    }

    if (Input.RedirectDrives)
    {
        Output["RedirectDrives"] = true;
    }

    if (Input.RedirectPrinters)
    {
        Output["RedirectPrinters"] = true;
    }

    if (Input.RedirectPorts)
    {
        Output["RedirectPorts"] = true;
    }

    if (Input.RedirectSmartCards)
    {
        Output["RedirectSmartCards"] = true;
    }

    if (!Input.RedirectClipboard)
    {
        Output["RedirectClipboard"] = false;
    }

    if (Input.RedirectDevices)
    {
        Output["RedirectDevices"] = true;
    }

    if (Input.RedirectPOSDevices)
    {
        Output["RedirectPOSDevices"] = true;
    }

    if (Input.RedirectDynamicDrives)
    {
        Output["RedirectDynamicDrives"] = true;
    }

    if (Input.RedirectDynamicDevices)
    {
        Output["RedirectDynamicDevices"] = true;
    }

    if (!Input.Drives.empty())
    {
        nlohmann::json Drives;
        for (std::string const& Drive : Input.Drives)
        {
            std::string DriveString = Drive;
            DriveString.resize(1);
            DriveString[0] = static_cast<char>(std::toupper(DriveString[0]));

            if (DriveString[0] < 'A' || DriveString[0] > 'Z')
            {
                continue;
            }

            Drives.push_back(DriveString);
        }
        Output["Drives"] = Drives;
    }

    if (!Input.Devices.empty())
    {
        nlohmann::json Devices;
        for (std::string const& Device : Input.Devices)
        {
            Devices.push_back(Device);
        }
        Output["Devices"] = Devices;
    }

    return Output;
}
