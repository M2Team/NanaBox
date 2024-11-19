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
}

#endif // !NANABOX_CONFIGURATION_PARSER
