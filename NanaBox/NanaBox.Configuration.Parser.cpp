/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      NanaBox.Configuration.Parser.cpp
 * PURPOSE:   Implementation for Virtual Machine Configuration Parser
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "NanaBox.Configuration.Parser.h"

nlohmann::json NanaBox::FromGuestType(
    NanaBox::GuestType const& Value)
{
    if (NanaBox::GuestType::Windows == Value)
    {
        return "Windows";
    }
    else if (NanaBox::GuestType::Linux == Value)
    {
        return "Linux";
    }

    return "Unknown";
}

NanaBox::GuestType NanaBox::ToGuestType(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Windows"))
    {
        return NanaBox::GuestType::Windows;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "Linux"))
    {
        return NanaBox::GuestType::Linux;
    }

    return NanaBox::GuestType::Unknown;
}

nlohmann::json NanaBox::FromUefiConsoleMode(
    NanaBox::UefiConsoleMode const& Value)
{
    if (NanaBox::UefiConsoleMode::Default == Value)
    {
        return "Default";
    }
    else if (NanaBox::UefiConsoleMode::ComPort1 == Value)
    {
        return "ComPort1";
    }
    else if (NanaBox::UefiConsoleMode::ComPort2 == Value)
    {
        return "ComPort2";
    }

    return "Disabled";
}

NanaBox::UefiConsoleMode NanaBox::ToUefiConsoleMode(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Default"))
    {
        return NanaBox::UefiConsoleMode::Default;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "ComPort1"))
    {
        return NanaBox::UefiConsoleMode::ComPort1;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "ComPort2"))
    {
        return NanaBox::UefiConsoleMode::ComPort2;
    }

    return NanaBox::UefiConsoleMode::Disabled;
}

nlohmann::json NanaBox::FromGpuAssignmentMode(
    NanaBox::GpuAssignmentMode const& Value)
{
    if (NanaBox::GpuAssignmentMode::Default == Value)
    {
        return "Default";
    }
    else if (NanaBox::GpuAssignmentMode::List == Value)
    {
        return "List";
    }
    else if (NanaBox::GpuAssignmentMode::Mirror == Value)
    {
        return "Mirror";
    }

    return "Disabled";
}

NanaBox::GpuAssignmentMode NanaBox::ToGpuAssignmentMode(
    nlohmann::json const& Value)
{
    std::string RawValue = Mile::Json::ToString(Value);

    if (0 == std::strcmp(RawValue.c_str(), "Default"))
    {
        return NanaBox::GpuAssignmentMode::Default;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "List"))
    {
        return NanaBox::GpuAssignmentMode::List;
    }
    else if (0 == std::strcmp(RawValue.c_str(), "Mirror"))
    {
        return NanaBox::GpuAssignmentMode::Mirror;
    }

    return NanaBox::GpuAssignmentMode::Disabled;
}
