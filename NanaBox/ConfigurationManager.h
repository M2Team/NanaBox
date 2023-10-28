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

#include <json.hpp>

namespace NanaBox
{
    void ComputeSystemUpdateGpu(
        winrt::com_ptr<ComputeSystem> const& Instance,
        GpuConfiguration const& Configuration);

    void ComputeSystemPrepareResourcesForNetworkAdapters(
        std::string const& Owner,
        std::vector<NetworkAdapterConfiguration>& Configuration);

    void RemoteDesktopUpdateKeyboardConfiguration(
        winrt::com_ptr<RdpClient> const& Instance,
        KeyboardConfiguration& Configuration);

    void DeserializeKeyboardConfiguration(
        nlohmann::json const& Input,
        KeyboardConfiguration& Output);

    nlohmann::json SerializeKeyboardConfiguration(
        KeyboardConfiguration const& Input);

    void DeserializeEnhancedSessionConfiguration(
        nlohmann::json const& Input,
        EnhancedSessionConfiguration& Output);

    nlohmann::json SerializeEnhancedSessionConfiguration(
        EnhancedSessionConfiguration const& Input);
}

#endif // !NANABOX_CONFIGURATION_MANAGER
