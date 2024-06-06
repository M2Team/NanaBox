# NanaBox Configuration File Reference

## Overview

- File Extension: .7b
- Save Encoding: UTF-8 with BOM as mandatory
- Configuration Format: JSON (JavaScript Object Notation)

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
    - SelectedDevices (String Array)
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
  - Keyboard (Object Array)
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
  - EnhancedSession (Object Array)
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

The GPU paravirtualization setting object of virtual machine.

Note: You can update GPU paravirtualization setting at runtime starting with
NanaBox 1.1.

#### AssignmentMode

The GPU paravirtualization assignment mode setting of virtual machine.

Available values: "Disabled", "Default", "List" and "Mirror"

#### EnableHostDriverStore

(Optional) Set it true if you want to enable HostDriverStore folder readonly
share for both Virtual SMB and Plan 9, which can mirror the DriverStore folder
from the Host OS. It will be useful when users using GPU paravirtualization
feature.

Note: You cannot suspend the virtual machine properly if this option is enabled.

Note: Available starting with NanaBox 1.2.

#### SelectedDevices

(Optional) The string array of selected GPUs used for paravirtualization of
virtual machine.

Note: Only valid in "List" GPU paravirtualization assignment mode.

Reference: https://docs.microsoft.com/en-us/windows/win32/hyperv_v2/msvm-partitionablegpu

Example value: "\\\\?\\PCI#VEN_10DE&DEV_1C82&SUBSYS_11BF1B4C&REV_A1#4&38ab2860&0&0008#{064092b3-625e-43bf-9eb5-dc845897dd59}\\GPUPARAV"

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

Note: You need Windows Server 2022 or later Host OS. (Tested under Windows 11
Version 24H2.)

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

(Optional) Keyboard setting object array of virtual machine.

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

(Optional) Enhanced session setting object array of virtual machine.

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
