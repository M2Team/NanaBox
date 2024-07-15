﻿#include "pch.h"
#include "ResizeVirtualHardDiskPage.h"
#if __has_include("ResizeVirtualHardDiskPage.g.cpp")
#include "ResizeVirtualHardDiskPage.g.cpp"
#endif

#include "Utils.h"

#include <regex>
#include <initguid.h>
#include <virtdisk.h>
#pragma comment(lib,"virtdisk.lib")

#include <ShObjIdl_core.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt
{
    using Windows::System::DispatcherQueuePriority;
}

namespace winrt::NanaBox::implementation
{
    ResizeVirtualHardDiskPage::ResizeVirtualHardDiskPage(
        _In_ HWND WindowHandle)
        : m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"ResizeVirtualHardDiskPage/GridTitleTextBlock/Text").c_str());
    }


    void ResizeVirtualHardDiskPage::InitializeComponent() {
        ResizeVirtualHardDiskPageT::InitializeComponent();

        this->m_DispatcherQueue =
            winrt::DispatcherQueue::GetForCurrentThread();
    }

    void ResizeVirtualHardDiskPage::FileNameBrowseButtonClickHandler(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e) {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);
        winrt::handle(Mile::CreateThread([=]()
        {
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

                static constexpr COMDLG_FILTERSPEC SupportedFileTypes[] =
                {
                    { L"VHDX (*.vhdx)", L"*.vhdx" },
                    { L"VHD (*.vhd)", L"*.vhd" }
                };

                winrt::check_hresult(FileDialog->SetFileTypes(
                    ARRAYSIZE(SupportedFileTypes), SupportedFileTypes));

                // Note: The array is 1-indexed
                winrt::check_hresult(FileDialog->SetFileTypeIndex(1));

                winrt::check_hresult(FileDialog->SetDefaultExtension(L"vhdx"));

                winrt::check_hresult(FileDialog->Show(this->m_WindowHandle));

                winrt::hstring FilePath;
                {
                    winrt::com_ptr<IShellItem> Result;
                    winrt::check_hresult(FileDialog->GetResult(Result.put()));

                    LPWSTR RawFilePath = nullptr;
                    winrt::check_hresult(Result->GetDisplayName(
                        SIGDN_FILESYSPATH,
                        &RawFilePath));
                    if (RawFilePath)
                    {
                        FilePath = winrt::to_hstring(RawFilePath);
                        ::CoTaskMemFree(RawFilePath);
                    }
                }
                if (FilePath.empty())
                {
                    return;
                }

                if (!this->m_DispatcherQueue)
                {
                    return;
                }
                this->m_DispatcherQueue.TryEnqueue(
                    winrt::DispatcherQueuePriority::Normal,
                    [=]()
                    {
                        this->FileNameTextBox().Text(FilePath);
                    });
            }
            catch (...)
            {

            }
        }));
    }

    void ResizeVirtualHardDiskPage::NaturalNumberTextBoxBeforeTextChanging(
        winrt::TextBox const& sender,
        winrt::TextBoxBeforeTextChangingEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (args.NewText().empty())
        {
            sender.Text(L"0");
            return;
        }

        if (!std::regex_match(
            args.NewText().c_str(),
            std::wregex(
                L"(|[[:digit:]]+)",
                std::regex_constants::icase)))
        {
            args.Cancel(true);
        }
    }

    void ResizeVirtualHardDiskPage::ResizeButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::hstring Path =
            this->FileNameTextBox().Text();
        std::uint64_t NewSize =
            std::stoull(this->SizeTextBox().Text().c_str());

        winrt::hstring SuccessInstructionText =
            Mile::WinRT::GetLocalizedString(
                L"ResizeVirtualHardDiskPage/SuccessInstructionText");
        winrt::hstring SuccessContentText =
            Mile::WinRT::GetLocalizedString(
                L"ResizeVirtualHardDiskPage/SuccessContentText");

        winrt::handle(Mile::CreateThread([=]()
        {
            HANDLE DiskHandle = INVALID_HANDLE_VALUE;

            VIRTUAL_STORAGE_TYPE StorageType;
            StorageType.DeviceId = VIRTUAL_STORAGE_TYPE_DEVICE_UNKNOWN;
            StorageType.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_UNKNOWN;

            OPEN_VIRTUAL_DISK_PARAMETERS Parameters;
            ZeroMemory(&Parameters, sizeof(Parameters));
            Parameters.Version = OPEN_VIRTUAL_DISK_VERSION_1;

            DWORD OpenError = OpenVirtualDisk(
                &StorageType,
                Path.c_str(),
                VIRTUAL_DISK_ACCESS_ALL,
                OPEN_VIRTUAL_DISK_FLAG_NONE, // select a better flag.
                &Parameters,
                &DiskHandle
            );

            if (ERROR_SUCCESS == OpenError) {
                GET_VIRTUAL_DISK_INFO Info;
                ZeroMemory(&Info, sizeof(Info));
                Info.Version = GET_VIRTUAL_DISK_INFO_SIZE;

                ULONG InfoSize = sizeof(GET_VIRTUAL_DISK_INFO);
                ULONG SizeUsed = 0;

                DWORD InfoError = GetVirtualDiskInformation(
                    DiskHandle,
                    &InfoSize,
                    &Info,
                    &SizeUsed
                );// ::GetVirtualDiskInformation();

                if (InfoError == ERROR_SUCCESS) {
                    UINT64 OldSize = 0L;
                    OldSize = Info.Size.VirtualSize;


                    DWORD ResizeError = ::SimpleResizeVirtualDisk(
                        Path.c_str(),
                        OldSize,
                        NewSize,
                        &DiskHandle);

                    if (ERROR_SUCCESS == ResizeError)
                    {
                        ::CloseHandle(DiskHandle);

                        ::ShowMessageDialog(
                            this->m_WindowHandle,
                            SuccessInstructionText.c_str(),
                            Mile::FormatWideString(
                                SuccessContentText.c_str(),
                                Path.c_str()).c_str());

                        ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
                    }
                    else
                    {
                        ::ShowErrorMessageDialog(
                            this->m_WindowHandle,
                            winrt::hresult_error(HRESULT_FROM_WIN32(ResizeError)));
                    }

                }
                else {
                    ::CloseHandle(DiskHandle);

                    ::ShowMessageDialog(
                        this->m_WindowHandle,
                        SuccessInstructionText.c_str(),
                        Mile::FormatWideString(
                            SuccessContentText.c_str(),
                            Path.c_str()).c_str());

                    ::PostMessageW(this->m_WindowHandle, WM_CLOSE, 0, 0);
                }


            }
            else {
                ::ShowErrorMessageDialog(
                    this->m_WindowHandle,
                    winrt::hresult_error(HRESULT_FROM_WIN32(OpenError)));
                
            }
        }));
    }

    void ResizeVirtualHardDiskPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
    
}
