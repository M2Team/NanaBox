/*
 * PROJECT:   NanaBox
 * FILE:      VirtualMachineConfiguration.cpp
 * PURPOSE:   Implementation for the Virtual Machine Configuration
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "VirtualMachineConfiguration.h"

#include <json.hpp>

NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GuestType, {
    { NanaBox::GuestType::Unknown, nullptr },
    { NanaBox::GuestType::Windows, "Windows" },
    { NanaBox::GuestType::Linux, "Linux" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GpuAssignmentMode, {
    { NanaBox::GpuAssignmentMode::Disabled, nullptr },
    { NanaBox::GpuAssignmentMode::Default, "Default" },
    { NanaBox::GpuAssignmentMode::List, "List" },
    { NanaBox::GpuAssignmentMode::Mirror, "Mirror" }
})

NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::ScsiDeviceType, {
    { NanaBox::ScsiDeviceType::VirtualDisk, "VirtualDisk" },
    { NanaBox::ScsiDeviceType::VirtualImage, "VirtualImage" },
    { NanaBox::ScsiDeviceType::PhysicalDevice, "PhysicalDevice" }
})

NanaBox::VirtualMachineConfiguration NanaBox::DeserializeConfiguration(
    std::string const& Configuration)
{
    nlohmann::json ParsedJson = nlohmann::json::parse(Configuration);

    nlohmann::json RootJson = ParsedJson["NanaBox"];

    if ("VirtualMachine" !=
        RootJson["Type"].get<std::string>())
    {
        throw std::exception(
            "Invalid Virtual Machine Configuration");
    }

    NanaBox::VirtualMachineConfiguration Result;

    try
    {
        Result.Version =
            RootJson["Version"].get<std::uint32_t>();
    }
    catch (...)
    {
        Result.Version = 1;
    }
    if (Result.Version < 1 || Result.Version > 1)
    {
        throw std::exception(
            "Invalid Version");
    }

    Result.GuestType =
        RootJson["GuestType"].get<NanaBox::GuestType>();

    Result.Name =
        RootJson["Name"].get<std::string>();

    try
    {
        Result.ProcessorCount =
            RootJson["ProcessorCount"].get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception(
            "Invalid Processor Count");
    }

    try
    {
        Result.MemorySize =
            RootJson["MemorySize"].get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception("Invalid Memory Size");
    }

    try
    {
        nlohmann::json ComPorts = RootJson["ComPorts"];

        for (nlohmann::json const& ComPort : ComPorts)
        {
            try
            {
                Result.ComPorts.push_back(
                    ComPort.get<std::string>());
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json Gpu = RootJson["Gpu"];

        Result.Gpu.AssignmentMode =
            Gpu["AssignmentMode"].get<NanaBox::GpuAssignmentMode>();

        try
        {
            nlohmann::json SelectedDevices = Gpu["SelectedDevices"];

            for (nlohmann::json const& SelectedDevice : SelectedDevices)
            {
                try
                {
                    Result.Gpu.SelectedDevices.push_back(
                        SelectedDevice.get<std::string>());
                }
                catch (...)
                {

                }
            }
        }
        catch (...)
        {

        }

        if (Result.Gpu.SelectedDevices.empty() &&
            Result.Gpu.AssignmentMode == NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.AssignmentMode = NanaBox::GpuAssignmentMode::Disabled;
        }

        if (Result.Gpu.AssignmentMode != NanaBox::GpuAssignmentMode::List)
        {
            Result.Gpu.SelectedDevices.clear();
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json NetworkAdapters = RootJson["NetworkAdapters"];

        for (nlohmann::json const& NetworkAdapter : NetworkAdapters)
        {
            try
            {
                NanaBox::NetworkAdapterConfiguration Current;

                try
                {
                    Current.Enabled =
                        NetworkAdapter["Enabled"].get<bool>();
                }
                catch (...)
                {
                    Current.Enabled = false;
                }

                try
                {
                    Current.Connected =
                        NetworkAdapter["Connected"].get<bool>();
                }
                catch (...)
                {
                    Current.Connected = false;
                }

                Current.MacAddress =
                    NetworkAdapter["MacAddress"].get<std::string>();
                if (Current.MacAddress.empty())
                {
                    continue;
                }

                Result.NetworkAdapters.push_back(Current);
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json ScsiDevices = RootJson["ScsiDevices"];

        for (nlohmann::json const& ScsiDevice : ScsiDevices)
        {
            NanaBox::ScsiDeviceConfiguration Current;

            try
            {
                Current.Enabled =
                    ScsiDevice["Enabled"].get<bool>();
            }
            catch (...)
            {
                Current.Enabled = false;
            }

            try
            {
                Current.Type =
                    ScsiDevice["Type"].get<NanaBox::ScsiDeviceType>();
            }
            catch (...)
            {
                continue;
            }

            Current.Path =
                ScsiDevice["Path"].get<std::string>();
            if (Current.Path.empty() &&
                Current.Type != NanaBox::ScsiDeviceType::VirtualImage)
            {
                continue;
            }

            Result.ScsiDevices.push_back(Current);
        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json SharedFolders = RootJson["SharedFolders"];

        for (nlohmann::json const& SharedFolder : SharedFolders)
        {
            try
            {
                NanaBox::SharedFolderConfiguration Current;

                try
                {
                    Current.Enabled =
                        SharedFolder["Enabled"].get<bool>();
                }
                catch (...)
                {
                    Current.Enabled = false;
                }

                try
                {
                    Current.ReadOnly =
                        SharedFolder["ReadOnly"].get<bool>();
                }
                catch (...)
                {
                    Current.ReadOnly = true;
                }

                Current.HostPath =
                    SharedFolder["HostPath"].get<std::string>();
                if (Current.HostPath.empty())
                {
                    continue;
                }

                Current.GuestName =
                    SharedFolder["GuestName"].get<std::string>();
                if (Current.GuestName.empty())
                {
                    continue;
                }

                Result.SharedFolders.push_back(Current);
            }
            catch (...)
            {

            }
        }
    }
    catch (...)
    {

    }

    try
    {
        Result.SecureBoot =
            RootJson["SecureBoot"].get<bool>();
    }
    catch (...)
    {
        Result.SecureBoot = false;
    }

    try
    {
        Result.Tpm =
            RootJson["Tpm"].get<bool>();
    }
    catch (...)
    {
        Result.Tpm = false;
    }

    try
    {
        Result.GuestStateFile =
            RootJson["GuestStateFile"].get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.RuntimeStateFile =
            RootJson["RuntimeStateFile"].get<std::string>();
    }
    catch (...)
    {

    }

    return Result;
}
