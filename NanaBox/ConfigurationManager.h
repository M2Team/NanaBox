/*
 * PROJECT:   NanaBox
 * FILE:      ConfigurationManager.h
 * PURPOSE:   Definition for the Virtual Machine Configuration Manager
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef NANABOX_CONFIGURATION_MANAGER
#define NANABOX_CONFIGURATION_MANAGER

#include "ConfigurationSpecification.h"

#include "HostCompute.h"
#include "RdpClient.h"

namespace NanaBox
{
    void ComputeSystemUpdateGpu(
        winrt::com_ptr<ComputeSystem> const& Instance,
        GpuConfiguration const& Configuration);

    void ComputeSystemAddNetworkAdapters(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::string const& Owner,
        std::vector<NetworkAdapterConfiguration>& Configuration);

    void RemoteDesktopUpdateKeyboardConfiguration(
        winrt::com_ptr<RdpClient> const& Instance,
        KeyboardConfiguration& Configuration);
}

#endif // !NANABOX_CONFIGURATION_MANAGER
