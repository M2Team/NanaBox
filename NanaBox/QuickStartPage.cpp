#include "pch.h"
#include "QuickStartPage.h"
#if __has_include("QuickStartPage.g.cpp")
#include "QuickStartPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

#include <ShlObj.h>
#include <Shlwapi.h>

#include <PathCch.h>
#pragma comment(lib,"PathCch.lib")

#include "ConfigurationManager.h"

namespace winrt::NanaBox
{
    using namespace ::NanaBox;
}

namespace winrt::NanaBox::implementation
{
    QuickStartPage::QuickStartPage(
        _In_ HWND WindowHandle,
        _Out_opt_ std::wstring* ConfigurationFilePath) :
        m_WindowHandle(WindowHandle),
        m_ConfigurationFilePath(ConfigurationFilePath)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"QuickStartPage/GridTitleTextBlock/Text").c_str());

        HICON ApplicationIconHandle = reinterpret_cast<HICON>(::LoadImageW(
            ::GetModuleHandleW(nullptr),
            MAKEINTRESOURCE(IDI_NANABOX),
            IMAGE_ICON,
            256,
            256,
            LR_SHARED));
        if (ApplicationIconHandle)
        {
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                TRUE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                FALSE,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
        }
    }

    void QuickStartPage::InitializeComponent()
    {
        QuickStartPageT::InitializeComponent();
    }

    void QuickStartPage::LaunchVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
            try
            {
                if (!this->m_ConfigurationFilePath)
                {
                    return;
                }

                winrt::com_ptr<IFileDialog> FileDialog =
                    winrt::create_instance<IFileDialog>(CLSID_FileOpenDialog);

                DWORD Flags = 0;
                winrt::check_hresult(FileDialog->GetOptions(&Flags));
                Flags |= FOS_FORCEFILESYSTEM;
                Flags |= FOS_NOCHANGEDIR;
                Flags |= FOS_DONTADDTORECENT;
                winrt::check_hresult(FileDialog->SetOptions(Flags));

                std::wstring FileTypeName = Mile::FormatWideString(
                    L"%s (*.7b)",
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationFileTypeName",
                        L"[NanaBox Virtual Machine Configuration]").c_str());

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    {
                        FileTypeName.c_str(),
                        L"*.7b"
                    }
                };
                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes),
                    SupportedFileTypes));
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));
                winrt::check_hresult(FileDialog->SetDefaultExtension(L"7b"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                PWSTR FilePath = nullptr;
                winrt::check_hresult(Result->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &FilePath));
                if (FilePath)
                {
                    *this->m_ConfigurationFilePath = std::wstring(FilePath);
                    ::CoTaskMemFree(FilePath);
                }

                if (!this->m_ConfigurationFilePath->empty())
                {
                    ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
                }
            }
            catch (...)
            {

            }
        }));
    }

    void QuickStartPage::CreateVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
            std::wstring VirtualImageFilePath;
            try
            {
                winrt::com_ptr<IFileDialog> FileDialog =
                    winrt::create_instance<IFileDialog>(CLSID_FileOpenDialog);

                DWORD Flags = 0;
                winrt::check_hresult(FileDialog->GetOptions(&Flags));

                Flags |= FOS_FORCEFILESYSTEM;
                Flags |= FOS_NOCHANGEDIR;
                Flags |= FOS_DONTADDTORECENT;
                winrt::check_hresult(FileDialog->SetOptions(Flags));

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    { L"ISO (*.iso)", L"*.iso" }
                };

                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/VirtualImagePathTip",
                        L"[Please select a bootable ISO image for installing "
                        L"the OS on your virtual machine, or close this dialog "
                        L"to skip the setting]").c_str()));

                // Note: The array is 1-indexed
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

                winrt::check_hresult(FileDialog->SetDefaultExtension(L"iso"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                LPWSTR RawFilePath = nullptr;
                winrt::check_hresult(Result->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &RawFilePath));
                if (RawFilePath)
                {
                    VirtualImageFilePath = std::wstring(RawFilePath);
                    ::CoTaskMemFree(RawFilePath);
                }
            }
            catch (...)
            {

            }

            std::wstring VirtualDiskFilePath;
            try
            {
                winrt::com_ptr<IFileDialog> FileDialog =
                    winrt::create_instance<IFileDialog>(CLSID_FileOpenDialog);

                DWORD Flags = 0;
                winrt::check_hresult(FileDialog->GetOptions(&Flags));

                Flags |= FOS_FORCEFILESYSTEM;
                Flags |= FOS_NOCHANGEDIR;
                Flags |= FOS_DONTADDTORECENT;
                winrt::check_hresult(FileDialog->SetOptions(Flags));

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    { L"VHDX (*.vhdx)", L"*.vhdx" },
                    { L"VHD (*.vhd)", L"*.vhd" }
                };

                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/VirtualDiskPathTip",
                        L"[Please select a VHD or VHDX virtual hard disk for "
                        L"the storage of your virtual machine, or close this "
                        L"dialog to skip the setting]").c_str()));

                // Note: The array is 1-indexed
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

                winrt::check_hresult(FileDialog->SetDefaultExtension(L"vhdx"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                LPWSTR RawFilePath = nullptr;
                winrt::check_hresult(Result->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &RawFilePath));
                if (RawFilePath)
                {
                    VirtualDiskFilePath = std::wstring(RawFilePath);
                    ::CoTaskMemFree(RawFilePath);
                }
            }
            catch (...)
            {

            }

            std::wstring ConfigurationName;
            std::wstring ConfigurationFilePath;
            try
            {
                winrt::com_ptr<IFileDialog> FileDialog =
                    winrt::create_instance<IFileDialog>(CLSID_FileSaveDialog);

                DWORD Flags = 0;
                winrt::check_hresult(FileDialog->GetOptions(&Flags));

                Flags |= FOS_FORCEFILESYSTEM;
                Flags |= FOS_NOCHANGEDIR;
                Flags |= FOS_DONTADDTORECENT;
                winrt::check_hresult(FileDialog->SetOptions(Flags));

                std::wstring FileTypeName = Mile::FormatWideString(
                    L"%s (*.7b)",
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationFileTypeName",
                        L"[NanaBox Virtual Machine Configuration]").c_str());

                COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    {
                        FileTypeName.c_str(),
                        L"*.7b"
                    }
                };

                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationNameTip",
                        L"[Please specify the save location for the "
                        L"configuration file of your virtual machine]").c_str()));

                // Note: The array is 1-indexed
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

                winrt::check_hresult(FileDialog->SetDefaultExtension(L"7b"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                {
                    LPWSTR RawFileName = nullptr;
                    winrt::check_hresult(Result->GetDisplayName(
                        SIGDN_NORMALDISPLAY,
                        &RawFileName));
                    if (RawFileName)
                    {
                        ConfigurationName = std::wstring(RawFileName);
                        ::CoTaskMemFree(RawFileName);
                    }
                }

                {
                    LPWSTR RawFilePath = nullptr;
                    winrt::check_hresult(Result->GetDisplayName(
                        SIGDN_FILESYSPATH,
                        &RawFilePath));
                    if (RawFilePath)
                    {
                        ConfigurationFilePath = std::wstring(RawFilePath);
                        ::CoTaskMemFree(RawFilePath);
                    }
                }
            }
            catch (...)
            {

            }
            if (ConfigurationName.empty() || ConfigurationFilePath.empty())
            {
                return;
            }

            {
                wchar_t* Found = std::wcsrchr(&ConfigurationName[0], L'.');
                if (Found)
                {
                    Found[0] = L'\0';
                }
                ConfigurationName.resize(
                    std::wcslen(ConfigurationName.c_str()));
            }

            if (!VirtualImageFilePath.empty())
            {
                std::wstring RelativePath(MAX_PATH, L'\0');
                if (::PathRelativePathToW(
                    &RelativePath[0],
                    ConfigurationFilePath.c_str(),
                    FILE_ATTRIBUTE_NORMAL,
                    VirtualImageFilePath.c_str(),
                    FILE_ATTRIBUTE_NORMAL))
                {
                    RelativePath.resize(std::wcslen(RelativePath.c_str()));
                    VirtualImageFilePath = RelativePath;
                }
            }

            if (!VirtualDiskFilePath.empty())
            {
                std::wstring RelativePath(MAX_PATH, L'\0');
                if (::PathRelativePathToW(
                    &RelativePath[0],
                    ConfigurationFilePath.c_str(),
                    FILE_ATTRIBUTE_NORMAL,
                    VirtualDiskFilePath.c_str(),
                    FILE_ATTRIBUTE_NORMAL))
                {
                    RelativePath.resize(std::wcslen(RelativePath.c_str()));
                    VirtualDiskFilePath = RelativePath;
                }
            }

            try
            {
                NanaBox::VirtualMachineConfiguration Configuration;
                Configuration.GuestType = NanaBox::GuestType::Windows;
                Configuration.Name = Mile::ToString(CP_UTF8, ConfigurationName);
                Configuration.ProcessorCount = 2;
                Configuration.MemorySize = 2048;
                {
                    NanaBox::NetworkAdapterConfiguration NetworkAdapter;
                    NetworkAdapter.Connected = true;
                    Configuration.NetworkAdapters.push_back(NetworkAdapter);
                }
                if (!VirtualDiskFilePath.empty())
                {
                    NanaBox::ScsiDeviceConfiguration ScsiDevice;
                    ScsiDevice.Type = NanaBox::ScsiDeviceType::VirtualDisk;
                    ScsiDevice.Path = Mile::ToString(CP_UTF8, VirtualDiskFilePath);
                    Configuration.ScsiDevices.push_back(ScsiDevice);
                }
                if (!VirtualImageFilePath.empty())
                {
                    NanaBox::ScsiDeviceConfiguration ScsiDevice;
                    ScsiDevice.Type = NanaBox::ScsiDeviceType::VirtualImage;
                    ScsiDevice.Path = Mile::ToString(CP_UTF8, VirtualImageFilePath);
                    Configuration.ScsiDevices.push_back(ScsiDevice);
                }

                std::string Content =
                    NanaBox::SerializeConfiguration(Configuration);

                ::WriteAllTextToUtf8TextFile(ConfigurationFilePath, Content);

                winrt::hstring SuccessInstructionText =
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/SuccessInstructionText");
                winrt::hstring SuccessContentText =
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/SuccessContentText");

                ::ShowMessageDialog(
                    this->m_WindowHandle,
                    SuccessInstructionText.c_str(),
                    Mile::FormatWideString(
                        SuccessContentText.c_str(),
                        ConfigurationFilePath.c_str()).c_str());
            }
            catch (...)
            {
                winrt::hresult_error Exception = Mile::WinRT::ToHResultError();
                ::ShowErrorMessageDialog(this->m_WindowHandle, Exception);
            }
        }));
    }

    void QuickStartPage::CreateVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowNewVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::ResizeVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowResizeVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::CompactVirtualHardDiskButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowCompactVirtualHardDiskDialog(this->m_WindowHandle);
    }

    void QuickStartPage::DocumentationButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::LaunchDocumentation();
    }

    void QuickStartPage::AboutButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::ShowAboutDialog(this->m_WindowHandle);
    }
}
