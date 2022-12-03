# NanaBox Release Notes

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
