# NanaBox Release Notes

**NanaBox 1.3 Update 2 (1.3.866.0)**

- Add prevent shut down support because shut down automatically without exiting
  NanaBox may corrupt the user's virtual machine.
- Introducing the Compact Virtual Hard Disk feature. (Contributed by per1cycle.)
- Add waiting notice window for the Resize Virtual Hard Disk feature.
  (Contributed by per1cycle.)
- Defer package updates while the app is running in Windows 11 23H2 or later.
  (Suggested by AndromedaMelody.)
- Update Mile.Project.Windows submodule to the August 12, 2024 latest.
- Update VC-LTL to 5.1.1.
- Update YY-Thunks to 1.1.2.
- Update Mile.Windows.Helpers to 1.0.645.
- Update Mile.Xaml to 2.3.1064.

**NanaBox 1.3 Update 1 (1.3.838.0)**

- Try to introduce the GPU-P support. (Suggested by Z841973620.)
- Introducing the Resize Virtual Hard Disk feature. (Contributed by per1cycle.)
- Disable prompting for credentials for the RDP ActiveX control for not popuping
  the credentials window after powering off the virtual machine. (Thanks to
  MiroKaku.)
- Add size unit support for new virtual hard disk UI.
- Use the minimum block size for creating new virtual hard disk files for
  smaller dynamically expansion size in daily experience.

**NanaBox 1.3 (1.3.813.0)**

- Introduce JSON Schema support for NanaBox Configuration File. (Contributed by
  driver1998.)
- Add the virtual machine existence check for solving network unavailable issues
  when people reopen the same virtual machine accidentally.
- Eliminate exceptions when creating the virtual machine.
- Improve maintainer tools for introducing automatic packaging support when
  building all targets for NanaBox.

**NanaBox 1.2 Update 4 (1.2.803.0)**

- Support customize partial System Management BIOS (SMBIOS) information for
  virtual machines. (Contributed by Z841973620.)
- Ignore the text scale factor for solving the UI layout issues like
  https://github.com/M2Team/NanaBox/issues/10. (Feedbacked by prakharb5.)

**NanaBox 1.2 Update 3 (1.2.800.0)**

- Fix the auto resizing issue when using the Enhanced Session mode. (Feedbacked
  and confirmed by dongle-the-gadget. Thanks to Devolutions because of their
  good document from https://webdevolutions.blob.core.windows.net/blog/pdf/smart-resizing-and-high-dpi-issues-in-remote-desktop-manager.pdf.)
- Update documents.

**NanaBox 1.2 Update 2 (1.2.797.0)**

- Introduce the Trusted Platform Module (TPM) 2.0 support for virtual machines
  in Windows Server 2022 or later Host OS. (Tested under Windows 11 Version
  24H2.)
- Backport the documentation changes from 
  https://github.com/M2Team/NanaZip/pull/435. (Thanks to sirredbeard.)

**NanaBox 1.2 Update 1 (1.2.786.0)**

- Enable the relative mouse mode support in Windows 11 Version 24H2 or later
  Host OS for improving the compatibility for applications running in the
  virtual machines.
- Update the main window toolbar layout via following the NanaZip new toolbar
  design.
- Fix the access issue for RDP connection over named pipe support for both basic
  session and enhanced session.
- Update UI layout of the sponsor button. (Suggested by dongle-the-gadget and
  namazso.)
- NanaBox will only check the Sponsor Edition addon licensing status the
  first time you launch NanaBox or click the sponsor button to optimize
  the user experience.
- Update Mile.Xaml to 2.2.944.
- Update Mile.Windows.Helpers to 1.0.558.

**NanaBox 1.2 (1.2.769.0)**

- Enable display resolution mode selection on Windows Server 2022 or later.
- Expose RDP connection over named pipe support for both basic session and
  enhanced session.
- Fix keyboard configuration deserialization issue.
- Add EnableHostDriverStore option for GPU-PV settings.
- Add Sponsor button and Sponsor NanaBox UI.
- Improve several implementations.

**NanaBox 1.1 Update 2 (1.1.602.0)**

- Improve the user experience to make people using the virtual machine 
  configuration created by NanaBox without any modifications. (Suggested by 
  Jarung.)
- Improve the stability when using the file dialog.
- Make GPU-PV enabled virtual machine supports save the state.
- Improve the RDP client uninitialization process.

**NanaBox 1.1 Update 1 (1.1.601.0)**

- Fix the file dialog freeze in some cases on Windows 10. (Thanks to Jarung.)
- Fix reload virtual machine support when modifying the COM port settings.
- Fix reload virtual machine support when modifying the network adapter 
  settings.
- Fix virtual machine initialization issue when you have disabled network 
  adapters.

**NanaBox 1.1 (1.1.600.0)**

- Add the Quick Start user experience based on XAML.
- Add the Create Virtual Machine user experience based on XAML.
- Add the Create Virtual Hard Disk user experience based on XAML.
- Modernize the message report window based on XAML.
- Enhance the main window toolbar.
- Refresh the exit confirmation window implementation.
- Improve the stability after restoring virtual machine from suspend.
- Add keyboard and enhanced session configuration support and you can modify
  these settings at runtime.
- Add support for modifying memory size, COM port named pipe, GPU-PV, network 
  adapter, SCSI controller settings at runtime.
- Remove some unnecessary configuration options.
  - `Enabled` option in `NetworkAdapters` configuration.
  - `Enabled` option in `ScsiDevices` configuration.
- Improve the running as the unpackage mode implementation.
- Improve the virtual machine session mode switching implementation.
- Improve the implementation of prerequisite check.
- Improve the accessibility user experience.
- Remove the Shell New Submenu support because we have the new Create Virtual
  Machine user experience.
- Improve several implementations.

**NanaBox 1.0 (1.0.471.0)**

- Fix the background color issue when window size is bigger than the virtual
  machine content or using the fullscreen mode.
- Improve the background color policy when window size is bigger than the
  virtual machine content in windowed mode for better user experience.
  (Suggested by AndromedaMelody.)
- Enable frame buffer redirection for reducing the overhead.
- Enable redirect key combinations by default to improve the user experience.
- Fix virtual machine instance network disconnection issue when starting the
  same virtual machine instance repeatedly.
- Improve the implementation of running with unpackaged mode in package mode
  via using package's LocalState folder instead of user's Temp folder.
- Improve the user experience when switching NanaBox instances via Windows
  taskbar.
- Update to Git submodule version of Mile.Project.Windows.
- Update Mile.Windows.Helpers to 1.0.17.
  (https://github.com/ProjectMile/Mile.Windows.Helpers/commits/main)
- Update Mile.Xaml to 2.1.661.
  (https://github.com/ProjectMile/Mile.Xaml/releases/tag/2.1.661.0)

**NanaBox 1.0 Preview 2 (1.0.247.0)**

- Fix access denied issue when virtual disk and image files located under any
  subfolders under %SystemDrive%\Users. (#2) (Thanks to AndromedaMelody.)
- Add full screen support. (#4) (Suggested by Belleve. Contributed by 
  AndromedaMelody.)
- Fix issue when configuring the GPU-PV with the list assignment mode.
- Add overcommit support for virtual machines.
- Implement the multi-language support: English and Simplified Chinese.
- Improve the exit notice window layout. (Suggested by AndromedaMelody.)
- Update new original icon assets. (Designed by Shomnipotence.)
- Update Mile.Xaml to 1.1.434. 
  (https://github.com/ProjectMile/Mile.Xaml/releases/tag/1.1.434.0)
- Update Mile.Windows.Helpers to 1.0.9. 
  (https://github.com/ProjectMile/Mile.Windows.Helpers/commits/main)
- Implement the Shell New Submenu support for NanaBox Virtual Machine 
  Configuration File creation. (Suggested by Legna.)
- Update NanaBox Configuration File Reference documentation.
- Add support for running with unpackaged mode in package mode for resolving 
  cannot copy files from virtual machine to host file explorer. (#1) (Thanks 
  to AndromedaMelody.)
- Make exposing the virtualization extensions to the virtual machine option 
  available in the NanaBox Configuration File. So, people can use NanaBox on 
  some processors which is not support Nested Virtualization via Hyper-V. (#3) 
  (Suggested by driver1998.)

**NanaBox 1.0 Preview 1 (1.0.112.0)**

- Use Host Compute System API as virtualization backend.
- Packaging with MSIX for modern deployment experience.
- Modernize the UI with XAML Islands with dark and light mode support. (Dynamic
  dark and light mode switching is supported for Windows 11 and later.)
- Add full High DPI and partial Accessibility support.
