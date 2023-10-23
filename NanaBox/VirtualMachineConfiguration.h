/*
 * PROJECT:   NanaBox
 * FILE:      VirtualMachineConfiguration.h
 * PURPOSE:   Definition for the Virtual Machine Configuration
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#ifndef NANABOX_VIRTUALMACHINECONFIGURATION
#define NANABOX_VIRTUALMACHINECONFIGURATION

#include "ConfigurationSpecification.h"

namespace NanaBox
{
    VirtualMachineConfiguration DeserializeConfiguration(
        std::string const& Configuration);

    std::string SerializeConfiguration(
        VirtualMachineConfiguration const& Configuration);

    std::string MakeHcsUpdateGpuRequest(
        GpuConfiguration const& Value);

    std::string MakeHcsAddNetworkAdapterRequest(
        NetworkAdapterConfiguration const& Value);

    std::string MakeHcsConfiguration(
        VirtualMachineConfiguration const& Configuration);
}

#endif // !NANABOX_VIRTUALMACHINECONFIGURATION
