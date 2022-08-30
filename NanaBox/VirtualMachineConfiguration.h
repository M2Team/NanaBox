/*
 * PROJECT:   NanaBox
 * FILE:      VirtualMachineConfiguration.h
 * PURPOSE:   Definition for the Virtual Machine Configuration
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#ifndef NANABOX_VIRTUALMACHINECONFIGURATION
#define NANABOX_VIRTUALMACHINECONFIGURATION

#if (defined(__cplusplus) && __cplusplus >= 201703L)
#elif (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#else
#error "[NanaBox] You should use a C++ compiler with the C++17 standard."
#endif

#include <cstdint>
#include <string>
#include <vector>

#include <Windows.h>

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
        VirtualDisk = 0,
        VirtualImage = 1,
        PhysicalDevice = 2,
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
        std::vector<std::string> SelectedDevices;
    };

    struct NetworkAdapterConfiguration
    {
        bool Enabled = false;
        bool Connected = false;
        std::string MacAddress;
        std::string EndpointId;
    };

    struct ScsiDeviceConfiguration
    {
        bool Enabled = false;
        ScsiDeviceType Type;
        std::string Path;
    };

    struct FullScreenConfiguration
    {
        bool ConnectionBar = true;
        std::int32_t HotKey = VK_RETURN;    // ENTER key
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
        FullScreenConfiguration FullScreen;
    };

    VirtualMachineConfiguration DeserializeConfiguration(
        std::string const& Configuration);

    std::string SerializeConfiguration(
        VirtualMachineConfiguration const& Configuration);

    std::string MakeHcsUpdateGpuRequest(
        GpuConfiguration const& Value);

    std::string MakeHcsConfiguration(
        VirtualMachineConfiguration const& Configuration);
}

#endif // !NANABOX_VIRTUALMACHINECONFIGURATION
