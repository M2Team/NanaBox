/*
 * PROJECT:   NanaBox
 * FILE:      NanaBox.Configuration.Specification.h
 * PURPOSE:   Definition for the Virtual Machine Configuration Specification
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef NANABOX_CONFIGURATION_SPECIFICATION
#define NANABOX_CONFIGURATION_SPECIFICATION

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaBox] You should use a C++ compiler with the C++17 standard."
#endif

#include <Windows.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace NanaBox
{
    enum class GuestType : std::int32_t
    {
        Unknown = 0,
        Windows = 1,
        Linux = 2,
    };

    enum class UefiConsoleMode : std::int32_t
    {
        Disabled = 0,
        Default = 1,
        ComPort1 = 2,
        ComPort2 = 3,
    };

    enum class GpuAssignmentMode : std::int32_t
    {
        Disabled = 0,
        Default = 1,
        List = 2,
        Mirror = 3,
    };

    enum class ScsiDeviceType : std::int32_t
    {
        Unknown = 0, // Private and only used for configuration parsing.
        VirtualDisk = 1,
        VirtualImage = 2,
        PhysicalDevice = 3,
    };

    struct ComPortsConfiguration
    {
        UefiConsoleMode UefiConsole = UefiConsoleMode::Disabled;
        std::string ComPort1;
        std::string ComPort2;
    };

    struct GpuConfiguration
    {
        GpuAssignmentMode AssignmentMode = GpuAssignmentMode::Disabled;
        bool EnableHostDriverStore = false;
        std::map<std::string, std::uint16_t> SelectedDevices;
    };

    struct NetworkAdapterConfiguration
    {
        bool Connected = false;
        std::string MacAddress;
        std::string EndpointId;
    };

    struct ScsiDeviceConfiguration
    {
        ScsiDeviceType Type;
        std::string Path;
    };

    struct KeyboardConfiguration
    {
        bool RedirectKeyCombinations = true;
        std::int32_t FullScreenHotkey = VK_CANCEL; // CTRL + ALT + ?
        std::int32_t CtrlEscHotkey = VK_HOME; // ALT + ?
        std::int32_t AltEscHotkey = VK_INSERT; // ALT + ?
        std::int32_t AltTabHotkey = VK_PRIOR; // ALT + ?
        std::int32_t AltShiftTabHotkey = VK_NEXT; // ALT + ?
        std::int32_t AltSpaceHotkey = VK_DELETE; // ALT + ?
        std::int32_t CtrlAltDelHotkey = VK_END; // CTRL + ALT + ?
        std::int32_t FocusReleaseLeftHotkey = VK_LEFT; // CTRL + ALT + ?
        std::int32_t FocusReleaseRightHotkey = VK_RIGHT; // CTRL + ALT + ?
    };

    struct EnhancedSessionConfiguration
    {
        bool RedirectAudio = true;
        bool RedirectAudioCapture = false;
        bool RedirectDrives = false;
        bool RedirectPrinters = false;
        bool RedirectPorts = false;
        bool RedirectSmartCards = false;
        bool RedirectClipboard = true;
        bool RedirectDevices = false;
        bool RedirectPOSDevices = false;
        bool RedirectDynamicDrives = false;
        bool RedirectDynamicDevices = false;
        std::vector<std::string> Drives;
        std::vector<std::string> Devices;
    };

    struct ChipsetInformationConfiguration
    {
        std::string BaseBoardSerialNumber;
        std::string ChassisSerialNumber;
        std::string ChassisAssetTag;
        std::string Manufacturer; // At least 20348.
        std::string ProductName; // At least 20348.
        std::string Version; // At least 20348.
        std::string SerialNumber; // At least 20348.
        std::string UUID; // At least 20348.
        std::string SKUNumber; // At least 20348.
        std::string Family; // At least 20348.
    };

    struct Plan9ShareConfiguration
    {
        bool ReadOnly = true;
        std::uint32_t Port;
        std::string Path;
        std::string Name;
    };

    struct VideoMonitorConfiguration
    {
        bool EnableBasicSessionDpiScaling = true;
        bool EnableDpiScalingValueOverride = false;
        bool EnableContentResizing = true;
        bool ShowFullScreenModeConnectionBar = true;
        std::uint16_t HorizontalResolution = 1024;
        std::uint16_t VerticalResolution = 768;
        std::uint32_t OverriddenDpiScalingValue = 100;
    };

    struct VirtualMachineConfiguration
    {
        std::uint32_t Version = 1;
        GuestType GuestType = GuestType::Unknown;
        std::string Name;
        std::uint32_t ProcessorCount = 0;
        std::uint64_t MemorySize = 0;
        ComPortsConfiguration ComPorts;
        GpuConfiguration Gpu;
        std::vector<NetworkAdapterConfiguration> NetworkAdapters;
        std::vector<ScsiDeviceConfiguration> ScsiDevices;
        bool SecureBoot = false;
        bool Tpm = false;
        std::string GuestStateFile;
        std::string RuntimeStateFile;
        std::string SaveStateFile;
        bool ExposeVirtualizationExtensions = false;
        KeyboardConfiguration Keyboard;
        EnhancedSessionConfiguration EnhancedSession;
        ChipsetInformationConfiguration ChipsetInformation;
        std::vector<Plan9ShareConfiguration> Plan9Shares; // Not Implemented
        VideoMonitorConfiguration VideoMonitor; // Not Implemented
    };
}

#endif // !NANABOX_CONFIGURATION_SPECIFICATION
