# NanaBox Release Notes

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
