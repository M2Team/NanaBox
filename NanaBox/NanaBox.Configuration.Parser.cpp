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
