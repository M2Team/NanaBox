#include "pch.h"
#include "QuickStartPage.h"
#if __has_include("QuickStartPage.g.cpp")
#include "QuickStartPage.g.cpp"
#endif

#include "Utils.h"

#include "NanaBoxResources.h"

#include <ShlObj.h>

#include "VirtualMachineConfiguration.h"

namespace winrt::NanaBox
{
    using namespace ::NanaBox;
}

using namespace winrt;
using namespace Windows::UI::Xaml;

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

            std::wstring FileTypeName = Mile::FormatWideString(
                L"%s (*.7b)",
                Mile::WinRT::GetLocalizedString(
                    L"QuickStartPage/ConfigurationFileTypeName").c_str());

            static COMDLG_FILTERSPEC SupportedFileTypes[] =
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
            if (this->m_ConfigurationFilePath)
            {
                *this->m_ConfigurationFilePath = FilePath;
            }
            ::CoTaskMemFree(FilePath);
        }
        catch (...)
        {

        }

        if (this->m_ConfigurationFilePath)
        {
            if (!this->m_ConfigurationFilePath->empty())
            {
                ::DestroyWindow(this->m_WindowHandle);
            }
        }
    }

    void QuickStartPage::CreateVirtualMachineButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        std::wstring FilePath;

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
                    L"QuickStartPage/ConfigurationFileTypeName").c_str());

            static COMDLG_FILTERSPEC SupportedFileTypes[] =
            {
                {
                    FileTypeName.c_str(),
                    L"*.7b"
                }
            };

            winrt::check_hresult(FileDialog->SetFileTypes(
                ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

            // Note: The array is 1-indexed
            winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

            winrt::check_hresult(FileDialog->SetDefaultExtension(L"7b"));

            winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

            winrt::com_ptr<IShellItem> Result;
            winrt::check_hresult(FileDialog->GetResult(Result.put()));

            LPWSTR RawFilePath = nullptr;
            winrt::check_hresult(Result->GetDisplayName(
                SIGDN_FILESYSPATH,
                &RawFilePath));
            FilePath = std::wstring(RawFilePath);
            ::CoTaskMemFree(RawFilePath);
        }
        catch (...)
        {

        }

        if (FilePath.empty())
        {
            return;
        }

        winrt::handle(Mile::CreateThread([=]()
        {
            try
            {
                NanaBox::VirtualMachineConfiguration Configuration;
                Configuration.GuestType = NanaBox::GuestType::Windows;
                Configuration.Name = winrt::to_string(
                    Mile::WinRT::GetLocalizedString(
                        L"QuickStartPage/ConfigurationNameTip"));
                Configuration.ProcessorCount = 2;
                Configuration.MemorySize = 4096;
                {
                    NanaBox::NetworkAdapterConfiguration NetworkAdapter;
                    NetworkAdapter.Connected = true;
                    Configuration.NetworkAdapters.push_back(NetworkAdapter);
                }
                {
                    NanaBox::ScsiDeviceConfiguration ScsiDevice;
                    ScsiDevice.Enabled = true;
                    ScsiDevice.Type = NanaBox::ScsiDeviceType::VirtualDisk;
                    ScsiDevice.Path = winrt::to_string(
                        Mile::WinRT::GetLocalizedString(
                            L"QuickStartPage/VirtualDiskPathTip"));
                    Configuration.ScsiDevices.push_back(ScsiDevice);
                }
                {
                    NanaBox::ScsiDeviceConfiguration ScsiDevice;
                    ScsiDevice.Enabled = true;
                    ScsiDevice.Type = NanaBox::ScsiDeviceType::VirtualImage;
                    ScsiDevice.Path = winrt::to_string(
                        Mile::WinRT::GetLocalizedString(
                            L"QuickStartPage/VirtualImagePathTip"));
                    Configuration.ScsiDevices.push_back(ScsiDevice);
                }
                Configuration.SecureBoot = true;

                std::string Content =
                    NanaBox::SerializeConfiguration(Configuration);

                ::WriteAllTextToUtf8TextFile(FilePath, Content);

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
                        FilePath.c_str()).c_str());

                OPENASINFO OpenAsInfo = { 0 };
                OpenAsInfo.pcszFile = FilePath.c_str();
                OpenAsInfo.oaifInFlags = OAIF_EXEC;
                ::SHOpenWithDialog(this->m_WindowHandle, &OpenAsInfo);
            }
            catch (winrt::hresult_error const& ex)
            {
                ::ShowErrorMessageDialog(this->m_WindowHandle, ex);
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
