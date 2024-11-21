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

#include <Mile.Helpers.Base.h>

#include <NanaBox.Configuration.Parser.h>

namespace NanaBox
{
    namespace ResolutionType
    {
        enum
        {
            Unspecified = 0,
            Maximum = 2,
            Single = 3,
            Default = 4
        };
    }
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

    nlohmann::json Chipset;
    {
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
        Chipset["Uefi"] = Uefi;

        if (!Configuration.ChipsetInformation.BaseBoardSerialNumber.empty())
        {
            Chipset["BaseBoardSerialNumber"] =
                Configuration.ChipsetInformation.BaseBoardSerialNumber;
        }
        if (!Configuration.ChipsetInformation.ChassisSerialNumber.empty())
        {
            Chipset["ChassisSerialNumber"] =
                Configuration.ChipsetInformation.ChassisSerialNumber;
        }
        if (!Configuration.ChipsetInformation.ChassisAssetTag.empty())
        {
            Chipset["ChassisAssetTag"] =
                Configuration.ChipsetInformation.ChassisAssetTag;
        }

        if (::MileIsWindowsVersionAtLeast(10, 0, 20348))
        {
            nlohmann::json SystemInformation;
            {
                if (!Configuration.ChipsetInformation.Manufacturer.empty())
                {
                    SystemInformation["Manufacturer"] =
                        Configuration.ChipsetInformation.Manufacturer;
                }
                if (!Configuration.ChipsetInformation.ProductName.empty())
                {
                    SystemInformation["ProductName"] =
                        Configuration.ChipsetInformation.ProductName;
                }
                if (!Configuration.ChipsetInformation.Version.empty())
                {
                    SystemInformation["Version"] =
                        Configuration.ChipsetInformation.Version;
                }
                if (!Configuration.ChipsetInformation.SerialNumber.empty())
                {
                    SystemInformation["SerialNumber"] =
                        Configuration.ChipsetInformation.SerialNumber;
                }
                if (!Configuration.ChipsetInformation.UUID.empty())
                {
                    SystemInformation["UUID"] =
                        Configuration.ChipsetInformation.UUID;
                }
                if (!Configuration.ChipsetInformation.SKUNumber.empty())
                {
                    SystemInformation["SKUNumber"] =
                        Configuration.ChipsetInformation.SKUNumber;
                }
                if (!Configuration.ChipsetInformation.Family.empty())
                {
                    SystemInformation["Family"] =
                        Configuration.ChipsetInformation.Family;
                }
            }
            Chipset["SystemInformation"] = SystemInformation;
        }
    }
    Result["VirtualMachine"]["Chipset"] = Chipset;

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
        if (::MileIsWindowsVersionAtLeast(10, 0, 20348))
        {
            VideoMonitor["ResolutionType"] = NanaBox::ResolutionType::Default;
        }
        VideoMonitor["HorizontalResolution"] = 1024;
        VideoMonitor["VerticalResolution"] = 768;
        {
            nlohmann::json ConnectionOptions;
            ConnectionOptions["NamedPipe"] = Mile::FormatString(
                "\\\\.\\pipe\\%s.BasicSession",
                Configuration.Name.c_str());
            nlohmann::json AccessSids;
            AccessSids.push_back(::GetCurrentProcessUserStringSid());
            ConnectionOptions["AccessSids"] = AccessSids;
            VideoMonitor["ConnectionOptions"] = ConnectionOptions;
        }
        Devices["VideoMonitor"] = VideoMonitor;

        nlohmann::json EnhancedModeVideo;
        {
            nlohmann::json ConnectionOptions;
            ConnectionOptions["NamedPipe"] = Mile::FormatString(
                "\\\\.\\pipe\\%s.EnhancedSession",
                Configuration.Name.c_str());
            nlohmann::json AccessSids;
            AccessSids.push_back(::GetCurrentProcessUserStringSid());
            ConnectionOptions["AccessSids"] = AccessSids;
            EnhancedModeVideo["ConnectionOptions"] = ConnectionOptions;
        }
        Devices["EnhancedModeVideo"] = EnhancedModeVideo;

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

        if (Configuration.Gpu.EnableHostDriverStore)
        {
            std::string ShareName = "HostDriverStore";
            std::string SharePath = "C:\\Windows\\System32\\DriverStore";

            {
                nlohmann::json Current;
                Current["Name"] = ShareName;
                Current["Path"] = SharePath;

                Current["Options"]["ReadOnly"] = true;
                Current["Options"]["PseudoOplocks"] = true;
                Current["Options"]["PseudoDirnotify"] = true;
                Current["Options"]["SupportCloudFiles"] = true;

                nlohmann::json SharedFolders;
                SharedFolders.push_back(Current);
                Devices["VirtualSmb"]["Shares"] = SharedFolders;
            }

            {
                const std::uint32_t Plan9ShareFlagsReadOnly = 0x00000001;

                const std::uint32_t Plan9SharePort = 50001;

                nlohmann::json Current;
                Current["Name"] = ShareName;
                Current["AccessName"] = ShareName;
                Current["Path"] = SharePath;
                Current["Port"] = Plan9SharePort;
                Current["Flags"] = Plan9ShareFlagsReadOnly;

                nlohmann::json SharedFolders;
                SharedFolders.push_back(Current);
                Devices["Plan9"]["Shares"] = SharedFolders;
            }
        }
    }
    Result["VirtualMachine"]["Devices"] = Devices;

    if (Configuration.Tpm)
    {
        nlohmann::json SecuritySettings;
        SecuritySettings["EnableTpm"] = true;
        SecuritySettings["Isolation"]["IsolationType"] = "GuestStateOnly";
        Result["VirtualMachine"]["SecuritySettings"] = SecuritySettings;
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
            for (std::pair<std::string, std::uint16_t> const& Device
                : Configuration.SelectedDevices)
            {
                Devices[Device.first] = Device.second;
            }
            Result["Settings"]["AssignmentRequest"] = Devices;
        }
    }
    Result["Settings"]["AllowVendorExtension"] = true;

    Instance->Modify(winrt::to_hstring(Result.dump()));
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
    Output.RedirectKeyCombinations = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectKeyCombinations"),
        Output.RedirectKeyCombinations);

    Output.FullScreenHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "FullScreenHotkey"),
            Output.FullScreenHotkey));

    Output.CtrlEscHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "CtrlEscHotkey"),
            Output.CtrlEscHotkey));

    Output.AltEscHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "AltEscHotkey"),
            Output.AltEscHotkey));

    Output.AltTabHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "AltTabHotkey"),
            Output.AltTabHotkey));

    Output.AltShiftTabHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "AltShiftTabHotkey"),
            Output.AltShiftTabHotkey));

    Output.AltSpaceHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "AltSpaceHotkey"),
            Output.AltSpaceHotkey));

    Output.CtrlAltDelHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "CtrlAltDelHotkey"),
            Output.CtrlAltDelHotkey));

    Output.FocusReleaseLeftHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "FocusReleaseLeftHotkey"),
            Output.FocusReleaseLeftHotkey));

    Output.FocusReleaseRightHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Input, "FocusReleaseRightHotkey"),
            Output.FocusReleaseRightHotkey));
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
    Output.RedirectAudio = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectAudio"),
        Output.RedirectAudio);

    Output.RedirectAudioCapture = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectAudioCapture"),
        Output.RedirectAudioCapture);

    Output.RedirectDrives = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectDrives"),
        Output.RedirectDrives);

    Output.RedirectPrinters = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectPrinters"),
        Output.RedirectPrinters);

    Output.RedirectPorts = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectPorts"),
        Output.RedirectPorts);

    Output.RedirectSmartCards = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectSmartCards"),
        Output.RedirectSmartCards);

    Output.RedirectClipboard = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectClipboard"),
        Output.RedirectClipboard);

    Output.RedirectDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectDevices"),
        Output.RedirectDevices);

    Output.RedirectPOSDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectPOSDevices"),
        Output.RedirectPOSDevices);

    Output.RedirectDynamicDrives = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectDynamicDrives"),
        Output.RedirectDynamicDrives);

    Output.RedirectDynamicDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Input, "RedirectDynamicDevices"),
        Output.RedirectDynamicDevices);

    for (nlohmann::json const& Drive : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Input, "Drives")))
    {
        std::string DriveString = Mile::Json::ToString(Drive);
        DriveString.resize(1);
        DriveString[0] = static_cast<char>(std::toupper(DriveString[0]));

        if (DriveString[0] < 'A' || DriveString[0] > 'Z')
        {
            continue;
        }

        Output.Drives.push_back(DriveString);
    }

    for (nlohmann::json const& Device : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Input, "Devices")))
    {
        std::string DeviceString = Mile::Json::ToString(Device);
        if (!DeviceString.empty())
        {
            Output.Devices.push_back(DeviceString);
        }
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

void NanaBox::DeserializeChipsetInformationConfiguration(
    nlohmann::json const& Input,
    NanaBox::ChipsetInformationConfiguration& Output)
{
    Output.BaseBoardSerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "BaseBoardSerialNumber"),
        Output.BaseBoardSerialNumber);

    Output.ChassisSerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "ChassisSerialNumber"),
        Output.ChassisSerialNumber);

    Output.ChassisAssetTag = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "ChassisAssetTag"),
        Output.ChassisAssetTag);

    Output.Manufacturer = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "Manufacturer"),
        Output.Manufacturer);

    Output.ProductName = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "ProductName"),
        Output.ProductName);

    Output.Version = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "Version"),
        Output.Version);

    Output.SerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "SerialNumber"),
        Output.SerialNumber);

    Output.UUID = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "UUID"),
        Output.UUID);

    Output.SKUNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "SKUNumber"),
        Output.SKUNumber);

    Output.Family = Mile::Json::ToString(
        Mile::Json::GetSubKey(Input, "Family"),
        Output.Family);
}

nlohmann::json NanaBox::SerializeChipsetInformationConfiguration(
    NanaBox::ChipsetInformationConfiguration const& Input)
{
    nlohmann::json Output;

    if (!Input.BaseBoardSerialNumber.empty())
    {
        Output["BaseBoardSerialNumber"] = Input.BaseBoardSerialNumber;
    }

    if (!Input.ChassisSerialNumber.empty())
    {
        Output["ChassisSerialNumber"] = Input.ChassisSerialNumber;
    }

    if (!Input.ChassisAssetTag.empty())
    {
        Output["ChassisAssetTag"] = Input.ChassisAssetTag;
    }

    if (!Input.Manufacturer.empty())
    {
        Output["Manufacturer"] = Input.Manufacturer;
    }

    if (!Input.ProductName.empty())
    {
        Output["ProductName"] = Input.ProductName;
    }

    if (!Input.Version.empty())
    {
        Output["Version"] = Input.Version;
    }

    if (!Input.SerialNumber.empty())
    {
        Output["SerialNumber"] = Input.SerialNumber;
    }

    if (!Input.UUID.empty())
    {
        Output["UUID"] = Input.UUID;
    }

    if (!Input.SKUNumber.empty())
    {
        Output["SKUNumber"] = Input.SKUNumber;
    }

    if (!Input.Family.empty())
    {
        Output["Family"] = Input.Family;
    }

    return Output;
}

NanaBox::VirtualMachineConfiguration NanaBox::DeserializeConfiguration(
    std::string const& Configuration)
{
    nlohmann::json ParsedJson = nlohmann::json::parse(Configuration);

    nlohmann::json RootJson = Mile::Json::GetSubKey(ParsedJson, "NanaBox");

    if ("VirtualMachine" != Mile::Json::ToString(
        Mile::Json::GetSubKey(RootJson, "Type")))
    {
        throw std::exception("Invalid Virtual Machine Configuration");
    }

    NanaBox::VirtualMachineConfiguration Result;

    Result.Version = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(RootJson, "Version")));
    if (Result.Version < 1 || Result.Version > 1)
    {
        throw std::exception("Invalid Version");
    }

    Result.GuestType = NanaBox::ToGuestType(
        Mile::Json::GetSubKey(RootJson, "GuestType"));

    Result.Name = Mile::Json::ToString(
        Mile::Json::GetSubKey(RootJson, "Name"),
        Result.Name);

    Result.ProcessorCount = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(RootJson, "ProcessorCount")));
    if (!Result.ProcessorCount)
    {
        throw std::exception("Invalid Processor Count");
    }

    Result.MemorySize = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(RootJson, "MemorySize")));
    if (!Result.MemorySize)
    {
        throw std::exception("Invalid Memory Size");
    }

    Result.ComPorts = NanaBox::ToComPortsConfiguration(
        Mile::Json::GetSubKey(RootJson, "ComPorts"));

    Result.Gpu = NanaBox::ToGpuConfiguration(
        Mile::Json::GetSubKey(RootJson, "Gpu"));

    for (nlohmann::json const& NetworkAdapter : Mile::Json::ToArray(
        Mile::Json::GetSubKey(RootJson, "NetworkAdapters")))
    {
        Result.NetworkAdapters.push_back(
            NanaBox::ToNetworkAdapterConfiguration(NetworkAdapter));
    }

    for (nlohmann::json const& ScsiDevice : Mile::Json::ToArray(
        Mile::Json::GetSubKey(RootJson, "ScsiDevices")))
    {
        NanaBox::ScsiDeviceConfiguration Current =
            NanaBox::ToScsiDeviceConfiguration(ScsiDevice);
        
        if (NanaBox::ScsiDeviceType::Unknown == Current.Type)
        {
            continue;
        }

        if (Current.Path.empty() &&
            Current.Type != NanaBox::ScsiDeviceType::VirtualImage)
        {
            continue;
        }

        Result.ScsiDevices.push_back(Current);
    }

    Result.SecureBoot = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(RootJson, "SecureBoot"),
        Result.SecureBoot);

    Result.Tpm = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(RootJson, "Tpm"),
        Result.Tpm);

    Result.GuestStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(RootJson, "GuestStateFile"),
        Result.GuestStateFile);

    Result.RuntimeStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(RootJson, "RuntimeStateFile"),
        Result.RuntimeStateFile);

    Result.SaveStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(RootJson, "SaveStateFile"),
        Result.SaveStateFile);

    Result.ExposeVirtualizationExtensions = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(RootJson, "ExposeVirtualizationExtensions"),
        Result.ExposeVirtualizationExtensions);

    NanaBox::DeserializeKeyboardConfiguration(
        Mile::Json::GetSubKey(RootJson, "Keyboard"),
        Result.Keyboard);

    NanaBox::DeserializeEnhancedSessionConfiguration(
        Mile::Json::GetSubKey(RootJson, "EnhancedSession"),
        Result.EnhancedSession);

    NanaBox::DeserializeChipsetInformationConfiguration(
        Mile::Json::GetSubKey(RootJson, "ChipsetInformation"),
        Result.ChipsetInformation);

    return Result;
}

std::string NanaBox::SerializeConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json RootJson;
    RootJson["Type"] = "VirtualMachine";
    RootJson["Version"] = Configuration.Version;
    RootJson["GuestType"] = NanaBox::FromGuestType(Configuration.GuestType);
    RootJson["Name"] = Configuration.Name;
    RootJson["ProcessorCount"] = Configuration.ProcessorCount;
    RootJson["MemorySize"] = Configuration.MemorySize;
    RootJson["ComPorts"] = NanaBox::FromComPortsConfiguration(
        Configuration.ComPorts);
    RootJson["Gpu"] = NanaBox::FromGpuConfiguration(Configuration.Gpu);
    if (!Configuration.NetworkAdapters.empty())
    {
        nlohmann::json NetworkAdapters;
        for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
            : Configuration.NetworkAdapters)
        {
            NetworkAdapters.push_back(
                NanaBox::FromNetworkAdapterConfiguration(NetworkAdapter));
        }
        RootJson["NetworkAdapters"] = NetworkAdapters;
    }
    if (!Configuration.ScsiDevices.empty())
    {
        nlohmann::json ScsiDevices;
        for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
            : Configuration.ScsiDevices)
        {
            if (NanaBox::ScsiDeviceType::Unknown == ScsiDevice.Type)
            {
                continue;
            }

            ScsiDevices.push_back(
                NanaBox::FromScsiDeviceConfiguration(ScsiDevice));
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
    {
        nlohmann::json ChipsetInformation =
            NanaBox::SerializeChipsetInformationConfiguration(
                Configuration.ChipsetInformation);
        if (!ChipsetInformation.empty())
        {
            RootJson["ChipsetInformation"] = ChipsetInformation;
        }
    }

    nlohmann::json Result;
    Result["NanaBox"] = RootJson;
    return Result.dump(2);
}
