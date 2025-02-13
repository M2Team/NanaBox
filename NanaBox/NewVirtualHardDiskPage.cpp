#include "pch.h"
#include "NewVirtualHardDiskPage.h"
#if __has_include("NewVirtualHardDiskPage.g.cpp")
#include "NewVirtualHardDiskPage.g.cpp"
#endif

#include "Utils.h"

#include <regex>

#include <ShObjIdl_core.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt
{
    using Windows::System::DispatcherQueuePriority;
}

namespace winrt::NanaBox::implementation
{
    NewVirtualHardDiskPage::NewVirtualHardDiskPage(
        _In_ HWND WindowHandle) :
        m_WindowHandle(WindowHandle)
    {
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::WinRT::GetLocalizedString(
                L"NewVirtualHardDiskPage/GridTitleTextBlock/Text").c_str());
    }

    void NewVirtualHardDiskPage::InitializeComponent()
    {
        NewVirtualHardDiskPageT::InitializeComponent();

        this->m_DispatcherQueue =
            winrt::DispatcherQueue::GetForCurrentThread();
    }

    void NewVirtualHardDiskPage::NaturalNumberTextBoxBeforeTextChanging(
        winrt::TextBox const& sender,
        winrt::TextBoxBeforeTextChangingEventArgs const& args)
    {
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

    void NewVirtualHardDiskPage::FileNameBrowseButtonClickHandler(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        winrt::handle(Mile::CreateThread([=]()
        {
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

                COMDLG_FILTERSPEC SupportedFileTypes[] =
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

    void NewVirtualHardDiskPage::CreateButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        const std::uint64_t UnitFactors[] =
        {
            1ULL, // B
            1000ULL, // KB
            (1ULL << 10), // KiB
            1000000ULL, // MB
            (1ULL << 20), // MiB
            1000000000ULL, // GB
            (1ULL << 30), // GiB
            1000000000000ULL, // RB
            (1ULL << 40), // TiB
        };
        const std::size_t MaximumUnitFactorsIndex =
            sizeof(UnitFactors) / sizeof(*UnitFactors) - 1;

        winrt::hstring Path =
            this->FileNameTextBox().Text();
        std::uint64_t OriginalSize =
            std::stoull(this->SizeTextBox().Text().c_str());
        std::int32_t UnitIndex =
            this->SizeUnitComboBox().SelectedIndex();
        if (UnitIndex > MaximumUnitFactorsIndex)
        {
            UnitIndex = MaximumUnitFactorsIndex;
        }
        std::uint64_t Size = OriginalSize * UnitFactors[UnitIndex];

        winrt::hstring SuccessInstructionText =
            Mile::WinRT::GetLocalizedString(
                L"NewVirtualHardDiskPage/SuccessInstructionText");
        winrt::hstring SuccessContentText =
            Mile::WinRT::GetLocalizedString(
                L"NewVirtualHardDiskPage/SuccessContentText");

        winrt::handle(Mile::CreateThread([=]()
        {
            HANDLE DiskHandle = INVALID_HANDLE_VALUE;

            DWORD Error = ::SimpleCreateVirtualDisk(
                Path.c_str(),
                Size,
                &DiskHandle);
            if (ERROR_SUCCESS == Error)
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
                    winrt::hresult_error(HRESULT_FROM_WIN32(Error)));
            }
        }));
    }

    void NewVirtualHardDiskPage::CancelButtonClick(
        winrt::IInspectable const& sender,
        winrt::RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        ::DestroyWindow(this->m_WindowHandle);
    }
}
