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

#include "Utils.h"

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

void NanaBox::ComputeSystemAddNetworkAdapters(
    winrt::com_ptr<NanaBox::ComputeSystem> const& Instance,
    std::string const& Owner,
    std::vector<NanaBox::NetworkAdapterConfiguration>& Configuration)
{
    NanaBox::HcnNetwork NetworkHandle = NanaBox::HcnOpenNetwork(
        NanaBox::DefaultSwitchId);

    std::string DefaultSwitchIdString = winrt::to_string(
        ::FromGuid(NanaBox::DefaultSwitchId));

    if (!Configuration.empty())
    {
        for (NanaBox::NetworkAdapterConfiguration& NetworkAdapter : Configuration)
        {
            if (!NetworkAdapter.Enabled)
            {
                continue;
            }

            GUID EndpointId;
            try
            {
                if (!NetworkAdapter.EndpointId.empty())
                {
                    EndpointId = winrt::guid(NetworkAdapter.EndpointId);
                }
            }
            catch (...)
            {

            }
            if (NetworkAdapter.EndpointId.empty())
            {
                winrt::check_hresult(::CoCreateGuid(&EndpointId));
                NetworkAdapter.EndpointId =
                    winrt::to_string(::FromGuid(EndpointId));
            }

            try
            {
                NanaBox::HcnDeleteEndpoint(EndpointId);
            }
            catch (...)
            {

            }

            NanaBox::HcnEndpoint EndpointHandle;
            nlohmann::json Settings;
            Settings["SchemaVersion"]["Major"] = 2;
            Settings["SchemaVersion"]["Minor"] = 0;
            Settings["Owner"] = Owner;
            Settings["HostComputeNetwork"] = DefaultSwitchIdString;
            if (!NetworkAdapter.MacAddress.empty())
            {
                Settings["MacAddress"] = NetworkAdapter.MacAddress;
            }

            EndpointHandle = NanaBox::HcnCreateEndpoint(
                NetworkHandle,
                EndpointId,
                winrt::to_hstring(Settings.dump()));

            nlohmann::json Properties = nlohmann::json::parse(winrt::to_string(
                NanaBox::HcnQueryEndpointProperties(EndpointHandle)));
            NetworkAdapter.MacAddress = Properties["MacAddress"];


            std::string ResourcePath = "VirtualMachine/Devices/NetworkAdapters/";
            ResourcePath += NetworkAdapter.EndpointId;

            nlohmann::json Result;

            Result["ResourcePath"] = ResourcePath;
            Result["RequestType"] = "Add";

            if (!NetworkAdapter.Connected)
            {
                Result["Settings"]["ConnectionState"] = "Disabled";
            }

            Result["Settings"]["EndpointId"] = NetworkAdapter.EndpointId;
            Result["Settings"]["MacAddress"] = NetworkAdapter.MacAddress;

            Instance->Modify(winrt::to_hstring(Result.dump()));
        }
    }
}

void NanaBox::RemoteDesktopUpdateKeyboardConfiguration(
    winrt::com_ptr<NanaBox::RdpClient> const& Instance,
    NanaBox::KeyboardConfiguration& Configuration)
{
    try
    {
        Instance->KeyboardHookMode(
            Configuration.RedirectKeyCombinations ? 1 : 2);
    }
    catch (...)
    {
        Configuration.RedirectKeyCombinations =
            (Instance->KeyboardHookMode() == 1);
    }

    try
    {
        Instance->HotKeyFullScreen(
            Configuration.FullScreenHotkey);
    }
    catch (...)
    {
        Configuration.FullScreenHotkey =
            Instance->HotKeyFullScreen();
    }

    try
    {
        Instance->HotKeyCtrlEsc(
            Configuration.CtrlEscHotkey);
    }
    catch (...)
    {
        Configuration.CtrlEscHotkey =
            Instance->HotKeyCtrlEsc();
    }

    try
    {
        Instance->HotKeyAltEsc(
            Configuration.AltEscHotkey);
    }
    catch (...)
    {
        Configuration.AltEscHotkey =
            Instance->HotKeyAltEsc();
    }

    try
    {
        Instance->HotKeyAltTab(
            Configuration.AltTabHotkey);
    }
    catch (...)
    {
        Configuration.AltTabHotkey =
            Instance->HotKeyAltTab();
    }

    try
    {
        Instance->HotKeyAltShiftTab(
            Configuration.AltShiftTabHotkey);
    }
    catch (...)
    {
        Configuration.AltShiftTabHotkey =
            Instance->HotKeyAltShiftTab();
    }

    try
    {
        Instance->HotKeyAltSpace(
            Configuration.AltSpaceHotkey);
    }
    catch (...)
    {
        Configuration.AltSpaceHotkey =
            Instance->HotKeyAltSpace();
    }

    try
    {
        Instance->HotKeyCtrlAltDel(
            Configuration.CtrlAltDelHotkey);
    }
    catch (...)
    {
        Configuration.CtrlAltDelHotkey =
            Instance->HotKeyCtrlAltDel();
    }

    try
    {
        Instance->HotKeyFocusReleaseLeft(
            Configuration.FocusReleaseLeftHotkey);
    }
    catch (...)
    {
        Configuration.FocusReleaseLeftHotkey =
            Instance->HotKeyFocusReleaseLeft();
    }

    try
    {
        Instance->HotKeyFocusReleaseRight(
            Configuration.FocusReleaseRightHotkey);
    }
    catch (...)
    {
        Configuration.FocusReleaseRightHotkey =
            Instance->HotKeyFocusReleaseRight();
    }
}

void NanaBox::DeserializeKeyboardConfiguration(
    nlohmann::json const& Input,
    NanaBox::KeyboardConfiguration& Output)
{
    try
    {
        Output.RedirectKeyCombinations =
            Input["RedirectKeyCombinations"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FullScreenHotkey =
            Input["FullScreenHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.CtrlEscHotkey =
            Input["CtrlEscHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltEscHotkey =
            Input["AltEscHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltTabHotkey =
            Input["AltTabHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltShiftTabHotkey =
            Input["AltShiftTabHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.AltSpaceHotkey =
            Input["AltSpaceHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.CtrlAltDelHotkey =
            Input["CtrlAltDelHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FocusReleaseLeftHotkey =
            Input["FocusReleaseLeftHotkey"].get<bool>();
    }
    catch (...)
    {

    }

    try
    {
        Output.FocusReleaseRightHotkey =
            Input["FocusReleaseRightHotkey"].get<bool>();
    }
    catch (...)
    {

    }
}
