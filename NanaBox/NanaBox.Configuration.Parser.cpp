/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      NanaBox.Configuration.Parser.cpp
 * PURPOSE:   Implementation for Virtual Machine Configuration Parser
 *
 * LICENSE:   The MIT License
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
