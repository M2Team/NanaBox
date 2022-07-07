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

namespace NanaBox
{
    enum class GuestType : std::int32_t
    {
        Unknown = 0,
        Windows = 1,
        Linux = 2,
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

    struct GpuConfiguration
    {
        GpuAssignmentMode AssignmentMode;
        std::vector<std::string> SelectedDevices;
    };

    struct NetworkAdapterConfiguration
    {
        bool Enabled;
        bool Connected;
        std::string MacAddress;
    };

    struct ScsiDeviceConfiguration
    {
        bool Enabled;
        ScsiDeviceType Type;
        std::string Path;
    };

    struct SharedFolderConfiguration
    {
        bool Enabled;
        bool ReadOnly;
        std::string HostPath;
        std::string GuestName;
    };

    struct VirtualMachineConfiguration
    {
        std::uint32_t Version;
        GuestType GuestType;
        std::string Name;
        std::uint32_t ProcessorCount;
        std::uint64_t MemorySize;
        std::vector<std::string> ComPorts;
        GpuConfiguration Gpu;
        std::vector<NetworkAdapterConfiguration> NetworkAdapters;
        std::vector<ScsiDeviceConfiguration> ScsiDevices;
        std::vector<SharedFolderConfiguration> SharedFolders;
        bool SecureBoot;
        bool Tpm;
        std::string GuestStateFile;
        std::string RuntimeStateFile;
    };

    VirtualMachineConfiguration DeserializeConfiguration(
        std::string const& Configuration);

    std::string SerializeConfiguration(
        VirtualMachineConfiguration const& Configuration);
}

#endif // !NANABOX_VIRTUALMACHINECONFIGURATION
