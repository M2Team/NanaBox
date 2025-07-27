/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       NanaBox.Configuration.Parser.cpp
 * PURPOSE:    Implementation for Virtual Machine Configuration Parser
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "NanaBox.Configuration.Parser.h"

nlohmann::json NanaBox::FromGuestType(
    NanaBox::GuestType const& Value)
{
    if (NanaBox::GuestType::Windows == Value)
    {
        return "Windows";
    }
    else if (NanaBox::GuestType::Linux == Value)
    {
        return "Linux";
    }

    return "Unknown";
}

NanaBox::GuestType NanaBox::ToGuestType(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Windows"))
    {
        return NanaBox::GuestType::Windows;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "Linux"))
    {
        return NanaBox::GuestType::Linux;
    }

    return NanaBox::GuestType::Unknown;
}

nlohmann::json NanaBox::FromUefiConsoleMode(
    NanaBox::UefiConsoleMode const& Value)
{
    if (NanaBox::UefiConsoleMode::Default == Value)
    {
        return "Default";
    }
    else if (NanaBox::UefiConsoleMode::ComPort1 == Value)
    {
        return "ComPort1";
    }
    else if (NanaBox::UefiConsoleMode::ComPort2 == Value)
    {
        return "ComPort2";
    }

    return "Disabled";
}

NanaBox::UefiConsoleMode NanaBox::ToUefiConsoleMode(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Default"))
    {
        return NanaBox::UefiConsoleMode::Default;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "ComPort1"))
    {
        return NanaBox::UefiConsoleMode::ComPort1;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "ComPort2"))
    {
        return NanaBox::UefiConsoleMode::ComPort2;
    }

    return NanaBox::UefiConsoleMode::Disabled;
}

nlohmann::json NanaBox::FromGpuAssignmentMode(
    NanaBox::GpuAssignmentMode const& Value)
{
    if (NanaBox::GpuAssignmentMode::Default == Value)
    {
        return "Default";
    }
    else if (NanaBox::GpuAssignmentMode::List == Value)
    {
        return "List";
    }
    else if (NanaBox::GpuAssignmentMode::Mirror == Value)
    {
        return "Mirror";
    }

    return "Disabled";
}

NanaBox::GpuAssignmentMode NanaBox::ToGpuAssignmentMode(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Default"))
    {
        return NanaBox::GpuAssignmentMode::Default;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "List"))
    {
        return NanaBox::GpuAssignmentMode::List;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "Mirror"))
    {
        return NanaBox::GpuAssignmentMode::Mirror;
    }

    return NanaBox::GpuAssignmentMode::Disabled;
}

nlohmann::json NanaBox::FromScsiDeviceType(
    NanaBox::ScsiDeviceType const& Value)
{
    if (NanaBox::ScsiDeviceType::VirtualDisk == Value)
    {
        return "VirtualDisk";
    }
    else if (NanaBox::ScsiDeviceType::VirtualImage == Value)
    {
        return "VirtualImage";
    }
    else if (NanaBox::ScsiDeviceType::PhysicalDevice == Value)
    {
        return "PhysicalDevice";
    }

    return "";
}

NanaBox::ScsiDeviceType NanaBox::ToScsiDeviceType(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "VirtualDisk"))
    {
        return NanaBox::ScsiDeviceType::VirtualDisk;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "VirtualImage"))
    {
        return NanaBox::ScsiDeviceType::VirtualImage;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "PhysicalDevice"))
    {
        return NanaBox::ScsiDeviceType::PhysicalDevice;
    }

    return NanaBox::ScsiDeviceType::Unknown;
}

nlohmann::json NanaBox::FromComPortsConfiguration(
    NanaBox::ComPortsConfiguration const& Value)
{
    nlohmann::json Result;

    Result["UefiConsole"] = NanaBox::FromUefiConsoleMode(Value.UefiConsole);

    if (!Value.ComPort1.empty())
    {
        Result["ComPort1"] = Value.ComPort1;
    }

    if (!Value.ComPort2.empty())
    {
        Result["ComPort2"] = Value.ComPort2;
    }

    return Result;
}

NanaBox::ComPortsConfiguration NanaBox::ToComPortsConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::ComPortsConfiguration Result;

    Result.UefiConsole = NanaBox::ToUefiConsoleMode(
        Mile::Json::GetSubKey(Value, "UefiConsole"));

    Result.ComPort1 = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "ComPort1"),
        Result.ComPort1);

    Result.ComPort2 = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "ComPort2"),
        Result.ComPort2);

    return Result;
}

nlohmann::json NanaBox::FromGpuConfiguration(
    NanaBox::GpuConfiguration const& Value)
{
    nlohmann::json Result;

    Result["AssignmentMode"] = NanaBox::FromGpuAssignmentMode(
        Value.AssignmentMode);

    if (Value.EnableHostDriverStore)
    {
        Result["EnableHostDriverStore"] = Value.EnableHostDriverStore;
    }

    if (!Value.SelectedDevices.empty())
    {
        nlohmann::json SelectedDevices;
        for (std::pair<std::string, std::uint16_t> const& SelectedDevice
            : Value.SelectedDevices)
        {
            if (0xFFFF == SelectedDevice.second)
            {
                SelectedDevices.push_back(SelectedDevice.first);
            }
            else
            {
                nlohmann::json Current;
                Current["DeviceInterface"] = SelectedDevice.first;
                Current["PartitionId"] = SelectedDevice.second;
                SelectedDevices.push_back(Current);
            }
        }
        Result["SelectedDevices"] = SelectedDevices;
    }

    return Result;
}

NanaBox::GpuConfiguration NanaBox::ToGpuConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::GpuConfiguration Result;

    Result.AssignmentMode = NanaBox::ToGpuAssignmentMode(
        Mile::Json::GetSubKey(Value, "AssignmentMode"));

    Result.EnableHostDriverStore = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "EnableHostDriverStore"),
        Result.EnableHostDriverStore);

    for (nlohmann::json const& SelectedDevice : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "SelectedDevices")))
    {
        std::string DeviceInterface = Mile::Json::ToString(SelectedDevice);
        if (!DeviceInterface.empty())
        {
            Result.SelectedDevices[DeviceInterface] = 0xFFFF;
        }
        else
        {
            DeviceInterface = Mile::Json::ToString(
                Mile::Json::GetSubKey(SelectedDevice, "DeviceInterface"));
            if (!DeviceInterface.empty())
            {
                Result.SelectedDevices[DeviceInterface] =
                    static_cast<std::uint16_t>(
                        Mile::Json::ToUInt64(Mile::Json::GetSubKey(
                            SelectedDevice, "PartitionId")));
            }
        }
    }

    if (Result.SelectedDevices.empty() &&
        Result.AssignmentMode == NanaBox::GpuAssignmentMode::List)
    {
        Result.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
    }

    if (Result.AssignmentMode != NanaBox::GpuAssignmentMode::List)
    {
        Result.SelectedDevices.clear();
    }

    return Result;
}

nlohmann::json NanaBox::FromNetworkAdapterConfiguration(
    NanaBox::NetworkAdapterConfiguration const& Value)
{
    nlohmann::json Result;

    Result["Connected"] = Value.Connected;

    if (!Value.MacAddress.empty())
    {
        Result["MacAddress"] = Value.MacAddress;
    }

    if (!Value.EndpointId.empty())
    {
        Result["EndpointId"] = Value.EndpointId;
    }

    return Result;
}

NanaBox::NetworkAdapterConfiguration NanaBox::ToNetworkAdapterConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::NetworkAdapterConfiguration Result;

    Result.Connected = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "Connected"),
        Result.Connected);

    Result.MacAddress = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "MacAddress"),
        Result.MacAddress);

    Result.EndpointId = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "EndpointId"),
        Result.EndpointId);

    return Result;
}

nlohmann::json NanaBox::FromScsiDeviceConfiguration(
    NanaBox::ScsiDeviceConfiguration const& Value)
{
    nlohmann::json Result;

    Result["Type"] = NanaBox::FromScsiDeviceType(Value.Type);

    if (!Value.Path.empty())
    {
        Result["Path"] = Value.Path;
    }

    return Result;
}

NanaBox::ScsiDeviceConfiguration NanaBox::ToScsiDeviceConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::ScsiDeviceConfiguration Result;

    Result.Type = NanaBox::ToScsiDeviceType(
        Mile::Json::GetSubKey(Value, "Type"));

    Result.Path = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Path"),
        Result.Path);

    return Result;
}

nlohmann::json NanaBox::FromKeyboardConfiguration(
    NanaBox::KeyboardConfiguration const& Value)
{
    nlohmann::json Result;

    if (!Value.RedirectKeyCombinations)
    {
        Result["RedirectKeyCombinations"] = false;
    }

    if (VK_CANCEL != Value.FullScreenHotkey)
    {
        Result["FullScreenHotkey"] = Value.FullScreenHotkey;
    }

    if (VK_HOME != Value.CtrlEscHotkey)
    {
        Result["CtrlEscHotkey"] = Value.CtrlEscHotkey;
    }

    if (VK_INSERT != Value.AltEscHotkey)
    {
        Result["AltEscHotkey"] = Value.AltEscHotkey;
    }

    if (VK_PRIOR != Value.AltTabHotkey)
    {
        Result["AltTabHotkey"] = Value.AltTabHotkey;
    }

    if (VK_NEXT != Value.AltShiftTabHotkey)
    {
        Result["AltShiftTabHotkey"] = Value.AltShiftTabHotkey;
    }

    if (VK_DELETE != Value.AltSpaceHotkey)
    {
        Result["AltSpaceHotkey"] = Value.AltSpaceHotkey;
    }

    if (VK_END != Value.CtrlAltDelHotkey)
    {
        Result["CtrlAltDelHotkey"] = Value.CtrlAltDelHotkey;
    }

    if (VK_LEFT != Value.FocusReleaseLeftHotkey)
    {
        Result["FocusReleaseLeftHotkey"] = Value.FocusReleaseLeftHotkey;
    }

    if (VK_RIGHT != Value.FocusReleaseRightHotkey)
    {
        Result["FocusReleaseRightHotkey"] = Value.FocusReleaseRightHotkey;
    }

    return Result;
}

NanaBox::KeyboardConfiguration NanaBox::ToKeyboardConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::KeyboardConfiguration Result;

    Result.RedirectKeyCombinations = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectKeyCombinations"),
        Result.RedirectKeyCombinations);

    Result.FullScreenHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "FullScreenHotkey"),
            Result.FullScreenHotkey));

    Result.CtrlEscHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "CtrlEscHotkey"),
            Result.CtrlEscHotkey));

    Result.AltEscHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "AltEscHotkey"),
            Result.AltEscHotkey));

    Result.AltTabHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "AltTabHotkey"),
            Result.AltTabHotkey));

    Result.AltShiftTabHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "AltShiftTabHotkey"),
            Result.AltShiftTabHotkey));

    Result.AltSpaceHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "AltSpaceHotkey"),
            Result.AltSpaceHotkey));

    Result.CtrlAltDelHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "CtrlAltDelHotkey"),
            Result.CtrlAltDelHotkey));

    Result.FocusReleaseLeftHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "FocusReleaseLeftHotkey"),
            Result.FocusReleaseLeftHotkey));

    Result.FocusReleaseRightHotkey =
        static_cast<std::int32_t>(Mile::Json::ToInt64(
            Mile::Json::GetSubKey(Value, "FocusReleaseRightHotkey"),
            Result.FocusReleaseRightHotkey));

    return Result;
}

nlohmann::json NanaBox::FromEnhancedSessionConfiguration(
    NanaBox::EnhancedSessionConfiguration const& Value)
{
    nlohmann::json Result;

    if (!Value.RedirectAudio)
    {
        Result["RedirectAudio"] = false;
    }

    if (Value.RedirectAudioCapture)
    {
        Result["RedirectAudioCapture"] = true;
    }

    if (Value.RedirectDrives)
    {
        Result["RedirectDrives"] = true;
    }

    if (Value.RedirectPrinters)
    {
        Result["RedirectPrinters"] = true;
    }

    if (Value.RedirectPorts)
    {
        Result["RedirectPorts"] = true;
    }

    if (Value.RedirectSmartCards)
    {
        Result["RedirectSmartCards"] = true;
    }

    if (!Value.RedirectClipboard)
    {
        Result["RedirectClipboard"] = false;
    }

    if (Value.RedirectDevices)
    {
        Result["RedirectDevices"] = true;
    }

    if (Value.RedirectPOSDevices)
    {
        Result["RedirectPOSDevices"] = true;
    }

    if (Value.RedirectDynamicDrives)
    {
        Result["RedirectDynamicDrives"] = true;
    }

    if (Value.RedirectDynamicDevices)
    {
        Result["RedirectDynamicDevices"] = true;
    }

    if (!Value.Drives.empty())
    {
        nlohmann::json Drives;
        for (std::string const& Drive : Value.Drives)
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
        Result["Drives"] = Drives;
    }

    if (!Value.Devices.empty())
    {
        nlohmann::json Devices;
        for (std::string const& Device : Value.Devices)
        {
            Devices.push_back(Device);
        }
        Result["Devices"] = Devices;
    }

    return Result;
}

NanaBox::EnhancedSessionConfiguration NanaBox::ToEnhancedSessionConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::EnhancedSessionConfiguration Result;

    Result.RedirectAudio = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectAudio"),
        Result.RedirectAudio);

    Result.RedirectAudioCapture = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectAudioCapture"),
        Result.RedirectAudioCapture);

    Result.RedirectDrives = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectDrives"),
        Result.RedirectDrives);

    Result.RedirectPrinters = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectPrinters"),
        Result.RedirectPrinters);

    Result.RedirectPorts = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectPorts"),
        Result.RedirectPorts);

    Result.RedirectSmartCards = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectSmartCards"),
        Result.RedirectSmartCards);

    Result.RedirectClipboard = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectClipboard"),
        Result.RedirectClipboard);

    Result.RedirectDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectDevices"),
        Result.RedirectDevices);

    Result.RedirectPOSDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectPOSDevices"),
        Result.RedirectPOSDevices);

    Result.RedirectDynamicDrives = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectDynamicDrives"),
        Result.RedirectDynamicDrives);

    Result.RedirectDynamicDevices = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "RedirectDynamicDevices"),
        Result.RedirectDynamicDevices);

    for (nlohmann::json const& Drive : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "Drives")))
    {
        std::string DriveString = Mile::Json::ToString(Drive);
        DriveString.resize(1);
        DriveString[0] = static_cast<char>(std::toupper(DriveString[0]));

        if (DriveString[0] < 'A' || DriveString[0] > 'Z')
        {
            continue;
        }

        Result.Drives.push_back(DriveString);
    }

    for (nlohmann::json const& Device : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "Devices")))
    {
        std::string DeviceString = Mile::Json::ToString(Device);
        if (!DeviceString.empty())
        {
            Result.Devices.push_back(DeviceString);
        }
    }

    return Result;
}

nlohmann::json NanaBox::FromChipsetInformationConfiguration(
    NanaBox::ChipsetInformationConfiguration const& Value)
{
    nlohmann::json Result;

    if (!Value.BaseBoardSerialNumber.empty())
    {
        Result["BaseBoardSerialNumber"] = Value.BaseBoardSerialNumber;
    }

    if (!Value.ChassisSerialNumber.empty())
    {
        Result["ChassisSerialNumber"] = Value.ChassisSerialNumber;
    }

    if (!Value.ChassisAssetTag.empty())
    {
        Result["ChassisAssetTag"] = Value.ChassisAssetTag;
    }

    if (!Value.Manufacturer.empty())
    {
        Result["Manufacturer"] = Value.Manufacturer;
    }

    if (!Value.ProductName.empty())
    {
        Result["ProductName"] = Value.ProductName;
    }

    if (!Value.Version.empty())
    {
        Result["Version"] = Value.Version;
    }

    if (!Value.SerialNumber.empty())
    {
        Result["SerialNumber"] = Value.SerialNumber;
    }

    if (!Value.UUID.empty())
    {
        Result["UUID"] = Value.UUID;
    }

    if (!Value.SKUNumber.empty())
    {
        Result["SKUNumber"] = Value.SKUNumber;
    }

    if (!Value.Family.empty())
    {
        Result["Family"] = Value.Family;
    }

    return Result;
}

NanaBox::ChipsetInformationConfiguration NanaBox::ToChipsetInformationConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::ChipsetInformationConfiguration Result;

    Result.BaseBoardSerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "BaseBoardSerialNumber"),
        Result.BaseBoardSerialNumber);

    Result.ChassisSerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "ChassisSerialNumber"),
        Result.ChassisSerialNumber);

    Result.ChassisAssetTag = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "ChassisAssetTag"),
        Result.ChassisAssetTag);

    Result.Manufacturer = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Manufacturer"),
        Result.Manufacturer);

    Result.ProductName = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "ProductName"),
        Result.ProductName);

    Result.Version = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Version"),
        Result.Version);

    Result.SerialNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "SerialNumber"),
        Result.SerialNumber);

    Result.UUID = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "UUID"),
        Result.UUID);

    Result.SKUNumber = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "SKUNumber"),
        Result.SKUNumber);

    Result.Family = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Family"),
        Result.Family);

    return Result;
}

nlohmann::json NanaBox::FromVideoMonitorConfiguration(
    NanaBox::VideoMonitorConfiguration const& Value)
{
    nlohmann::json Result;

    if (1024 != Value.HorizontalResolution)
    {
        Result["HorizontalResolution"] = Value.HorizontalResolution;
    }

    if (768 != Value.VerticalResolution)
    {
        Result["VerticalResolution"] = Value.VerticalResolution;
    }

    if (Value.DisableBasicSessionDpiScaling)
    {
        Result["DisableBasicSessionDpiScaling"] = true;
    }

    return Result;
}

NanaBox::VideoMonitorConfiguration NanaBox::ToVideoMonitorConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::VideoMonitorConfiguration Result;

    Result.HorizontalResolution =
        static_cast<std::uint16_t>(Mile::Json::ToUInt64(
            Mile::Json::GetSubKey(Value, "HorizontalResolution"),
            Result.HorizontalResolution));

    Result.VerticalResolution =
        static_cast<std::uint16_t>(Mile::Json::ToUInt64(
            Mile::Json::GetSubKey(Value, "VerticalResolution"),
            Result.VerticalResolution));

    Result.DisableBasicSessionDpiScaling = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "DisableBasicSessionDpiScaling"),
        Result.DisableBasicSessionDpiScaling);

    return Result;
}

nlohmann::json NanaBox::FromPlan9ShareConfiguration(
    Plan9ShareConfiguration const& Value)
{
    nlohmann::json Result;

    if (Value.ReadOnly)
    {
        Result["ReadOnly"] = true;
    }

    Result["Port"] = Value.Port;

    if (!Value.Path.empty())
    {
        Result["Path"] = Value.Path;
    }

    if (!Value.Name.empty())
    {
        Result["Name"] = Value.Name;
    }

    return Result;
}

NanaBox::Plan9ShareConfiguration NanaBox::ToPlan9ShareConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::Plan9ShareConfiguration Result;

    Result.ReadOnly = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "ReadOnly"),
        Result.ReadOnly);

    Result.Port = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(Value, "Port"),
        Result.Port));

    Result.Path = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Path"),
        Result.Path);

    Result.Name = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Name"),
        Result.Name);

    return Result;
}

nlohmann::json NanaBox::FromVirtualMachineConfiguration(
    NanaBox::VirtualMachineConfiguration const& Value)
{
    nlohmann::json Result;

    Result["Version"] = Value.Version;

    Result["GuestType"] = NanaBox::FromGuestType(Value.GuestType);

    Result["Name"] = Value.Name;

    Result["ProcessorCount"] = Value.ProcessorCount;

    Result["MemorySize"] = Value.MemorySize;

    Result["ComPorts"] = NanaBox::FromComPortsConfiguration(Value.ComPorts);

    Result["Gpu"] = NanaBox::FromGpuConfiguration(Value.Gpu);

    if (!Value.NetworkAdapters.empty())
    {
        nlohmann::json NetworkAdapters;
        for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
            : Value.NetworkAdapters)
        {
            NetworkAdapters.push_back(
                NanaBox::FromNetworkAdapterConfiguration(NetworkAdapter));
        }
        Result["NetworkAdapters"] = NetworkAdapters;
    }

    if (!Value.ScsiDevices.empty())
    {
        nlohmann::json ScsiDevices;
        for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
            : Value.ScsiDevices)
        {
            if (NanaBox::ScsiDeviceType::Unknown == ScsiDevice.Type)
            {
                continue;
            }

            ScsiDevices.push_back(
                NanaBox::FromScsiDeviceConfiguration(ScsiDevice));
        }
        Result["ScsiDevices"] = ScsiDevices;
    }

    if (Value.SecureBoot)
    {
        Result["SecureBoot"] = Value.SecureBoot;
    }

    if (Value.Tpm)
    {
        Result["Tpm"] = Value.Tpm;
    }

    if (!Value.GuestStateFile.empty())
    {
        Result["GuestStateFile"] = Value.GuestStateFile;
    }

    if (!Value.RuntimeStateFile.empty())
    {
        Result["RuntimeStateFile"] = Value.RuntimeStateFile;
    }

    if (!Value.SaveStateFile.empty())
    {
        Result["SaveStateFile"] = Value.SaveStateFile;
    }

    if (Value.ExposeVirtualizationExtensions)
    {
        Result["ExposeVirtualizationExtensions"] =
            Value.ExposeVirtualizationExtensions;
    }

    {
        nlohmann::json Keyboard =
            NanaBox::FromKeyboardConfiguration(Value.Keyboard);
        if (!Keyboard.empty())
        {
            Result["Keyboard"] = Keyboard;
        }
    }

    {
        nlohmann::json EnhancedSession =
            NanaBox::FromEnhancedSessionConfiguration(Value.EnhancedSession);
        if (!EnhancedSession.empty())
        {
            Result["EnhancedSession"] = EnhancedSession;
        }
    }

    {
        nlohmann::json ChipsetInformation =
            NanaBox::FromChipsetInformationConfiguration(
                Value.ChipsetInformation);
        if (!ChipsetInformation.empty())
        {
            Result["ChipsetInformation"] = ChipsetInformation;
        }
    }

    {
        nlohmann::json VideoMonitor =
            NanaBox::FromVideoMonitorConfiguration(Value.VideoMonitor);
        if (!VideoMonitor.empty())
        {
            Result["VideoMonitor"] = VideoMonitor;
        }
    }

    if (!Value.Policies.empty())
    {
        nlohmann::json Policies;
        for (std::string const& Policy : Value.Policies)
        {
            Policies.push_back(Policy);
        }
        Result["Policies"] = Policies;
    }

    if (!Value.Plan9Shares.empty())
    {
        nlohmann::json Plan9Shares;
        for (NanaBox::Plan9ShareConfiguration const& Plan9Share
            : Value.Plan9Shares)
        {
            if (Plan9Share.Path.empty() || Plan9Share.Name.empty())
            {
                continue;
            }

            Plan9Shares.push_back(
                NanaBox::FromPlan9ShareConfiguration(Plan9Share));
        }
        Result["Plan9Shares"] = Plan9Shares;
    }

    return Result;
}

NanaBox::VirtualMachineConfiguration NanaBox::ToVirtualMachineConfiguration(
    nlohmann::json const& Value)
{
    NanaBox::VirtualMachineConfiguration Result;

    Result.Version = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(Value, "Version")));

    Result.GuestType = NanaBox::ToGuestType(
        Mile::Json::GetSubKey(Value, "GuestType"));

    Result.Name = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "Name"),
        Result.Name);

    Result.ProcessorCount = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(Value, "ProcessorCount")));

    Result.MemorySize = static_cast<std::uint32_t>(Mile::Json::ToUInt64(
        Mile::Json::GetSubKey(Value, "MemorySize")));

    Result.ComPorts = NanaBox::ToComPortsConfiguration(
        Mile::Json::GetSubKey(Value, "ComPorts"));

    Result.Gpu = NanaBox::ToGpuConfiguration(
        Mile::Json::GetSubKey(Value, "Gpu"));

    for (nlohmann::json const& NetworkAdapter : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "NetworkAdapters")))
    {
        Result.NetworkAdapters.push_back(
            NanaBox::ToNetworkAdapterConfiguration(NetworkAdapter));
    }

    for (nlohmann::json const& ScsiDevice : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "ScsiDevices")))
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
        Mile::Json::GetSubKey(Value, "SecureBoot"),
        Result.SecureBoot);

    Result.Tpm = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "Tpm"),
        Result.Tpm);

    Result.GuestStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "GuestStateFile"),
        Result.GuestStateFile);

    Result.RuntimeStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "RuntimeStateFile"),
        Result.RuntimeStateFile);

    Result.SaveStateFile = Mile::Json::ToString(
        Mile::Json::GetSubKey(Value, "SaveStateFile"),
        Result.SaveStateFile);

    Result.ExposeVirtualizationExtensions = Mile::Json::ToBoolean(
        Mile::Json::GetSubKey(Value, "ExposeVirtualizationExtensions"),
        Result.ExposeVirtualizationExtensions);

    Result.Keyboard = NanaBox::ToKeyboardConfiguration(
        Mile::Json::GetSubKey(Value, "Keyboard"));

    Result.EnhancedSession = NanaBox::ToEnhancedSessionConfiguration(
        Mile::Json::GetSubKey(Value, "EnhancedSession"));

    Result.ChipsetInformation = NanaBox::ToChipsetInformationConfiguration(
        Mile::Json::GetSubKey(Value, "ChipsetInformation"));

    Result.VideoMonitor = NanaBox::ToVideoMonitorConfiguration(
        Mile::Json::GetSubKey(Value, "VideoMonitor"));

    for (nlohmann::json const& Policy : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "Policies")))
    {
        std::string PolicyString = Mile::Json::ToString(Policy);
        if (!PolicyString.empty())
        {
            Result.Policies.push_back(PolicyString);
        }
    }

    for (nlohmann::json const& Plan9Share : Mile::Json::ToArray(
        Mile::Json::GetSubKey(Value, "Plan9Shares")))
    {
        NanaBox::Plan9ShareConfiguration Current =
            NanaBox::ToPlan9ShareConfiguration(Plan9Share);

        if (Current.Path.empty() || Current.Name.empty())
        {
            continue;
        }

        Result.Plan9Shares.push_back(Current);
    }

    return Result;
}
