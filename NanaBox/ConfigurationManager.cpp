/*
 * PROJECT:    NanaBox
 * FILE:       ConfigurationManager.cpp
 * PURPOSE:    Implementation for the Virtual Machine Configuration Manager
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "ConfigurationManager.h"

#include "Utils.h"

#include <Mile.Helpers.Base.h>

#include <NanaBox.Configuration.Parser.h>

#include <Shlwapi.h>

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

    nlohmann::json ComputeTopology;
    {
        nlohmann::json Memory;
        {
            Memory["SizeInMB"] = Configuration.MemorySize;
            Memory["AllowOvercommit"] = true;
        }
        ComputeTopology["Memory"] = Memory;

        nlohmann::json Processor;
        {
            Processor["Count"] = Configuration.ProcessorCount;
            if (Configuration.ExposeVirtualizationExtensions)
            {
                Processor["ExposeVirtualizationExtensions"] = true;
            }
        }
        ComputeTopology["Processor"] = Processor;

        if (::MileIsWindowsVersionAtLeast(10, 0, 26100) &&
            !Configuration.Policies.empty())
        {
            std::uint32_t GuestFeatureSet = 0;
            for (std::string const& Policy : Configuration.Policies)
            {
                if ("LegacyPcrMeasurement" == Policy)
                {
                    GuestFeatureSet |= 0x00000002;
                }
                else if ("DisableSha384Pcr" == Policy)
                {
                    GuestFeatureSet |= 0x00000004;
                }
                else if ("LegacyRteWidth" == Policy)
                {
                    GuestFeatureSet |= 0x00000008;
                }
                else if ("LegacyTrustedLaunchMemoryMap" == Policy)
                {
                    GuestFeatureSet |= 0x00000010;
                }
                else if ("LegacyApicSelectionLogic" == Policy)
                {
                    GuestFeatureSet |= 0x00000020;
                }
                else if ("LegacyApicManganeseOverride" == Policy)
                {
                    GuestFeatureSet |= 0x00000040;
                }
                else if ("LegacyGpaLayout" == Policy)
                {
                    GuestFeatureSet |= 0x00000080;
                }
                else if ("ForceLegacyHcl" == Policy)
                {
                    GuestFeatureSet |= 0x00000100;
                }
                else if ("ForceCurrentHcl" == Policy)
                {
                    GuestFeatureSet |= 0x00000200;
                }
                else if ("UseExtendedMaxVpPerSocketLimit" == Policy)
                {
                    GuestFeatureSet |= 0x00000400;
                }
            }
            if (GuestFeatureSet)
            {
                // UseGuestFeatureSetFlags
                GuestFeatureSet |= 0x00000001;
            }
            ComputeTopology["Compatibility"]["GuestFeatureSet"] =
                GuestFeatureSet;
        }
    }
    Result["VirtualMachine"]["ComputeTopology"] = ComputeTopology;

    // Note: Skip Configuration.Gpu because it need to add at runtime.

    nlohmann::json Devices;
    {
        bool IsDefaultResolution = (
            1024 == Configuration.VideoMonitor.HorizontalResolution &&
            768 == Configuration.VideoMonitor.VerticalResolution);

        nlohmann::json VideoMonitor;
        if (::MileIsWindowsVersionAtLeast(10, 0, 20348) && IsDefaultResolution)
        {
            VideoMonitor["ResolutionType"] = NanaBox::ResolutionType::Default;
        }
        VideoMonitor["HorizontalResolution"] =
            Configuration.VideoMonitor.HorizontalResolution;
        VideoMonitor["VerticalResolution"] =
            Configuration.VideoMonitor.VerticalResolution;
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

        {
            nlohmann::json Plan9Shares;

            if (Configuration.Gpu.EnableHostDriverStore)
            {
                const std::uint32_t Plan9SharePort = 50001;

                const struct { LPCSTR Name; LPCWSTR Path; } Plan9ShareItems[] =
                {
                    { "HostDriverStore", L"\\DriverStore" },
                    { "NanaBox.HostDrivers", L"\\DriverStore\\FileRepository" },
                    { "NanaBox.HostLxssLib", L"\\lxss\\lib" }
                };
                const size_t Plan9ShareItemsCount =
                    sizeof(Plan9ShareItems) / sizeof(*Plan9ShareItems);

                std::wstring SystemDirectoryPath(MAX_PATH, L'\0');
                SystemDirectoryPath.resize(::GetSystemDirectoryW(
                    SystemDirectoryPath.data(),
                    static_cast<UINT>(SystemDirectoryPath.size())));

                for (size_t i = 0; i < Plan9ShareItemsCount; ++i)
                {
                    std::wstring SharePath(
                        SystemDirectoryPath + Plan9ShareItems[i].Path);

                    if (!::PathFileExistsW(SharePath.c_str()))
                    {
                        continue;
                    }

                    nlohmann::json Current;
                    Current["Name"] = Plan9ShareItems[i].Name;
                    Current["AccessName"] = Plan9ShareItems[i].Name;
                    Current["Path"] = Mile::ToString(CP_UTF8, SharePath);
                    Current["Port"] = Plan9SharePort;
                    Current["Flags"] = NanaBox::Plan9ShareFlags::ReadOnly;
                    Plan9Shares.push_back(Current);
                }
            }

            for (NanaBox::Plan9ShareConfiguration const& Plan9Share
                : Configuration.Plan9Shares)
            {
                nlohmann::json Current;
                Current["Name"] = Plan9Share.Name;
                Current["AccessName"] = Plan9Share.Name;
                Current["Path"] = Mile::ToString(
                    CP_UTF8,
                    ::GetAbsolutePath(Mile::ToWideString(
                        CP_UTF8,
                        Plan9Share.Path)));
                Current["Port"] = Plan9Share.Port;
                Current["Flags"] = Plan9Share.ReadOnly
                    ? NanaBox::Plan9ShareFlags::ReadOnly
                    : NanaBox::Plan9ShareFlags::None;
                Plan9Shares.push_back(Current);
            }

            if (!Plan9Shares.empty())
            {
                Devices["Plan9"]["Shares"] = Plan9Shares;
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

NanaBox::HcnNetwork NanaBox::ComputeNetworkGetAvailableNetwork()
{
    nlohmann::json Settings;
    Settings["Name"] = "NanaBox";
    Settings["Type"] = "ICS";
    Settings["IsolateSwitch"] = true;
    Settings["Flags"] =
        NanaBox::HcnNetworkFlags::EnableDns |
        NanaBox::HcnNetworkFlags::EnableDhcp |
        NanaBox::HcnNetworkFlags::EnableNonPersistent;
    NanaBox::HcnNetwork NetworkHandle;
    try
    {
        NetworkHandle = NanaBox::HcnCreateNetwork(
            NanaBox::NanaBoxSwitchId,
            winrt::to_hstring(Settings.dump()));
    }
    catch (winrt::hresult_error const& ex)
    {
        if (HCN_E_NETWORK_ALREADY_EXISTS == ex.code())
        {
            NetworkHandle = NanaBox::HcnOpenNetwork(
                NanaBox::NanaBoxSwitchId);
        }
        else
        {
            throw;
        }
    }
    return NetworkHandle;
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

    NanaBox::HcnNetwork NetworkHandle =
        NanaBox::ComputeNetworkGetAvailableNetwork();

    NanaBox::HcnEndpoint EndpointHandle;
    nlohmann::json Settings;
    Settings["SchemaVersion"]["Major"] = 2;
    Settings["SchemaVersion"]["Minor"] = 0;
    Settings["Owner"] = Owner;
    Settings["HostComputeNetwork"] = winrt::to_string(
        ::FromGuid(NanaBox::NanaBoxSwitchId));
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

    NanaBox::VirtualMachineConfiguration Result =
        NanaBox::ToVirtualMachineConfiguration(RootJson);

    if (Result.Version < 1 || Result.Version > 1)
    {
        throw std::exception("Invalid Version");
    }

    if (!Result.ProcessorCount)
    {
        throw std::exception("Invalid Processor Count");
    }

    if (!Result.MemorySize)
    {
        throw std::exception("Invalid Memory Size");
    }

    return Result;
}

std::string NanaBox::SerializeConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["NanaBox"] = NanaBox::FromVirtualMachineConfiguration(Configuration);
    Result["NanaBox"]["Type"] = "VirtualMachine";

    return Result.dump(2);
}
