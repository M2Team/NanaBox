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
                "Enabled": true,
                "Connected": true,
                "MacAddress": "00-15-5D-64-2F-AB",
                "EndpointId": "f2288275-6c30-47d4-bc24-293fa9c9cb12"
            }
        ],
        "ScsiDevices": [
            {
                "Enabled": true,
                "Type": "VirtualDisk",
                "Path": "Test.vhdx"
            },
            {
                "Enabled": true,
                "Type": "VirtualImage",
                "Path": ""
            }
        ],
        "SecureBoot": true,
        "Tpm": true,
        "GuestStateFile": "Test.vmgs",
        "RuntimeStateFile": "Test.vmrs",
        "SaveStateFile": "Test.SaveState.vmrs"
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
    - SelectedDevices (String Array)
  - NetworkAdapters (Object Array)
    - Enabled (Boolean)
    - Connected (Boolean)
    - MacAddress (String)
    - EndpointId (String)
  - ScsiDevices  (Object Array)
    - Enabled (Boolean)
    - Type (String)
    - Path (String)
  - SecureBoot (Boolean)
  - Tpm (Boolean)
  - GuestStateFile (String)
  - RuntimeStateFile (String)
  - SaveStateFile (String)

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

### ComPorts

The COM ports setting object of virtual machine.

#### UefiConsole

The UEFI serial console setting of virtual machine.

Available values: "Disabled", "Default", "ComPort1" and "ComPort2"

#### ComPort1

(Optional) The named pipe path of guest COM1 of virtual machine.

Example value: "\\\\.\\pipe\\vmpipe1"

#### ComPort2

(Optional) The named pipe path of guest COM2 of virtual machine.

Example value: "\\\\.\\pipe\\vmpipe2"

### Gpu

The GPU paravirtualization setting object of virtual machine.

#### AssignmentMode

The GPU paravirtualization assignment mode setting of virtual machine.

Available values: "Disabled", "Default", "List" and "Mirror"

#### SelectedDevices

(Optional) The string array selected GPU used for paravirtualization of virtual
machine.

Note: Only valid in "List" GPU paravirtualization assignment mode.

Reference: https://docs.microsoft.com/en-us/windows/win32/hyperv_v2/msvm-partitionablegpu

Example value: "\\\\?\\PCI#VEN_10DE&DEV_1C82&SUBSYS_11BF1B4C&REV_A1#4&38ab2860&0&0008#{064092b3-625e-43bf-9eb5-dc845897dd59}\\GPUPARAV"

### NetworkAdapters

The network adapters setting object array of virtual machine.

#### Enabled

Enable the current network adapter if set it true.

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

#### Enabled

Enable the current SCSI device if set it true.

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

(Optional) The TPM setting of virtual machine.

Note: TPM support is not available because Host Compute API seems doesn't 
implement them.

If you want to enable TPM for your virtual machine, please set it true.

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
        "Enabled": true,
        "EndpointId": "cd67ed54-c64d-4c98-bc0a-6d5f38ca43ce",
        "MacAddress": "00-15-5D-64-2F-AB"
      }
    ],
    "ProcessorCount": 2,
    "RuntimeStateFile": "TestVM.vmrs",
    "ScsiDevices": [
      {
        "Enabled": true,
        "Path": "Windows 11.vhdx",
        "Type": "VirtualDisk"
      },
      {
        "Enabled": true,
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
        "Enabled": true,
        "EndpointId": "594f35de-4d8e-4173-8e9f-00f2b372d2f4",
        "MacAddress": "00-15-5D-47-EB-71"
      }
    ],
    "ProcessorCount": 2,
    "ScsiDevices": [
      {
        "Enabled": true,
        "Path": "D:\\NanaBox VM\\UbuntuVM\\UbuntuVM.vhdx",
        "Type": "VirtualDisk"
      },
      {
        "Enabled": true,
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
        "Enabled": true,
        "EndpointId": "d811c915-5421-4e31-afbf-5db608e7ad24",
        "MacAddress": "00-15-5D-47-EB-0D"
      }
    ],
    "ProcessorCount": 1,
    "RuntimeStateFile": "UefiTestVM.vmrs",
    "ScsiDevices": [
      {
        "Enabled": true,
        "Path": "D:\\NanaBox VM\\UEFIDebugDisk.vhdx",
        "Type": "VirtualDisk"
      }
    ],
    "Type": "VirtualMachine",
    "Version": 1
  }
}
```
