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

namespace NanaBox
{
    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GuestType, {
        { NanaBox::GuestType::Unknown, "Unknown" },
        { NanaBox::GuestType::Windows, "Windows" },
        { NanaBox::GuestType::Linux, "Linux" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GpuAssignmentMode, {
        { NanaBox::GpuAssignmentMode::Disabled, "Disabled" },
        { NanaBox::GpuAssignmentMode::Default, "Default" },
        { NanaBox::GpuAssignmentMode::List, "List" },
        { NanaBox::GpuAssignmentMode::Mirror, "Mirror" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::ScsiDeviceType, {
        { NanaBox::ScsiDeviceType::VirtualDisk, "VirtualDisk" },
        { NanaBox::ScsiDeviceType::VirtualImage, "VirtualImage" },
        { NanaBox::ScsiDeviceType::PhysicalDevice, "PhysicalDevice" }
    })
}

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
                        NetworkAdapter.at("Enabled").get<bool>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.Connected =
                        NetworkAdapter.at("Connected").get<bool>();
                }
                catch (...)
                {

                }

                Current.MacAddress =
                    NetworkAdapter.at("MacAddress").get<std::string>();
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
                    ScsiDevice.at("Enabled").get<bool>();
            }
            catch (...)
            {

            }

            try
            {
                Current.Type =
                    ScsiDevice.at("Type").get<NanaBox::ScsiDeviceType>();
            }
            catch (...)
            {
                continue;
            }

            try
            {
                Current.Path = ScsiDevice.at("Path").get<std::string>();
            }
            catch (...)
            {

            }
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
                        SharedFolder.at("Enabled").get<bool>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.ReadOnly =
                        SharedFolder.at("ReadOnly").get<bool>();
                }
                catch (...)
                {

                }

                Current.HostPath =
                    SharedFolder.at("HostPath").get<std::string>();
                if (Current.HostPath.empty())
                {
                    continue;
                }

                Current.GuestName =
                    SharedFolder.at("GuestName").get<std::string>();
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

    }

    try
    {
        Result.Tpm =
            RootJson["Tpm"].get<bool>();
    }
    catch (...)
    {

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

std::string NanaBox::SerializeConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json RootJson;
    RootJson["Type"] = "VirtualMachine";
    RootJson["Version"] = Configuration.Version;
    RootJson["GuestType"] = Configuration.GuestType;
    RootJson["Name"] = Configuration.Name;
    RootJson["ProcessorCount"] = Configuration.ProcessorCount;
    RootJson["MemorySize"] = Configuration.MemorySize;
    if (!Configuration.ComPorts.empty())
    {
        nlohmann::json ComPorts;
        for (std::string const& ComPort
            : Configuration.ComPorts)
        {
            ComPorts.push_back(ComPort);
        }
        RootJson["ComPorts"] = ComPorts;
    }
    {
        nlohmann::json Gpu;
        Gpu["AssignmentMode"] = Configuration.Gpu.AssignmentMode;
        if (!Configuration.Gpu.SelectedDevices.empty())
        {
            nlohmann::json SelectedDevices;
            for (std::string const& SelectedDevice
                : Configuration.Gpu.SelectedDevices)
            {
                SelectedDevices.push_back(SelectedDevice);
            }
            Gpu["SelectedDevices"] = SelectedDevices;
        }
        RootJson["Gpu"] = Gpu;
    }
    if (!Configuration.NetworkAdapters.empty())
    {
        nlohmann::json NetworkAdapters;
        for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
            : Configuration.NetworkAdapters)
        {
            nlohmann::json Current;
            Current["Enabled"] = NetworkAdapter.Enabled;
            Current["Connected"] = NetworkAdapter.Connected;
            Current["MacAddress"] = NetworkAdapter.MacAddress;
            NetworkAdapters.push_back(Current);
        }
        RootJson["NetworkAdapters"] = NetworkAdapters;
    }
    if (!Configuration.ScsiDevices.empty())
    {
        nlohmann::json ScsiDevices;
        for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
            : Configuration.ScsiDevices)
        {
            nlohmann::json Current;
            Current["Enabled"] = ScsiDevice.Enabled;
            Current["Type"] = ScsiDevice.Type;
            if (!ScsiDevice.Path.empty())
            {
                Current["Path"] = ScsiDevice.Path;
            }
            ScsiDevices.push_back(Current);
        }
        RootJson["ScsiDevices"] = ScsiDevices;
    }
    if (!Configuration.SharedFolders.empty())
    {
        nlohmann::json SharedFolders;
        for (NanaBox::SharedFolderConfiguration const& SharedFolder
            : Configuration.SharedFolders)
        {
            nlohmann::json Current;
            Current["Enabled"] = SharedFolder.Enabled;
            Current["ReadOnly"] = SharedFolder.ReadOnly;
            Current["HostPath"] = SharedFolder.HostPath;
            Current["GuestName"] = SharedFolder.GuestName;
            SharedFolders.push_back(Current);
        }
        RootJson["SharedFolders"] = SharedFolders;
    }
    if (Configuration.SecureBoot)
    {
        RootJson["SecureBoot"] = Configuration.SecureBoot;
    }
    if (Configuration.Tpm)
    {
        RootJson["Tpm"] = Configuration.Tpm;
    }    
    if (!Configuration.GuestStateFile.empty())
    {
        RootJson["GuestStateFile"] = Configuration.GuestStateFile;
    }   
    if (!Configuration.RuntimeStateFile.empty())
    {
        RootJson["RuntimeStateFile"] = Configuration.RuntimeStateFile;
    }   

    nlohmann::json Result;
    Result["NanaBox"] = RootJson;
    return Result.dump(2);
}

std::string NanaBox::HcsGenerateConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["SchemaVersion"]["Major"] = 2;
    Result["SchemaVersion"]["Minor"] = 1;

    Result["Owner"] = Configuration.Name;

    Result["ShouldTerminateOnLastHandleClosed"] = true;

    nlohmann::json BootThis;
    BootThis["DeviceType"] = "ScsiDrive";
    BootThis["DevicePath"] = "NanaBox Scsi Controller";
    BootThis["DiskNumber"] = 0;
    Result["VirtualMachine"]["Chipset"]["Uefi"]["BootThis"] = BootThis;

    nlohmann::json Memory;
    Memory["SizeInMB"] = Configuration.MemorySize;
    Result["VirtualMachine"]["ComputeTopology"]["Memory"] = Memory;

    nlohmann::json Processor;
    Processor["Count"] = Configuration.ProcessorCount;
    Processor["ExposeVirtualizationExtensions"] = true;
    Result["VirtualMachine"]["ComputeTopology"]["Processor"] = Processor;

    // Note: Skip Configuration.Gpu because it need to add at runtime.

    nlohmann::json Devices;
    {
        nlohmann::json VideoMonitor;
        VideoMonitor["HorizontalResolution"] = 1024;
        VideoMonitor["VerticalResolution"] = 768;
        Devices["VideoMonitor"] = VideoMonitor;

        Devices["EnhancedModeVideo"] = nlohmann::json::object();

        Devices["Keyboard"] = nlohmann::json::object();

        Devices["Mouse"] = nlohmann::json::object();

        if (!Configuration.ComPorts.empty())
        {
            nlohmann::json ComPorts;
            std::uint32_t Count = 0;
            for (std::string const& ComPort : Configuration.ComPorts)
            {
                ComPorts[std::to_string(Count++)]["NamedPipe"] = ComPort;
            }
            Devices["ComPorts"] = ComPorts;
        }

        if (!Configuration.NetworkAdapters.empty())
        {
            nlohmann::json NetworkAdapters;
            for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
                : Configuration.NetworkAdapters)
            {
                if (!NetworkAdapter.Enabled)
                {
                    continue;
                }

                nlohmann::json Current;

                if (!NetworkAdapter.Connected)
                {
                    Current["ConnectionState"] = "Disabled";
                }

                Current["EndpointId"] = NetworkAdapter.EndpointId;
                Current["MacAddress"] = NetworkAdapter.MacAddress;

                NetworkAdapters[NetworkAdapter.EndpointId] = Current;
            }
            Devices["NetworkAdapters"] = NetworkAdapters;
        }

        if (!Configuration.ScsiDevices.empty())
        {
            nlohmann::json ScsiDevices;
            std::uint32_t Count = 0;
            for (NanaBox::ScsiDeviceConfiguration const& ScsiDevice
                : Configuration.ScsiDevices)
            {
                if (!ScsiDevice.Enabled)
                {
                    continue;
                }

                nlohmann::json Current;

                switch (ScsiDevice.Type)
                {
                case NanaBox::ScsiDeviceType::VirtualDisk:
                {
                    Current["Type"] = "VirtualDisk";
                    break;
                }
                case NanaBox::ScsiDeviceType::VirtualImage:
                {
                    Current["Type"] = "Iso";
                    break;
                }
                case NanaBox::ScsiDeviceType::PhysicalDevice:
                {
                    Current["Type"] = "PassThru";
                    break;
                }
                default:
                    continue;
                }
                Current["Path"] = ScsiDevice.Path;
                ScsiDevices[std::to_string(Count++)] = Current;
            }
            nlohmann::json ScsiController;
            ScsiController["Attachments"] = ScsiDevices;
            Devices["Scsi"]["NanaBox Scsi Controller"] = ScsiController;
        }

        if (!Configuration.SharedFolders.empty())
        {
            nlohmann::json SharedFolders;
            for (NanaBox::SharedFolderConfiguration const& SharedFolder
                : Configuration.SharedFolders)
            {
                if (!SharedFolder.Enabled)
                {
                    continue;
                }

                nlohmann::json Current;

                if (NanaBox::GuestType::Windows == Configuration.GuestType)
                {
                    Current["Name"] = SharedFolder.GuestName;
                    Current["Path"] = SharedFolder.HostPath;
                    if (SharedFolder.ReadOnly)
                    {
                        Current["Options"]["ReadOnly"] = true;
                        Current["Options"]["ShareRead"] = true;
                        Current["Options"]["CacheIo"] = true;    
                        Current["Options"]["PseudoOplocks"] = true;
                    }
                }
                else if (NanaBox::GuestType::Linux == Configuration.GuestType)
                {
                    const std::uint32_t Plan9Port = 564;
                    const std::uint32_t Plan9ReadOnly = 0x00000001;
                    const std::uint32_t Plan9LinuxMetadata = 0x00000004;

                    Current["Name"] = SharedFolder.GuestName;
                    Current["AccessName"] = SharedFolder.GuestName;
                    Current["Path"] = SharedFolder.HostPath;
                    Current["Port"] = Plan9Port;
                    std::uint32_t Flags = Plan9LinuxMetadata;
                    if (SharedFolder.ReadOnly)
                    {
                        Flags |= Plan9ReadOnly;
                    }
                    Current["Flags"] = Flags;    
                }

                SharedFolders.push_back(Current);
            }
            if (NanaBox::GuestType::Windows == Configuration.GuestType)
            {
                Devices["VirtualSmb"]["Shares"] = SharedFolders;
            }
            else if(NanaBox::GuestType::Linux == Configuration.GuestType)
            {
                Devices["Plan9"]["Shares"] = SharedFolders;
            }
        }
    }
    Result["VirtualMachine"]["Devices"] = Devices;

    if (Configuration.SecureBoot)
    {
        throw std::exception(
            "Not Implemented: Configuration.SecureBoot");
    }

    if (Configuration.Tpm)
    {
        throw std::exception(
            "Not Implemented: Configuration.Tpm");
    }

    if (!Configuration.GuestStateFile.empty())
    {
        throw std::exception(
            "Not Implemented: Configuration.GuestStateFile");
    }

    if (!Configuration.RuntimeStateFile.empty())
    {
        throw std::exception(
            "Not Implemented: Configuration.RuntimeStateFile");
    }

    return Result.dump(2);
}
