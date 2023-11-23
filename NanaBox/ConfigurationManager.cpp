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

namespace NanaBox
{
    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GuestType, {
        { NanaBox::GuestType::Unknown, "Unknown" },
        { NanaBox::GuestType::Windows, "Windows" },
        { NanaBox::GuestType::Linux, "Linux" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::UefiConsoleMode, {
        { NanaBox::UefiConsoleMode::Disabled, "Disabled" },
        { NanaBox::UefiConsoleMode::Default, "Default" },
        { NanaBox::UefiConsoleMode::ComPort1, "ComPort1" },
        { NanaBox::UefiConsoleMode::ComPort2, "ComPort2" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GpuAssignmentMode, {
        { NanaBox::GpuAssignmentMode::Disabled, "Disabled" },
        { NanaBox::GpuAssignmentMode::Default, "Default" },
        { NanaBox::GpuAssignmentMode::List, "List" },
        { NanaBox::GpuAssignmentMode::Mirror, "Mirror" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::ScsiDeviceType, {
        { NanaBox::ScsiDeviceType::VirtualDisk, "VirtualDisk" },
        { NanaBox::ScsiDeviceType::VirtualImage, "VirtualImage" },
        { NanaBox::ScsiDeviceType::PhysicalDevice, "PhysicalDevice" }
    })
}

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

nlohmann::json NanaBox::MakeHcsScsiDeviceConfiguration(
    NanaBox::ScsiDeviceConfiguration const& Configuration)
{
    nlohmann::json Result;

    switch (Configuration.Type)
    {
    case NanaBox::ScsiDeviceType::VirtualDisk:
    {
        Result["Type"] = "VirtualDisk";
        break;
    }
    case NanaBox::ScsiDeviceType::VirtualImage:
    {
        Result["Type"] = "Iso";
        break;
    }
    case NanaBox::ScsiDeviceType::PhysicalDevice:
    {
        Result["Type"] = "PassThru";
        break;
    }
    default:
        Result["Type"] = "";
        break;
    }
    Result["Path"] = Mile::ToString(
        CP_UTF8,
        ::GetAbsolutePath(Mile::ToWideString(
            CP_UTF8,
            Configuration.Path)));

    return Result;
}

std::string NanaBox::MakeHcsConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["SchemaVersion"]["Major"] = 2;
    Result["SchemaVersion"]["Minor"] = 1;

    Result["Owner"] = Configuration.Name;

    Result["ShouldTerminateOnLastHandleClosed"] = true;

    nlohmann::json Uefi;
    {
        switch (Configuration.ComPorts.UefiConsole)
        {
        case NanaBox::UefiConsoleMode::Default:
            Uefi["Console"] = "Default";
            break;
        case NanaBox::UefiConsoleMode::ComPort1:
            Uefi["Console"] = "ComPort1";
            break;
        case NanaBox::UefiConsoleMode::ComPort2:
            Uefi["Console"] = "ComPort2";
            break;
        default:
            Uefi["Console"] = "Disabled";
            break;
        }

        if (Configuration.SecureBoot)
        {
            Uefi["ApplySecureBootTemplate"] = "Apply";
            Uefi["SecureBootTemplateId"] =
                "1734c6e8-3154-4dda-ba5f-a874cc483422";
        }
    }
    Result["VirtualMachine"]["Chipset"]["Uefi"] = Uefi;

    nlohmann::json Memory;
    Memory["SizeInMB"] = Configuration.MemorySize;
    Memory["AllowOvercommit"] = true;
    Result["VirtualMachine"]["ComputeTopology"]["Memory"] = Memory;

    nlohmann::json Processor;
    Processor["Count"] = Configuration.ProcessorCount;
    if (Configuration.ExposeVirtualizationExtensions)
    {
        Processor["ExposeVirtualizationExtensions"] = true;
    }
    Result["VirtualMachine"]["ComputeTopology"]["Processor"] = Processor;

    // Note: Skip Configuration.Gpu because it need to add at runtime.

    nlohmann::json Devices;
    {
        nlohmann::json VideoMonitor;
        VideoMonitor["HorizontalResolution"] = 1024;
        VideoMonitor["VerticalResolution"] = 768;
        Devices["VideoMonitor"] = VideoMonitor;

        Devices["EnhancedModeVideo"] = nlohmann::json::object();

        Devices["Keyboard"] = nlohmann::json::object();

        Devices["Mouse"] = nlohmann::json::object();

        nlohmann::json ComPorts;
        if (!Configuration.ComPorts.ComPort1.empty())
        {
            ComPorts["0"] = NanaBox::MakeHcsComPortConfiguration(
                Configuration.ComPorts.ComPort1);
        }
        if (!Configuration.ComPorts.ComPort2.empty())
        {
            ComPorts["1"] = NanaBox::MakeHcsComPortConfiguration(
                Configuration.ComPorts.ComPort2);
        }
        Devices["ComPorts"] = ComPorts;

        if (!Configuration.NetworkAdapters.empty())
        {
            nlohmann::json NetworkAdapters;
            for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
                : Configuration.NetworkAdapters)
            {
                if (!NetworkAdapter.Connected)
                {
                    continue;
                }
                NetworkAdapters[NetworkAdapter.EndpointId] =
                    NanaBox::MakeHcsNetworkAdapterConfiguration(NetworkAdapter);
            }
            Devices["NetworkAdapters"] = NetworkAdapters;
        }

        if (!Configuration.ScsiDevices.empty())
        {
            nlohmann::json ScsiDevices;
            std::uint32_t Count = 0;
            for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
                : Configuration.ScsiDevices)
            {
                ScsiDevices[std::to_string(Count++)] =
                    NanaBox::MakeHcsScsiDeviceConfiguration(ScsiDevice);
            }
            nlohmann::json ScsiController;
            ScsiController["Attachments"] = ScsiDevices;
            Devices["Scsi"]["NanaBox Scsi Controller"] = ScsiController;
        }
    }
    Result["VirtualMachine"]["Devices"] = Devices;

    if (Configuration.Tpm)
    {
        Result["VirtualMachine"]["SecuritySettings"]["EnableTpm"] = true;
    }

    if (!Configuration.GuestStateFile.empty())
    {
        Result["VirtualMachine"]["GuestState"]["GuestStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.GuestStateFile)));
    }

    if (!Configuration.RuntimeStateFile.empty())
    {
        Result["VirtualMachine"]["GuestState"]["RuntimeStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.RuntimeStateFile)));
    }

    if (!Configuration.SaveStateFile.empty())
    {
        Result["VirtualMachine"]["RestoreState"]["SaveStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.SaveStateFile)));
    }

    return Result.dump(2);
}

void NanaBox::ComputeNetworkCreateEndpoint(
    std::string const& Owner,
    NanaBox::NetworkAdapterConfiguration& Configuration)
{
    GUID EndpointId;
    if (Configuration.EndpointId.empty())
    {
        winrt::check_hresult(::CoCreateGuid(&EndpointId));
        Configuration.EndpointId = winrt::to_string(::FromGuid(EndpointId));
    }
    else
    {
        EndpointId = winrt::guid(Configuration.EndpointId);
    }

    NanaBox::HcnNetwork NetworkHandle = NanaBox::HcnOpenNetwork(
        NanaBox::DefaultSwitchId);

    std::string DefaultSwitchIdString = winrt::to_string(
        ::FromGuid(NanaBox::DefaultSwitchId));

    NanaBox::HcnEndpoint EndpointHandle;
    nlohmann::json Settings;
    Settings["SchemaVersion"]["Major"] = 2;
    Settings["SchemaVersion"]["Minor"] = 0;
    Settings["Owner"] = Owner;
    Settings["HostComputeNetwork"] = DefaultSwitchIdString;
    if (!Configuration.MacAddress.empty())
    {
        Settings["MacAddress"] = Configuration.MacAddress;
    }

    EndpointHandle = NanaBox::HcnCreateEndpoint(
        NetworkHandle,
        EndpointId,
        winrt::to_hstring(Settings.dump()));

    nlohmann::json Properties = nlohmann::json::parse(winrt::to_string(
        NanaBox::HcnQueryEndpointProperties(EndpointHandle)));
    Configuration.MacAddress = Properties["MacAddress"];
}

void NanaBox::ComputeNetworkDeleteEndpoint(
    NanaBox::NetworkAdapterConfiguration& Configuration)
{
    try
    {
        if (!Configuration.EndpointId.empty())
        {
            GUID EndpointId = winrt::guid(Configuration.EndpointId);
            NanaBox::HcnDeleteEndpoint(EndpointId);
        }
    }
    catch (...)
    {

    }
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

void NanaBox::ComputeSystemAddComPort(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint32_t const& PortID,
    std::string const& NamedPipe)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/ComPorts/%d",
        PortID);
    Result["RequestType"] = "Add";
    Result["Settings"] = NanaBox::MakeHcsComPortConfiguration(NamedPipe);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemRemoveComPort(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint32_t const& PortID,
    std::string const& NamedPipe)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/ComPorts/%d",
        PortID);
    Result["RequestType"] = "Remove";
    Result["Settings"] = NanaBox::MakeHcsComPortConfiguration(NamedPipe);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemUpdateComPort(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint32_t const& PortID,
    std::string const& NamedPipe)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/ComPorts/%d",
        PortID);
    Result["RequestType"] = "Update";
    Result["Settings"] = NanaBox::MakeHcsComPortConfiguration(NamedPipe);

    Instance->Modify(winrt::to_hstring(Result.dump()));
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

void NanaBox::ComputeSystemUpdateNetworkAdapter(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::NetworkAdapterConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/NetworkAdapters/%s",
        Configuration.EndpointId.c_str());
    Result["RequestType"] = "Update";
    Result["Settings"] = NanaBox::MakeHcsNetworkAdapterConfiguration(
        Configuration);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemAddScsiDevice(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint32_t const& DeviceID,
    NanaBox::ScsiDeviceConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/Scsi/NanaBox Scsi Controller/Attachments/%d",
        DeviceID);
    Result["RequestType"] = "Add";
    Result["Settings"] = NanaBox::MakeHcsScsiDeviceConfiguration(Configuration);

    Instance->Modify(winrt::to_hstring(Result.dump()));
}

void NanaBox::ComputeSystemUpdateScsiDevice(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::uint32_t const& DeviceID,
    NanaBox::ScsiDeviceConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = Mile::FormatString(
        "VirtualMachine/Devices/Scsi/NanaBox Scsi Controller/Attachments/%d",
        DeviceID);
    Result["RequestType"] = "Update";
    Result["Settings"] = NanaBox::MakeHcsScsiDeviceConfiguration(Configuration);

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

NanaBox::VirtualMachineConfiguration NanaBox::DeserializeConfiguration(
    std::string const& Configuration)
{
    nlohmann::json ParsedJson = nlohmann::json::parse(Configuration);

    nlohmann::json RootJson = ParsedJson.at("NanaBox");

    if ("VirtualMachine" !=
        RootJson.at("Type").get<std::string>())
    {
        throw std::exception(
            "Invalid Virtual Machine Configuration");
    }

    NanaBox::VirtualMachineConfiguration Result;

    try
    {
        Result.Version =
            RootJson.at("Version").get<std::uint32_t>();
    }
    catch (...)
    {

    }
    if (Result.Version < 1 || Result.Version > 1)
    {
        throw std::exception(
            "Invalid Version");
    }

    Result.GuestType =
        RootJson.at("GuestType").get<NanaBox::GuestType>();

    Result.Name =
        RootJson.at("Name").get<std::string>();

    try
    {
        Result.ProcessorCount =
            RootJson.at("ProcessorCount").get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception(
            "Invalid Processor Count");
    }

    try
    {
        Result.MemorySize =
            RootJson.at("MemorySize").get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception("Invalid Memory Size");
    }

    try
    {
        nlohmann::json ComPorts = RootJson.at("ComPorts");

        try
        {
            Result.ComPorts.UefiConsole =
                ComPorts.at("UefiConsole").get<NanaBox::UefiConsoleMode>();
        }
        catch (...)
        {

        }

        try
        {
            Result.ComPorts.ComPort1 =
                ComPorts.at("ComPort1").get<std::string>();
        }
        catch (...)
        {

        }

        try
        {
            Result.ComPorts.ComPort2 =
                ComPorts.at("ComPort2").get<std::string>();
        }
        catch (...)
        {

        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json Gpu = RootJson.at("Gpu");

        Result.Gpu.AssignmentMode =
            Gpu.at("AssignmentMode").get<NanaBox::GpuAssignmentMode>();

        try
        {
            nlohmann::json SelectedDevices = Gpu.at("SelectedDevices");

            for (nlohmann::json const& SelectedDevice : SelectedDevices)
            {
                try
                {
                    Result.Gpu.SelectedDevices.push_back(
                        SelectedDevice.get<std::string>());
                }
                catch (...)
                {

                }
            }
        }
        catch (...)
        {

        }

        if (Result.Gpu.SelectedDevices.empty() &&
            Result.Gpu.AssignmentMode == NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
        }

        if (Result.Gpu.AssignmentMode != NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.SelectedDevices.clear();
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json NetworkAdapters = RootJson.at("NetworkAdapters");

        for (nlohmann::json const& NetworkAdapter : NetworkAdapters)
        {
            try
            {
                NanaBox::NetworkAdapterConfiguration Current;

                try
                {
                    Current.Connected =
                        NetworkAdapter.at("Connected").get<bool>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.MacAddress =
                        NetworkAdapter.at("MacAddress").get<std::string>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.EndpointId =
                        NetworkAdapter.at("EndpointId").get<std::string>();
                }
                catch (...)
                {

                }

                Result.NetworkAdapters.push_back(Current);
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
        nlohmann::json ScsiDevices = RootJson.at("ScsiDevices");

        for (nlohmann::json const& ScsiDevice : ScsiDevices)
        {
            NanaBox::ScsiDeviceConfiguration Current;

            try
            {
                Current.Type =
                    ScsiDevice.at("Type").get<NanaBox::ScsiDeviceType>();
            }
            catch (...)
            {
                continue;
            }

            try
            {
                Current.Path = ScsiDevice.at("Path").get<std::string>();
            }
            catch (...)
            {

            }
            if (Current.Path.empty() &&
                Current.Type != NanaBox::ScsiDeviceType::VirtualImage)
            {
                continue;
            }

            Result.ScsiDevices.push_back(Current);
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.SecureBoot =
            RootJson.at("SecureBoot").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Tpm =
            RootJson.at("Tpm").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Result.GuestStateFile =
            RootJson.at("GuestStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.RuntimeStateFile =
            RootJson.at("RuntimeStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.SaveStateFile =
            RootJson.at("SaveStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.ExposeVirtualizationExtensions =
            RootJson.at("ExposeVirtualizationExtensions").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        NanaBox::DeserializeKeyboardConfiguration(
            RootJson.at("Keyboard"),
            Result.Keyboard);
    }
    catch (...)
    {

    }

    try
    {
        NanaBox::DeserializeEnhancedSessionConfiguration(
            RootJson.at("EnhancedSession"),
            Result.EnhancedSession);
    }
    catch (...)
    {

    }

    return Result;
}

std::string NanaBox::SerializeConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json RootJson;
    RootJson["Type"] = "VirtualMachine";
    RootJson["Version"] = Configuration.Version;
    RootJson["GuestType"] = Configuration.GuestType;
    RootJson["Name"] = Configuration.Name;
    RootJson["ProcessorCount"] = Configuration.ProcessorCount;
    RootJson["MemorySize"] = Configuration.MemorySize;
    {
        nlohmann::json ComPorts;
        ComPorts["UefiConsole"] = Configuration.ComPorts.UefiConsole;
        if (!Configuration.ComPorts.ComPort1.empty())
        {
            ComPorts["ComPort1"] = Configuration.ComPorts.ComPort1;
        }
        if (!Configuration.ComPorts.ComPort2.empty())
        {
            ComPorts["ComPort2"] = Configuration.ComPorts.ComPort2;
        }
        RootJson["ComPorts"] = ComPorts;
    }
    {
        nlohmann::json Gpu;
        Gpu["AssignmentMode"] = Configuration.Gpu.AssignmentMode;
        if (!Configuration.Gpu.SelectedDevices.empty())
        {
            nlohmann::json SelectedDevices;
            for (std::string const& SelectedDevice
                : Configuration.Gpu.SelectedDevices)
            {
                SelectedDevices.push_back(SelectedDevice);
            }
            Gpu["SelectedDevices"] = SelectedDevices;
        }
        RootJson["Gpu"] = Gpu;
    }
    if (!Configuration.NetworkAdapters.empty())
    {
        nlohmann::json NetworkAdapters;
        for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
            : Configuration.NetworkAdapters)
        {
            nlohmann::json Current;
            Current["Connected"] = NetworkAdapter.Connected;
            if (!NetworkAdapter.MacAddress.empty())
            {
                Current["MacAddress"] = NetworkAdapter.MacAddress;
            }
            if (!NetworkAdapter.EndpointId.empty())
            {
                Current["EndpointId"] = NetworkAdapter.EndpointId;
            }
            NetworkAdapters.push_back(Current);
        }
        RootJson["NetworkAdapters"] = NetworkAdapters;
    }
    if (!Configuration.ScsiDevices.empty())
    {
        nlohmann::json ScsiDevices;
        for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
            : Configuration.ScsiDevices)
        {
            nlohmann::json Current;
            Current["Type"] = ScsiDevice.Type;
            if (!ScsiDevice.Path.empty())
            {
                Current["Path"] = ScsiDevice.Path;
            }
            ScsiDevices.push_back(Current);
        }
        RootJson["ScsiDevices"] = ScsiDevices;
    }
    if (Configuration.SecureBoot)
    {
        RootJson["SecureBoot"] = Configuration.SecureBoot;
    }
    if (Configuration.Tpm)
    {
        RootJson["Tpm"] = Configuration.Tpm;
    }
    if (!Configuration.GuestStateFile.empty())
    {
        RootJson["GuestStateFile"] = Configuration.GuestStateFile;
    }
    if (!Configuration.RuntimeStateFile.empty())
    {
        RootJson["RuntimeStateFile"] = Configuration.RuntimeStateFile;
    }
    if (!Configuration.SaveStateFile.empty())
    {
        RootJson["SaveStateFile"] = Configuration.SaveStateFile;
    }
    if (Configuration.ExposeVirtualizationExtensions)
    {
        RootJson["ExposeVirtualizationExtensions"] =
            Configuration.ExposeVirtualizationExtensions;
    }
    {
        nlohmann::json Keyboard =
            NanaBox::SerializeKeyboardConfiguration(
                Configuration.Keyboard);
        if (!Keyboard.empty())
        {
            RootJson["Keyboard"] = Keyboard;
        }
    }
    {
        nlohmann::json EnhancedSession =
            NanaBox::SerializeEnhancedSessionConfiguration(
                Configuration.EnhancedSession);
        if (!EnhancedSession.empty())
        {
            RootJson["EnhancedSession"] = EnhancedSession;
        }
    }

    nlohmann::json Result;
    Result["NanaBox"] = RootJson;
    return Result.dump(2);
}
