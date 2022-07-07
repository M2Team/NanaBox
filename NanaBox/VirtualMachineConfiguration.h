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
}

#endif // !NANABOX_VIRTUALMACHINECONFIGURATION
