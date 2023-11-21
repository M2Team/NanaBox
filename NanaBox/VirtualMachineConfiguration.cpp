/*
 * PROJECT:   NanaBox
 * FILE:      VirtualMachineConfiguration.cpp
 * PURPOSE:   Implementation for the Virtual Machine Configuration
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#include "VirtualMachineConfiguration.h"

#include <winrt/Windows.Foundation.h>

#include <json.hpp>

#include "ConfigurationManager.h"

#include "Utils.h"

namespace NanaBox
{
    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::GuestType, {
        { NanaBox::GuestType::Unknown, "Unknown" },
        { NanaBox::GuestType::Windows, "Windows" },
        { NanaBox::GuestType::Linux, "Linux" }
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(NanaBox::UefiConsoleMode, {
        { NanaBox::UefiConsoleMode::Disabled, "Disabled" },
        { NanaBox::UefiConsoleMode::Default, "Default" },
        { NanaBox::UefiConsoleMode::ComPort1, "ComPort1" },
        { NanaBox::UefiConsoleMode::ComPort2, "ComPort2" }
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

    nlohmann::json RootJson = ParsedJson.at("NanaBox");

    if ("VirtualMachine" !=
        RootJson.at("Type").get<std::string>())
    {
        throw std::exception(
            "Invalid Virtual Machine Configuration");
    }

    NanaBox::VirtualMachineConfiguration Result;

    try
    {
        Result.Version =
            RootJson.at("Version").get<std::uint32_t>();
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
        RootJson.at("GuestType").get<NanaBox::GuestType>();

    Result.Name =
        RootJson.at("Name").get<std::string>();

    try
    {
        Result.ProcessorCount =
            RootJson.at("ProcessorCount").get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception(
            "Invalid Processor Count");
    }

    try
    {
        Result.MemorySize =
            RootJson.at("MemorySize").get<std::uint32_t>();
    }
    catch (...)
    {
        throw std::exception("Invalid Memory Size");
    }

    try
    {
        nlohmann::json ComPorts = RootJson.at("ComPorts");

        try
        {
            Result.ComPorts.UefiConsole =
                ComPorts.at("UefiConsole").get<NanaBox::UefiConsoleMode>();
        }
        catch (...)
        {

        }

        try
        {
            Result.ComPorts.ComPort1 =
                ComPorts.at("ComPort1").get<std::string>();
        }
        catch (...)
        {

        }

        try
        {
            Result.ComPorts.ComPort2 =
                ComPorts.at("ComPort2").get<std::string>();
        }
        catch (...)
        {

        }
    }
    catch (...)
    {

    }

    try
    {
        nlohmann::json Gpu = RootJson.at("Gpu");

        Result.Gpu.AssignmentMode =
            Gpu.at("AssignmentMode").get<NanaBox::GpuAssignmentMode>();

        try
        {
            nlohmann::json SelectedDevices = Gpu.at("SelectedDevices");

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
        nlohmann::json NetworkAdapters = RootJson.at("NetworkAdapters");

        for (nlohmann::json const& NetworkAdapter : NetworkAdapters)
        {
            try
            {
                NanaBox::NetworkAdapterConfiguration Current;

                try
                {
                    Current.Connected =
                        NetworkAdapter.at("Connected").get<bool>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.MacAddress =
                        NetworkAdapter.at("MacAddress").get<std::string>();
                }
                catch (...)
                {

                }

                try
                {
                    Current.EndpointId =
                        NetworkAdapter.at("EndpointId").get<std::string>();
                }
                catch (...)
                {

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
        nlohmann::json ScsiDevices = RootJson.at("ScsiDevices");

        for (nlohmann::json const& ScsiDevice : ScsiDevices)
        {
            NanaBox::ScsiDeviceConfiguration Current;

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
        Result.SecureBoot =
            RootJson.at("SecureBoot").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Result.Tpm =
            RootJson.at("Tpm").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Result.GuestStateFile =
            RootJson.at("GuestStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.RuntimeStateFile =
            RootJson.at("RuntimeStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.SaveStateFile =
            RootJson.at("SaveStateFile").get<std::string>();
    }
    catch (...)
    {

    }

    try
    {
        Result.ExposeVirtualizationExtensions =
            RootJson.at("ExposeVirtualizationExtensions").get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        NanaBox::DeserializeKeyboardConfiguration(
            RootJson.at("Keyboard"),
            Result.Keyboard);
    }
    catch (...)
    {

    }

    try
    {
        NanaBox::DeserializeEnhancedSessionConfiguration(
            RootJson.at("EnhancedSession"),
            Result.EnhancedSession);
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
    {
        nlohmann::json ComPorts;
        ComPorts["UefiConsole"] = Configuration.ComPorts.UefiConsole;
        if (!Configuration.ComPorts.ComPort1.empty())
        {
            ComPorts["ComPort1"] = Configuration.ComPorts.ComPort1;
        }
        if (!Configuration.ComPorts.ComPort2.empty())
        {
            ComPorts["ComPort2"] = Configuration.ComPorts.ComPort2;
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
            Current["Connected"] = NetworkAdapter.Connected;
            if (!NetworkAdapter.MacAddress.empty())
            {
                Current["MacAddress"] = NetworkAdapter.MacAddress;
            }
            if (!NetworkAdapter.EndpointId.empty())
            {
                Current["EndpointId"] = NetworkAdapter.EndpointId;
            }
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
            Current["Type"] = ScsiDevice.Type;
            if (!ScsiDevice.Path.empty())
            {
                Current["Path"] = ScsiDevice.Path;
            }
            ScsiDevices.push_back(Current);
        }
        RootJson["ScsiDevices"] = ScsiDevices;
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
    if (!Configuration.SaveStateFile.empty())
    {
        RootJson["SaveStateFile"] = Configuration.SaveStateFile;
    }
    if (Configuration.ExposeVirtualizationExtensions)
    {
        RootJson["ExposeVirtualizationExtensions"] =
            Configuration.ExposeVirtualizationExtensions;
    }
    {
        nlohmann::json Keyboard =
            NanaBox::SerializeKeyboardConfiguration(
                Configuration.Keyboard);
        if (!Keyboard.empty())
        {
            RootJson["Keyboard"] = Keyboard;
        } 
    }
    {
        nlohmann::json EnhancedSession =
            NanaBox::SerializeEnhancedSessionConfiguration(
                Configuration.EnhancedSession);
        if (!EnhancedSession.empty())
        {
            RootJson["EnhancedSession"] = EnhancedSession;
        }
    }

    nlohmann::json Result;
    Result["NanaBox"] = RootJson;
    return Result.dump(2);
}

std::string NanaBox::MakeHcsConfiguration(
    NanaBox::VirtualMachineConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["SchemaVersion"]["Major"] = 2;
    Result["SchemaVersion"]["Minor"] = 1;

    Result["Owner"] = Configuration.Name;

    Result["ShouldTerminateOnLastHandleClosed"] = true;

    nlohmann::json Uefi;
    {
        switch (Configuration.ComPorts.UefiConsole)
        {
        case NanaBox::UefiConsoleMode::Default:
            Uefi["Console"] = "Default";
            break;
        case NanaBox::UefiConsoleMode::ComPort1:
            Uefi["Console"] = "ComPort1";
            break;
        case NanaBox::UefiConsoleMode::ComPort2:
            Uefi["Console"] = "ComPort2";
            break;
        default:
            Uefi["Console"] = "Disabled";
            break;
        }

        if (Configuration.SecureBoot)
        {
            Uefi["ApplySecureBootTemplate"] = "Apply";
            Uefi["SecureBootTemplateId"] =
                "1734c6e8-3154-4dda-ba5f-a874cc483422";
        }
    }
    Result["VirtualMachine"]["Chipset"]["Uefi"] = Uefi;

    nlohmann::json Memory;
    Memory["SizeInMB"] = Configuration.MemorySize;
    Memory["AllowOvercommit"] = true;
    Result["VirtualMachine"]["ComputeTopology"]["Memory"] = Memory;

    nlohmann::json Processor;
    Processor["Count"] = Configuration.ProcessorCount;
    if (Configuration.ExposeVirtualizationExtensions)
    {
        Processor["ExposeVirtualizationExtensions"] = true;
    }
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

        nlohmann::json ComPorts;
        if (!Configuration.ComPorts.ComPort1.empty())
        {
            ComPorts["0"] = NanaBox::MakeHcsComPortConfiguration(
                Configuration.ComPorts.ComPort1);
        }
        if (!Configuration.ComPorts.ComPort2.empty())
        {
            ComPorts["1"] = NanaBox::MakeHcsComPortConfiguration(
                Configuration.ComPorts.ComPort2);
        }
        Devices["ComPorts"] = ComPorts;

        if (!Configuration.NetworkAdapters.empty())
        {
            nlohmann::json NetworkAdapters;
            for (NanaBox::NetworkAdapterConfiguration const& NetworkAdapter
                : Configuration.NetworkAdapters)
            {
                NetworkAdapters[NetworkAdapter.EndpointId] =
                    NanaBox::MakeHcsNetworkAdapterConfiguration(NetworkAdapter);
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
                ScsiDevices[std::to_string(Count++)] =
                    NanaBox::MakeHcsScsiDeviceConfiguration(ScsiDevice);
            }
            nlohmann::json ScsiController;
            ScsiController["Attachments"] = ScsiDevices;
            Devices["Scsi"]["NanaBox Scsi Controller"] = ScsiController;
        }

        /*{
            std::string ShareName = "HostDriverStore";
            std::string SharePath = "C:\\Windows\\System32\\DriverStore";

            {
                nlohmann::json Current;
                Current["Name"] = ShareName;
                Current["Path"] = SharePath;

                Current["Options"]["ReadOnly"] = true;
                Current["Options"]["PseudoOplocks"] = true;
                Current["Options"]["PseudoDirnotify"] = true;
                Current["Options"]["SupportCloudFiles"] = true;

                nlohmann::json SharedFolders;
                SharedFolders.push_back(Current);
                Devices["VirtualSmb"]["Shares"] = SharedFolders;
            }

            {
                const std::uint32_t Plan9ShareFlagsReadOnly = 0x00000001;

                const std::uint32_t Plan9SharePort = 50001;

                nlohmann::json Current;
                Current["Name"] = ShareName;
                Current["AccessName"] = ShareName;
                Current["Path"] = SharePath;
                Current["Port"] = Plan9SharePort;
                Current["Flags"] = Plan9ShareFlagsReadOnly;

                nlohmann::json SharedFolders;
                SharedFolders.push_back(Current);
                Devices["Plan9"]["Shares"] = SharedFolders;
            }
        }*/
    }
    Result["VirtualMachine"]["Devices"] = Devices;

    if (Configuration.Tpm)
    {
        Result["VirtualMachine"]["SecuritySettings"]["EnableTpm"] = true;
    }

    if (!Configuration.GuestStateFile.empty())
    {
        Result["VirtualMachine"]["GuestState"]["GuestStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.GuestStateFile)));
    }

    if (!Configuration.RuntimeStateFile.empty())
    {
        Result["VirtualMachine"]["GuestState"]["RuntimeStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.RuntimeStateFile)));
    }

    if (!Configuration.SaveStateFile.empty())
    {
        Result["VirtualMachine"]["RestoreState"]["SaveStateFilePath"] =
            Mile::ToString(
                CP_UTF8,
                ::GetAbsolutePath(Mile::ToWideString(
                    CP_UTF8,
                    Configuration.SaveStateFile)));
    }

    return Result.dump(2);
}
