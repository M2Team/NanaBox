/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      NanaBox.Configuration.Parser.h
 * PURPOSE:   Definition for Virtual Machine Configuration Parser
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef NANABOX_CONFIGURATION_PARSER
#define NANABOX_CONFIGURATION_PARSER

#include <NanaBox.Configuration.Specification.h>

#include <Mile.Json.h>

namespace NanaBox
{
    nlohmann::json FromGuestType(
        GuestType const& Value);

    GuestType ToGuestType(
        nlohmann::json const& Value);

    nlohmann::json FromUefiConsoleMode(
        UefiConsoleMode const& Value);

    UefiConsoleMode ToUefiConsoleMode(
        nlohmann::json const& Value);

    nlohmann::json FromGpuAssignmentMode(
        GpuAssignmentMode const& Value);

    GpuAssignmentMode ToGpuAssignmentMode(
        nlohmann::json const& Value);

    nlohmann::json FromScsiDeviceType(
        ScsiDeviceType const& Value);

    ScsiDeviceType ToScsiDeviceType(
        nlohmann::json const& Value);

    nlohmann::json FromComPortsConfiguration(
        ComPortsConfiguration const& Value);

    ComPortsConfiguration ToComPortsConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromGpuConfiguration(
        GpuConfiguration const& Value);

    GpuConfiguration ToGpuConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromNetworkAdapterConfiguration(
        NetworkAdapterConfiguration const& Value);

    NetworkAdapterConfiguration ToNetworkAdapterConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromScsiDeviceConfiguration(
        ScsiDeviceConfiguration const& Value);

    ScsiDeviceConfiguration ToScsiDeviceConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromKeyboardConfiguration(
        KeyboardConfiguration const& Value);

    KeyboardConfiguration ToKeyboardConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromEnhancedSessionConfiguration(
        EnhancedSessionConfiguration const& Value);

    EnhancedSessionConfiguration ToEnhancedSessionConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromChipsetInformationConfiguration(
        ChipsetInformationConfiguration const& Value);

    ChipsetInformationConfiguration ToChipsetInformationConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromVideoMonitorConfiguration(
        VideoMonitorConfiguration const& Value);

    VideoMonitorConfiguration ToVideoMonitorConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromPlan9ShareConfiguration(
        Plan9ShareConfiguration const& Value);

    Plan9ShareConfiguration ToPlan9ShareConfiguration(
        nlohmann::json const& Value);

    nlohmann::json FromVirtualMachineConfiguration(
        VirtualMachineConfiguration const& Value);

    VirtualMachineConfiguration ToVirtualMachineConfiguration(
        nlohmann::json const& Value);
}

#endif // !NANABOX_CONFIGURATION_PARSER
