# NanaBox Release Notes

**NanaBox 1.1 (1.1.???.0)**

- Add the Quick Start user experience based on XAML.
- Add the Create Virtual Machine user experience based on XAML.
- Add the Create Virtual Hard Disk user experience based on XAML.
- Modernize the message report window based on XAML.
- Enhance the main window toolbar.
- Refresh the exit confirmation window implementation.
- Improve the stability after restoring virtual machine from suspend.
- Add keyboard and enhanced session configuration support.
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
