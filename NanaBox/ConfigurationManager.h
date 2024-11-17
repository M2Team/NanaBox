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

#include "NanaBox.Configuration.Specification.h"

#include "HostCompute.h"
#include "RdpClient.h"

#include <Mile.Json.h>

namespace NanaBox
{
    nlohmann::json MakeHcsComPortConfiguration(
        std::string const& NamedPipe);

    nlohmann::json MakeHcsNetworkAdapterConfiguration(
        NetworkAdapterConfiguration const& Configuration);

    nlohmann::json MakeHcsScsiDeviceConfiguration(
        ScsiDeviceConfiguration const& Configuration);

    std::string MakeHcsConfiguration(
        VirtualMachineConfiguration const& Configuration);

    void ComputeNetworkCreateEndpoint(
        std::string const& Owner,
        NetworkAdapterConfiguration& Configuration);

    void ComputeNetworkDeleteEndpoint(
        NetworkAdapterConfiguration& Configuration);

    void ComputeSystemUpdateMemorySize(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint64_t const& MemorySize);

    void ComputeSystemAddComPort(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint32_t const& PortID,
        std::string const& NamedPipe);

    void ComputeSystemRemoveComPort(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint32_t const& PortID,
        std::string const& NamedPipe);

    void ComputeSystemUpdateComPort(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint32_t const& PortID,
        std::string const& NamedPipe);

    void ComputeSystemAddNetworkAdapter(
        winrt::com_ptr<ComputeSystem> const& Instance,
        NetworkAdapterConfiguration const& Configuration);

    void ComputeSystemRemoveNetworkAdapter(
        winrt::com_ptr<ComputeSystem> const& Instance,
        NetworkAdapterConfiguration const& Configuration);

    void ComputeSystemAddScsiDevice(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint32_t const& DeviceID,
        ScsiDeviceConfiguration const& Configuration);

    void ComputeSystemUpdateScsiDevice(
        winrt::com_ptr<ComputeSystem> const& Instance,
        std::uint32_t const& DeviceID,
        ScsiDeviceConfiguration const& Configuration);

    void ComputeSystemUpdateGpu(
        winrt::com_ptr<ComputeSystem> const& Instance,
        GpuConfiguration const& Configuration);

    void RemoteDesktopUpdateKeyboardConfiguration(
        winrt::com_ptr<RdpClient> const& Instance,
        KeyboardConfiguration& Configuration);

    void RemoteDesktopUpdateEnhancedSessionConfiguration(
        winrt::com_ptr<RdpClient> const& Instance,
        EnhancedSessionConfiguration& Configuration);

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

    void DeserializeChipsetInformationConfiguration(
        nlohmann::json const& Input,
        ChipsetInformationConfiguration& Output);

    nlohmann::json SerializeChipsetInformationConfiguration(
        ChipsetInformationConfiguration const& Input);

    VirtualMachineConfiguration DeserializeConfiguration(
        std::string const& Configuration);

    std::string SerializeConfiguration(
        VirtualMachineConfiguration const& Configuration);
}

#endif // !NANABOX_CONFIGURATION_MANAGER
