# ![NanaBox](Assets/NanaBox.png) NanaBox

[![GitHub Actions Build Status](https://github.com/M2Team/NanaBox/actions/workflows/BuildBinaries.yml/badge.svg?branch=main&event=push)](https://github.com/M2Team/NanaBox/actions/workflows/BuildBinaries.yml?query=event%3Apush+branch%3Amain)
[![Total Downloads](https://img.shields.io/github/downloads/M2Team/NanaBox/total)](https://github.com/M2Team/NanaBox/releases)

[![Windows Store - Release Channel](https://img.shields.io/badge/Windows%20Store-Release%20Channel-blue)](https://www.microsoft.com/store/apps/9NJXJSCB2JK0)
[![Windows Store - Preview Channel](https://img.shields.io/badge/Windows%20Store-Preview%20Channel-blue)](https://www.microsoft.com/store/apps/9NCBGTS09QJJ)

[![Latest Version - Release Channel](https://img.shields.io/github/v/release/M2Team/NanaBox?display_name=release&sort=date&color=%23a4a61d)](https://github.com/M2Team/NanaBox/releases/latest)
[![Latest Version - Preview Channel](https://img.shields.io/github/v/release/M2Team/NanaBox?include_prereleases&display_name=release&sort=date&color=%23a4a61d)](https://github.com/M2Team/NanaBox/releases)

[![Latest Release Downloads - Release Channel](https://img.shields.io/github/downloads/M2Team/NanaBox/latest/total)](https://github.com/M2Team/NanaBox/releases/latest)
[![Latest Release Downloads - Preview Channel](https://img.shields.io/github/downloads-pre/M2Team/NanaBox/latest/total)](https://github.com/M2Team/NanaBox/releases)

![Screenshot 1](Documents/Screenshot1.png)

![Screenshot 2](Documents/Screenshot2.png)

![Screenshot 3](Documents/Screenshot3.png)

NanaBox is a third-party lightweight XAML-based out-of-box-experience oriented
Hyper-V virtualization software based on Host Compute System API, Remote Desktop
ActiveX control and XAML Islands.

NanaBox is not a Hyper-V client because Host Compute System API is low-level API
of Hyper-V WMI Providers which used in Hyper-V Manager, and Host Compute System
API is stateless which not available to manage virtual machines listed in
Hyper-V Manager.

NanaBox chooses Host Compute System API instead of Hyper-V WMI Providers because
the author (Kenji Mouri) wants to have portable virtual machine configurations
and feels disgusting for registering virtual machine configurations into system.

NanaBox chooses define own JSON-based virtual machine configurations format
because Hyper-V's VMCX is a binary format and no documented format definitions
from Microsoft.

**All kinds of contributions will be appreciated. All suggestions, pull 
requests and issues are welcome.**

If you want to sponsor me, please read [this document to become a sponsor].

If you'd like me to add features or improvements ahead of time, please use
[paid services].

[this document to become a sponsor]: https://github.com/MouriNaruto/MouriNaruto/blob/main/Sponsor
[paid services]: https://github.com/MouriNaruto/MouriNaruto/blob/main/PaidServices.md

## Features

- Based on Host Compute System API as virtualization backend.
- Support Hyper-V Enhanced Session Mode for Windows Guests.
- Support Hyper-V Nested Virtualization.
- Support Hyper-V GPU paravirtualization solution a.k.a GPU-PV.
- Support expose COM ports and physical drives to the virtual machine.
- Support enables Secure Boot for virtual machine.
- Use JSON based format to support portable virtual machine.
- Packaging with MSIX for modern deployment experience.
- Modernize the UI with XAML Islands with dark and light mode support.
- Full High DPI and partial Accessibility support.
- Support full screen mode.
- Multi-language Support: English and Simplified Chinese.
- Support relative mouse mode in Windows 11 Version 24H2 or later Host OS.

## System Requirements

- Supported OS: Windows 10 Version 2004 (Build 19041) or later
- Supported Platforms: x64 and ARM64

## Known issues

- Due to Host Compute System API and Host Compute Network API limitations, 
  there are some limitations from them also applied to NanaBox:
  - NanaBox only supports create UEFI Class 3 or UEFI without CSM or Hyper-V 
    Gen 2 virtual machines.
  - TPM support for virtual machines created by NanaBox is not available.
  - NanaBox needs elevated privilege via UAC.
  - The automatic switch between Enhanced Session Mode and Basic Session Mode
    won't be implemented.
  - You shouldn't put your virtual machine configuration file at the root of
    the drive, or you will meet the access denied error.

Reference documents for known issues:

- [MD4: Notes for using Host Compute System API](https://github.com/MouriNaruto/MouriDocs/tree/main/docs/4)

## Development Roadmap

- Future Series (T.B.D.)
  - [ ] Add friendly virtual machine creation UI and settings UI.
  - [ ] Provide tool for Hyper-V vmcx migration. (Suggested by Belleve.)
  - [ ] Add support for setting RDP options with .rdp file. (Suggested by 
        awakecoding.)
  - [ ] Add support for connecting virtual machine with MsRdpEx from 
        Devolutions. (Suggested by awakecoding.)

## Documents

- [License](License.md)
- [Relevant People](Documents/People.md)
- [Privacy Policy](Documents/Privacy.md)
- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Release Notes](Documents/ReleaseNotes.md)
- [Versioning](Documents/Versioning.md)
- [NanaBox Configuration File Reference](Documents/ConfigurationReference.md)
- [NanaBox Sponsor Edition](Documents/SponsorEdition.md)
