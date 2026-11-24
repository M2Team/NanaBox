#include "pch.h"
#include "VirtualMachineSettingsPage.h"
#if __has_include("VirtualMachineSettingsPage.g.cpp")
#include "VirtualMachineSettingsPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

#include <ShlObj.h>
#include <Shlwapi.h>

#include "ConfigurationManager.h"
#include "MainWindow.h"

namespace winrt::NanaBox::implementation
{
    VirtualMachineSettingsPage::VirtualMachineSettingsPage(
        _In_ HWND WindowHandle,
        std::wstring const& ConfigurationFilePath,
        std::wstring const& Name,
        _In_opt_ NanaBox::MainWindow* MainWindow) :
        m_WindowHandle(WindowHandle),
        m_ConfigurationFilePath(ConfigurationFilePath),
        m_MainWindow(MainWindow)
    {
        LPCWSTR TitleResource = ConfigurationFilePath.empty()
            ? L"VirtualMachineSettingsPage/CreateTitle"
            : L"VirtualMachineSettingsPage/Title";
        if (!Name.empty())
        {
            TitleResource = ConfigurationFilePath.empty()
                ? L"VirtualMachineSettingsPage/NamedCreateTitle"
                : L"VirtualMachineSettingsPage/NamedTitle";
        }
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::FormatWideString(
                Mile::WinRT::GetLocalizedString(TitleResource).c_str(),
                Name.c_str()).c_str());

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

    void VirtualMachineSettingsPage::InitializeComponent()
    {
        VirtualMachineSettingsPageT::InitializeComponent();

        this->OpenButton().IsEnabled(!this->m_ConfigurationFilePath.empty());
        this->ApplyButton().IsEnabled(this->m_MainWindow != nullptr);
        this->CreateButton().IsEnabled(this->m_ConfigurationFilePath.empty());
    }

    void VirtualMachineSettingsPage::OpenButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
            winrt::hstring SettingsInstructionText =
                Mile::WinRT::GetLocalizedString(
                    L"VirtualMachineSettingsPage/SettingsInstructionText");
            winrt::hstring SettingsContentText =
                Mile::WinRT::GetLocalizedString(
                    L"VirtualMachineSettingsPage/SettingsContentText");

            ::ShowMessageDialog(
                this->m_WindowHandle,
                SettingsInstructionText.c_str(),
                SettingsContentText.c_str());

            OPENASINFO OpenAsInfo = {};
            OpenAsInfo.pcszFile = this->m_ConfigurationFilePath.c_str();
            OpenAsInfo.oaifInFlags = OAIF_EXEC;
            ::SHOpenWithDialog(this->m_WindowHandle, &OpenAsInfo);
        }));
    }

    void VirtualMachineSettingsPage::ReferenceButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::LaunchDocumentation();
    }

    void VirtualMachineSettingsPage::ApplyButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        if (this->m_MainWindow)
        {
            this->m_MainWindow->ReloadVirtualMachineSettings();
        }
    }

    void VirtualMachineSettingsPage::CreateButtonClick(
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
                    ARRAYSIZE(SupportedFileTypes),
                    SupportedFileTypes));

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"VirtualMachineSettingsPage/VirtualImagePathTip",
                        L"[Please select a bootable ISO image for installing "
                        L"the OS on your virtual machine, or close this dialog "
                        L"to skip the setting]").c_str()));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                VirtualImageFilePath = ::GetFileSystemPathFromFileDialog(
                    FileDialog.get());
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
                    {
                        L"Virtual Hard Disk Files (*.vhd;*.vhdx)",
                        L"*.vhd;*.vhdx"
                    }
                };
                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes),
                    SupportedFileTypes));

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"VirtualMachineSettingsPage/VirtualDiskPathTip",
                        L"[Please select a VHD or VHDX virtual hard disk for "
                        L"the storage of your virtual machine, or close this "
                        L"dialog to skip the setting]").c_str()));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                VirtualDiskFilePath = ::GetFileSystemPathFromFileDialog(
                    FileDialog.get());
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
                        L"VirtualMachineSettingsPage/ConfigurationFileTypeName",
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

                winrt::check_hresult(FileDialog->SetTitle(
                    Mile::WinRT::GetLocalizedString(
                        L"VirtualMachineSettingsPage/ConfigurationNameTip",
                        L"[Please specify the save location for the "
                        L"configuration file of your virtual machine]").c_str()));

                // Note: The array is 1-indexed
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

                winrt::check_hresult(FileDialog->SetDefaultExtension(L"7b"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::com_ptr<IShellItem> Result;
                winrt::check_hresult(FileDialog->GetResult(Result.put()));

                ConfigurationName = ::GetDisplayNameFromShellItem(
                    Result.get(),
                    SIGDN_NORMALDISPLAY);

                ConfigurationFilePath = ::GetDisplayNameFromShellItem(
                    Result.get(),
                    SIGDN_FILESYSPATH);
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

                ::SetWindowTextW(
                    this->m_WindowHandle,
                    Mile::FormatWideString(
                        Mile::WinRT::GetLocalizedString(
                            L"VirtualMachineSettingsPage/"
                            L"NamedCreateTitle").c_str(),
                        ConfigurationName.c_str()).c_str());

                winrt::hstring SuccessInstructionText =
                    Mile::WinRT::GetLocalizedString(
                        L"VirtualMachineSettingsPage/SuccessInstructionText");
                winrt::hstring SuccessContentText =
                    Mile::WinRT::GetLocalizedString(
                        L"VirtualMachineSettingsPage/SuccessContentText");

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
}
