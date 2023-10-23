/*
 * PROJECT:   NanaBox
 * FILE:      ConfigurationManager.h
 * PURPOSE:   Implementation for the Virtual Machine Configuration Manager
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "ConfigurationManager.h"

#include <json.hpp>

void NanaBox::ComputeSystemUpdateGpu(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    NanaBox::GpuConfiguration const& Configuration)
{
    nlohmann::json Result;

    Result["ResourcePath"] = "VirtualMachine/ComputeTopology/Gpu";
    Result["RequestType"] = "Update";

    Result["Settings"]["AssignmentMode"] = "Disabled";
    if (NanaBox::GpuAssignmentMode::Default == Configuration.AssignmentMode)
    {
        Result["Settings"]["AssignmentMode"] = "Default";
    }
    else if (NanaBox::GpuAssignmentMode::Mirror == Configuration.AssignmentMode)
    {
        Result["Settings"]["AssignmentMode"] = "Mirror";
    }
    else if (NanaBox::GpuAssignmentMode::List == Configuration.AssignmentMode)
    {
        if (!Configuration.SelectedDevices.empty())
        {
            Result["Settings"]["AssignmentMode"] = "List";
            nlohmann::json Devices;
            for (std::string const& Device : Configuration.SelectedDevices)
            {
                Devices[Device] = 0xffff;
            }
            Result["Settings"]["AssignmentRequest"] = Devices;
        }
    }
    Result["Settings"]["AllowVendorExtension"] = true;

    Instance->Modify(winrt::to_hstring(Result.dump()));
}
