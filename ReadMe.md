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
Hyper-V client based on Host Compute System API, Remote Desktop ActiveX control
and XAML Islands.

**All kinds of contributions will be appreciated. All suggestions, pull 
requests and issues are welcome.**

## Features

- Based on Host Compute System API as virtualization backend.
- Support Hyper-V Enhanced Session Mode for Windows Guests.
- Support Hyper-V Nested Virtualization.
- Support Hyper-V GPU paravirtualization solution a.k.a GPU-PV.
- Support expose COM ports and physical drives to the virtual machine.
- Support enables Secure Boot for virtual machine.
- Use JSON based format to support portable virtual machine.
- Packaging with MSIX for modern deployment experience.
- Modernize the UI with XAML Islands with dark and light mode support. (Dynamic
  dark and light mode switching is supported for Windows 11 and later.)
- Full High DPI and partial Accessibility support.
- Support full screen mode.
- Multi-language Support: English and Simplified Chinese.

## System Requirements

- Supported OS: Windows 10 Version 2004 (Build 19041) or later
- Supported Platforms: x64 and ARM64

## Known issues

- The localization for the NanaBox UI is not implemented in NanaBox 1.0 Preview
  1.
- The virtual machine creation UI and settings UI is not implemented in NanaBox
  1.0 Preview 1.
- The automatic switch between Enhanced Session Mode and Basic Session Mode is 
  not implemented because HcsEventSystemRdpEnhancedModeStateChanged event can 
  be fired on at least Windows 10 Build 17763, but in Windows 11 Build 22621 
  cannot. It's confirmed as a HCS API bug via Kris Harper and will fix it in 
  the future.
- TPM support is not available because current stage of HCS API seems doesn't 
  implement them.
- NanaBox needs elevated privilege via UAC because HCN API in Windows 11 Build
  22621 will return ERROR_ACCESS_DENIED even add the current user to the Hyper-V
  Administrators user group. Windows 10 don't have that issue.
- Due to HCS API limitation, the guest only supports UEFI Class 3 or UEFI 
  without CSM or Hyper-V Gen 2 VM.

## Development Roadmap

- 1.0 Series (2023 Q2)
  - [x] Based on Host Compute System API as virtualization backend.
  - [x] Add Hyper-V Enhanced Session Mode support for Windows Guests.
  - [x] Add Hyper-V GPU paravirtualization solution a.k.a GPU-PV support.
  - [x] Add expose COM ports and physical drives to the virtual machine support.
  - [x] Add enable Secure Boot for virtual machine support.
  - [x] Add JSON based format to support portable virtual machine.
  - [x] Packaging with MSIX for modern deployment experience.
  - [x] Modernize the UI with XAML Islands with dark and light mode support. 
        (Dynamic dark and light mode switching is supported for Windows 11 and 
        later.)
  - [x] Add full High DPI and partial Accessibility support.
  - [x] Add full screen support. (Suggested by Belleve.)
  - [x] Add localization for the NanaBox UI.
  - [x] Submit to the Windows Store.
  - [ ] Add virtual machine creation UI and settings UI.
- 1.x Series (Late 2023)
  - [ ] Provide tool for Hyper-V vmcx migration. (Suggested by Belleve.)
  - [ ] Add support for setting RDP options with .rdp file. (Suggested by 
        awakecoding.)
  - [ ] Add support for connecting virtual machine with MsRdpEx from 
        Devolutions. (Suggested by awakecoding.)
- Future Series (T.B.D.)
  - Currently no new feature plans for this series.

## Documents

- [License](License.md)
- [Relevant People](Documents/People.md)
- [Privacy Policy](Documents/Privacy.md)
- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Release Notes](Documents/ReleaseNotes.md)
- [Versioning](Documents/Versioning.md)
- [NanaBox Configuration File Reference](Documents/ConfigurationReference.md)
