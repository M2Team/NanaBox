# NanaBox Configuration File Reference

## Overview

- File Extension: .7b
- Save Encoding: UTF-8 with BOM as mandatory
- Configuration Format: JSON (JavaScript Object Notation)
- JSON Schema: https://raw.githubusercontent.com/M2Team/NanaBox/main/Documents/ConfigurationSchema.json

```
{
    "NanaBox": {
        "Type": "VirtualMachine",
        "Version": 1,
        "GuestType": "Windows",
        "Name": "DemoVM",     
        "ProcessorCount": 4,
        "MemorySize": 2048,
        "ComPorts": {
            "UefiConsole": "ComPort1",
            "ComPort1": "\\\\.\\pipe\\vmpipe1",
            "ComPort2": "\\\\.\\pipe\\vmpipe2"
        },
        "Gpu": {
            "AssignmentMode": "List",
            "SelectedDevices": [
                "\\\\?\\PCI#VEN_10DE&DEV_1C82&SUBSYS_11BF1B4C&REV_A1#4&38ab2860&0&0008#{064092b3-625e-43bf-9eb5-dc845897dd59}\\GPUPARAV"
            ]
        },     
        "NetworkAdapters": [
            {
                "Connected": true,
                "MacAddress": "00-15-5D-64-2F-AB",
                "EndpointId": "f2288275-6c30-47d4-bc24-293fa9c9cb12"
            }
        ],
        "ScsiDevices": [
            {
                "Type": "VirtualDisk",
                "Path": "Test.vhdx"
            },
            {
                "Type": "VirtualImage",
                "Path": ""
            }
        ],
        "SecureBoot": true,
        "Tpm": true,
        "GuestStateFile": "Test.vmgs",
        "RuntimeStateFile": "Test.vmrs",
        "SaveStateFile": "Test.SaveState.vmrs",
        "ExposeVirtualizationExtensions": true
    }
}
```

## Reference

- NanaBox (Object)
  - Type (String)
  - Version (Number)
  - GuestType (String)
  - Name (String)
  - ProcessorCount (Number)
  - MemorySize (Number)
  - ComPorts (Object)
    - UefiConsole (String)
    - ComPort1 (String)
    - ComPort2 (String)
  - Gpu (Object)
    - AssignmentMode (String)
    - EnableHostDriverStore (Boolean)
    - SelectedDevices (String or Object Array)
      - DeviceInterface (String, Object Array Only)
      - PartitionId (String, Object Array Only)
  - NetworkAdapters (Object Array)
    - Connected (Boolean)
    - MacAddress (String)
    - EndpointId (String)
  - ScsiDevices (Object Array)
    - Type (String)
    - Path (String)
  - SecureBoot (Boolean)
  - Tpm (Boolean)
  - GuestStateFile (String)
  - RuntimeStateFile (String)
  - SaveStateFile (String)
  - ExposeVirtualizationExtensions (Boolean)
  - Keyboard (Object)
    - RedirectKeyCombinations (Boolean)
    - FullScreenHotkey (Number)
    - CtrlEscHotkey (Number)
    - AltEscHotkey (Number)
    - AltTabHotkey (Number)
    - AltShiftTabHotkey (Number)
    - AltSpaceHotkey (Number)
    - CtrlAltDelHotkey (Number)
    - FocusReleaseLeftHotkey (Number)
    - FocusReleaseRightHotkey (Number)
  - EnhancedSession (Object)
    - RedirectAudio (Boolean)
    - RedirectAudioCapture (Boolean)
    - RedirectDrives (Boolean)
    - RedirectPrinters (Boolean)
    - RedirectPorts (Boolean)
    - RedirectSmartCards (Boolean)
    - RedirectClipboard (Boolean)
    - RedirectDevices (Boolean)
    - RedirectPOSDevices (Boolean)
    - RedirectDynamicDrives (Boolean)
    - RedirectDynamicDevices (Boolean)
    - Drives (String Array)
    - Devices (String Array)
  - ChipsetInformation (Object)
    - BaseBoardSerialNumber (String)
    - ChassisSerialNumber (String)
    - ChassisAssetTag (String)
    - Manufacturer (String)
    - ProductName (String)
    - Version (String)
    - SerialNumber (String)
    - Family (String)
    - UUID (String)
    - SKUNumber (String)
  - VideoMonitor (Object)
    - HorizontalResolution (Number)
    - VerticalResolution (Number)
    - DisableBasicSessionDpiScaling (Boolean)
  - Policies (String Array)

### NanaBox

The parent object for all types of NanaBox Configuration File.

### Type

The type of configuration.

Available values: "VirtualMachine"

### Version

The version of virtual machine configuration. Only 1 is available.

Available values: 1

### GuestType

The guest OS type of virtual machine.

Available values: "Windows", "Linux" and "Unknown"

### Name

The unique name of virtual machine.

Example value: "TestVM"

### ProcessorCount

The processor count of virtual machine, in cores.

Example value: 2

### MemorySize

The memory size of virtual machine, in MB.

Example value: 2048

Note: You can update the memory size at runtime starting with NanaBox 1.1.

### ComPorts

The COM ports setting object of virtual machine.

#### UefiConsole

The UEFI serial console setting of virtual machine.

Available values: "Disabled", "Default", "ComPort1" and "ComPort2"

#### ComPort1

(Optional) The named pipe path of guest COM1 of virtual machine.

Example value: "\\\\.\\pipe\\vmpipe1"

Note: You can update the named pipe path at runtime starting with NanaBox 1.1.

#### ComPort2

(Optional) The named pipe path of guest COM2 of virtual machine.

Example value: "\\\\.\\pipe\\vmpipe2"

Note: You can update the named pipe path at runtime starting with NanaBox 1.1.

### Gpu

The GPU setting object of virtual machine. You can choose GPU-P (partitioning)
and/or GPU-PV (paravirtualization) modes for every specific GPU instance.

Note: Before NanaBox 1.3 Update 1, you can only use GPU-PV (paravirtualization)
mode for virtual machines.

Note: You can update GPU setting at runtime starting with NanaBox 1.1.

#### AssignmentMode

The GPU assignment mode setting of virtual machine.

Available values: "Disabled", "Default", "List" and "Mirror"

- "Disabled" mode: Do not assign any GPU to the virtual machine.
- "Default" mode: Assign the single default GPU with GPU-PV (paravirtualization)
  mode to virtual machine, which currently is POST GPU.
- "List" mode: Assign the GPU(s)/partition(s) specified in SelectedDevices to
  virtual machine. If SelectedDevices is empty, do not assign GPU to the virtual
  machine.
- "Mirror" mode: Assign all current and future GPUs with GPU-PV
  (paravirtualization) mode to virtual machine.

#### EnableHostDriverStore

(Optional) Set it true if you want to enable HostDriverStore folder readonly
share for both Virtual SMB and Plan 9 (Guest access name "HostDriverStore" with
Hyper-V Socket Port 50001), which can mirror the DriverStore folder from the
Host OS. It will be useful when users using GPU paravirtualization feature.

Note: You cannot suspend the virtual machine properly if this option is enabled.

Note: Available starting with NanaBox 1.2.

#### SelectedDevices

(Optional) The string or object array of selected GPUs used for virtual machine.
Set as the object with "DeviceInterface" and "PartitionId" for the specific GPU
instance if you want to use GPU-P (partitioning) mode, or set as the string
which contains "DeviceInterface" for using GPU-PV (paravirtualization) mode.

Note: Only valid in "List" GPU paravirtualization assignment mode.

Note: Before NanaBox 1.3 Update 1, you can only use GPU-PV (paravirtualization)
mode for virtual machines.

Reference: https://docs.microsoft.com/en-us/windows/win32/hyperv_v2/msvm-partitionablegpu

##### DeviceInterface

The device interface path of the selected GPU used for paravirtualization of
virtual machine.

Example value: "\\\\?\\PCI#VEN_10DE&DEV_1C82&SUBSYS_11BF1B4C&REV_A1#4&38ab2860&0&0008#{064092b3-625e-43bf-9eb5-dc845897dd59}\\GPUPARAV"

##### PartitionId

The partition ID of the selected GPU used for paravirtualization of virtual
machine. It's actually an uint16 value. Set it 65535 for using GPU-PV 
(paravirtualization) mode.

### NetworkAdapters

The network adapters setting object array of virtual machine.

Note: You can add, remove and update network adapters at runtime starting with
NanaBox 1.1.

#### Connected

Make the current network adapter connected if set it true.

#### MacAddress

(Optional) The MAC address of the current network adapter.

Note: If value not set, NanaBox will generate a new one for it.

Example value: "00-15-5D-64-2F-AB"

#### EndpointId

(Optional) The Endpoint GUID of the current network adapter.

Note: If value not set, NanaBox will generate a new one for it.

Note: This option is used for internal implementation.

Example value: "f2288275-6c30-47d4-bc24-293fa9c9cb12"

### ScsiDevices

The SCSI devices setting object array of virtual machine.

Note: You can add and update SCSI devices at runtime starting with NanaBox 1.1.

#### Type

The type of the current SCSI device.

Available values: "VirtualDisk", "VirtualImage" and "PhysicalDevice"

#### Path

The path of the current SCSI device.

Note: The relative path is supported.

When type is "VirtualDisk", you can use vhdx and vhd files.

When type is "VirtualImage", you can use iso files, and you can make it empty or
not set if you want to make a ejected virtual optical drive.

When type is "PhysicalDevice", you can expose your physical drive to virtual 
machine. you can set it something like "\\\\.\\PhysicalDriveX" where X is an 
integer that represents the particular enumeration of the physical disk on the 
caller's system.

### SecureBoot

(Optional) The Secure Boot setting of virtual machine.

If you want to enable Secure Boot for your virtual machine, please set it true.

### Tpm

(Optional) The Trusted Platform Module (TPM) setting of virtual machine.

If you want to enable Trusted Platform Module (TPM) for your virtual machine,
please set it true.

Note: Available starting with NanaBox 1.2 Update 2.

Note: Only the Trusted Platform Module (TPM) 2.0 is supported.

Note: You need Windows 11 Version 24H2 or later Host OS. (Although Windows
Server 2022 had introduced the related Host Compute System API interfaces,
but it seems doesn't be implemented.)

### GuestStateFile

(Optional) The path of guest state file for virtual machine.

Note: The relative path is supported.

Note: If value not set or file not exist, NanaBox will create a new one for it.

Example value: "TestVM.vmgs"

### RuntimeStateFile

(Optional) The path of runtime state file for virtual machine.

Note: The relative path is supported.

Note: If value not set or file not exist, NanaBox will create a new one for it.

Example value: "TestVM.vmrs"

### SaveStateFile

(Optional) The path of save state file for virtual machine.

Note: The relative path is supported.

Note: This option is used for internal implementation.

Example value: "TestVM.SaveState.vmrs"

### ExposeVirtualizationExtensions

(Optional) Expose the virtualization extensions to the virtual machine if set
it true.

Note: Some processors don't support exposing the virtualization extensions to
the virtual machine.

### Keyboard

(Optional) Keyboard setting object of virtual machine.

For more information about the default keyboard shortcut behavior, please read
https://learn.microsoft.com/en-us/windows/win32/termserv/terminal-services-shortcut-keys.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectKeyCombinations

(Optional) Apply key combinations at the virtual machine if set it true, or
apply key combinations to the virtual machine only when the host is running in
full-screen mode.

If you don't want to apply key combinations at the virtual machine, please set
it false.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### FullScreenHotkey

(Optional) Specifies the virtual-key code to add to CTRL+ALT to determine the
hotkey replacement for switching to full-screen mode.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### CtrlEscHotkey

(Optional) Specifies the virtual-key code to add to ALT to determine the hotkey
replacement for CTRL+ESC.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### AltEscHotkey

(Optional) Specifies the virtual-key code to add to ALT to determine the hotkey
replacement for ALT+ESC.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### AltTabHotkey

(Optional) Specifies the virtual-key code to add to ALT to determine the hotkey
replacement for ALT+TAB.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### AltShiftTabHotkey

(Optional) Specifies the virtual-key code to add to ALT to determine the hotkey
replacement for ALT+SHIFT+TAB.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### AltSpaceHotkey

(Optional) Specifies the virtual-key code to add to ALT to determine the hotkey
replacement for ALT+SPACE.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### CtrlAltDelHotkey

(Optional) Specifies the virtual-key code to add to CTRL+ALT to determine the
hotkey replacement for CTRL+ALT+DELETE, also called the secure attention
sequence (SAS).

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### FocusReleaseLeftHotkey

(Optional) Specifies the virtual-key code to add to Ctrl+Alt to determine the
hotkey replacement for Ctrl+Alt+Left Arrow.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

#### FocusReleaseRightHotkey

(Optional) Specifies the virtual-key code to add to Ctrl+Alt to determine the
hotkey replacement for Ctrl+Alt+Right Arrow.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

Note: You need to use the decimal value of the virtual-key code.

Note: For more information about virtual-key code, please read 
https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.

### EnhancedSession

(Optional) Enhanced session setting object of virtual machine.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectAudio

(Optional) Redirect sounds from the virtual machine to the host if set it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectAudioCapture

(Optional) Redirect audio capture from the host to the virtual machine if set it
true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectDrives

(Optional) Redirect all disk drives from the host to the virtual machine if set
it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectPrinters

(Optional) Redirect all printers from the host to the virtual machine if set it
true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectPorts

(Optional) Redirect all local ports (for example, COM and LPT) from the host to
the virtual machine if set it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectSmartCards

(Optional) Redirect all smart cards from the host to the virtual machine if set
it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectClipboard

(Optional) Redirect clipboard from the host to the virtual machine if set it
true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectDevices

(Optional) Redirect all devices from the host to the virtual machine if set it
true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectPOSDevices

(Optional) Redirect all Point of Service devices from the host to the virtual
machine if set it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectDynamicDrives

(Optional) Redirect all dynamically attached Plug and Play (PnP) drives that are
enumerated while virtual machine running if set it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### RedirectDynamicDevices

(Optional) Redirect all dynamically attached Plug and Play (PnP) devices that
are enumerated while virtual machine running if set it true.

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### Drives

(Optional) The string array of selected disk drives used for redirection from
the host to the virtual machine.

Example value: "C"

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

#### Devices

(Optional) The string array of selected devices used for redirection from the
host to the virtual machine.

Example value: "USB\\VID_5986&PID_211C&MI_00\\6&218C4A3&0&0000"

Note: Available starting with NanaBox 1.1 and you can modify these settings at
runtime.

### ChipsetInformation

The chipset information object of virtual machine.

Note: Available starting with NanaBox 1.2 Update 4.

#### BaseBoardSerialNumber

The serial number of the baseboard of virtual machine.

Example value: "QTFCOU0000001"

Note: Available starting with NanaBox 1.2 Update 4.

#### ChassisSerialNumber

The serial number of the chassis of virtual machine.

Example value: "QTFCOU0000001"

Note: Available starting with NanaBox 1.2 Update 4.

#### ChassisAssetTag 

The asset tag of the chassis of virtual machine.

Example value: "TestVM"

Note: Available starting with NanaBox 1.2 Update 4.

#### Manufacturer

The manufacturer of virtual machine.

Leave blank to use default vaule: "Microsoft Corporation"

Example value: "NVIDIA"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### ProductName

The product name of virtual machine.

Leave blank to use default vaule: "Virtual Machine"

Example value: "DGX-1"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### Version

The version of virtual machine.

Leave blank to use default vaule: "Hyper-V UEFI Release v4.1"

Example value: "DGX-1 with V100-32 1.0"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### SerialNumber

The serial number of virtual machine.

Leave blank to use default vaule

Example value: "QTFCOU0000001"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### Family

The family of virtual machine.

Leave blank to use default vaule: "Virtual Machine"

Example value: "Enterprise Server"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### UUID

The universally unique identifier of virtual machine.

Example value: "12345678-abcd-efgh-ijkl-0123456789ab"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

#### SKUNumber

The sku number of virtual machine.

Example value: "TestVM"

Note: You need Windows Server 2022 or later Host OS.

Note: Available starting with NanaBox 1.2 Update 4.

### VideoMonitor

The video monitor setting object of virtual machine.

Note: Available starting with NanaBox 1.5.

#### HorizontalResolution

(Optional) The horizontal resolution of the video monitor of virtual machine.

Example value: 1920

Note: Only the Basic Session Mode is constrainted with this setting.

Note: Available starting with NanaBox 1.5.

#### VerticalResolution

(Optional) The vertical resolution of the video monitor of virtual machine.

Example value: 1080

Note: Only the Basic Session Mode is constrainted with this setting.

Note: Available starting with NanaBox 1.5.

#### DisableBasicSessionDpiScaling

(Optional) Disable the DPI scaling for the Basic Session Mode of virtual
machine if set it true.

Note: Available starting with NanaBox 1.5 Update 2 and you can modify these
settings at runtime.

### Policies

The string array of policies of virtual machine.

Note: Available starting with NanaBox 1.5.

#### Available Policies starting with NanaBox 1.5

Note: These policies need Windows 11 Version 24H2 or later Host OS.

- LegacyPcrMeasurement
- DisableSha384Pcr
- LegacyRteWidth
- LegacyTrustedLaunchMemoryMap
- LegacyApicSelectionLogic
- LegacyApicManganeseOverride
- LegacyGpaLayout
- ForceLegacyHcl
- ForceCurrentHcl
- UseExtendedMaxVpPerSocketLimit

Example: If you want to do some hacks to boot x86 (64-Bit) Windows XP / Vista /
7 on Hyper-V Generation 2 Virtual Machines with NanaBox and Windows 11 Version
24H2 or later Host OS, you can set the following policies to help you do further
researches.

- LegacyRteWidth
- LegacyApicSelectionLogic
- LegacyApicManganeseOverride
- LegacyGpaLayout

## Samples

### Typical Windows Virtual Machine

```
{
  "NanaBox": {
    "ComPorts": {
      "UefiConsole": "Disabled"
    },
    "Gpu": {
      "AssignmentMode": "Mirror"
    },
    "GuestStateFile": "TestVM.vmgs",
    "GuestType": "Windows",
    "MemorySize": 4096,
    "Name": "TestVM",
    "NetworkAdapters": [
      {
        "Connected": true,
        "EndpointId": "cd67ed54-c64d-4c98-bc0a-6d5f38ca43ce",
        "MacAddress": "00-15-5D-64-2F-AB"
      }
    ],
    "ProcessorCount": 2,
    "RuntimeStateFile": "TestVM.vmrs",
    "ScsiDevices": [
      {
        "Path": "Windows 11.vhdx",
        "Type": "VirtualDisk"
      },
      {
        "Type": "VirtualImage"
      }
    ],
    "SecureBoot": true,
    "Type": "VirtualMachine",
    "Version": 1
  }
}
```

### Typical Ubuntu Virtual Machine

```
{
  "NanaBox": {
    "ComPorts": {
      "ComPort1": "",
      "ComPort2": "",
      "UefiConsole": "Disabled"
    },
    "Gpu": {
      "AssignmentMode": "Mirror"
    },
    "GuestType": "Linux",
    "MemorySize": 4096,
    "Name": "UbuntuVM",
    "NetworkAdapters": [
      {
        "Connected": true,
        "EndpointId": "594f35de-4d8e-4173-8e9f-00f2b372d2f4",
        "MacAddress": "00-15-5D-47-EB-71"
      }
    ],
    "ProcessorCount": 2,
    "ScsiDevices": [
      {
        "Path": "D:\\NanaBox VM\\UbuntuVM\\UbuntuVM.vhdx",
        "Type": "VirtualDisk"
      },
      {
        "Path": "D:\\Updates\\ubuntu-22.04-desktop-amd64.iso",
        "Type": "VirtualImage"
      }
    ],
    "Type": "VirtualMachine",
    "Version": 1
  }
}
```

### Typical UEFI Application Debugging Virtual Machine

```
{
  "NanaBox": {
    "ComPorts": {
      "ComPort1": "\\\\.\\pipe\\vmpipe1",
      "ComPort2": "\\\\.\\pipe\\vmpipe2",
      "UefiConsole": "ComPort1"
    },
    "Gpu": {
      "AssignmentMode": "Disabled"
    },
    "GuestStateFile": "UefiTestVM.vmgs",
    "GuestType": "Windows",
    "MemorySize": 512,
    "Name": "UefiTestVM",
    "NetworkAdapters": [
      {
        "Connected": true,
        "EndpointId": "d811c915-5421-4e31-afbf-5db608e7ad24",
        "MacAddress": "00-15-5D-47-EB-0D"
      }
    ],
    "ProcessorCount": 1,
    "RuntimeStateFile": "UefiTestVM.vmrs",
    "ScsiDevices": [
      {
        "Path": "D:\\NanaBox VM\\UEFIDebugDisk.vhdx",
        "Type": "VirtualDisk"
      }
    ],
    "Type": "VirtualMachine",
    "Version": 1
  }
}
```

### Typical NVIDIA DGX-1 Virtual Machine (In order to bypass the installation detection of the DGX BaseOS)

```
{
  "NanaBox": {
    "ChipsetInformation": {
      "Family": "Enterprise Server",
      "Manufacturer": "NVIDIA",
      "ProductName": "DGX-1",
      "SerialNumber": "QTFCOU0000001",
      "Version": "DGX-1 with V100-32 1.0"
    },
    "ComPorts": {
      "UefiConsole": "Disabled"
    },
    "Gpu": {
      "AssignmentMode": "List",
      "SelectedDevices": [
        "\\\\?\\PCI#VEN_10DE&DEV_1DB5&SUBSYS_12491215&REV_A1#4&38ab2860&0&0008#{064092b3-625e-43bf-9eb5-dc845897dd59}\\GPUPARAV"
      ]
    },
    "GuestType": "Linux",
    "MemorySize": 16384,
    "Name": "DGX OS",
    "NetworkAdapters": [
      {
        "Connected": true,
        "EndpointId": "594f35de-4d8e-4173-8e9f-00f2b372d2f4",
        "MacAddress": "00-15-5D-47-EB-71"
      }
    ],
    "ProcessorCount": 8,
    "ScsiDevices": [
      {
        "Path": "D:\\NanaBox VM\\DGX OS.vhdx",
        "Type": "VirtualDisk"
      },
      {
        "Path": "D:\\Updates\\DGXOS-5.6.0-2024-04-16-06-58-50.iso",
        "Type": "VirtualImage"
      }
    ],
    "Type": "VirtualMachine",
    "Version": 1
  }
}
```
