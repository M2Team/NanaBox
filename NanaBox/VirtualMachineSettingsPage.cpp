#include "pch.h"
#include "VirtualMachineSettingsPage.h"
#if __has_include("VirtualMachineSettingsPage.g.cpp")
#include "VirtualMachineSettingsPage.g.cpp"
#endif

#include "Utils.h"
#include "ConfigurationManager.h"
#include "NanaBoxResources.h"
#include "MainWindow.h"

#include <algorithm>
#include <charconv>
#include <cwchar>
#include <map>
#include <sstream>
#include <set>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Core.h>

namespace winrt
{
    using Windows::UI::Core::CoreDispatcherPriority;
    using Windows::UI::Xaml::Visibility;
    using Windows::UI::Xaml::ThicknessHelper;
    using Windows::UI::Xaml::TextWrapping;
    using Windows::UI::Xaml::FrameworkElement;
    using Windows::UI::Xaml::GridLengthHelper;
    using Windows::UI::Xaml::HorizontalAlignment;
    using Windows::UI::Xaml::VerticalAlignment;
    using Windows::UI::Xaml::Automation::AutomationProperties;
    using Windows::UI::Xaml::Controls::ColumnDefinition;
    using Windows::UI::Xaml::Controls::Grid;
    using Windows::UI::Xaml::Controls::SelectionChangedEventArgs;
    using Windows::UI::Xaml::Controls::TextBlock;
    using Windows::UI::Xaml::Controls::ToolTipService;
}

namespace
{
    winrt::hstring GetSettingsString(
        _In_ LPCWSTR Name)
    {
        std::wstring ResourcePath = L"VirtualMachineSettingsPage/";
        ResourcePath.append(Name);
        return Mile::WinRT::GetLocalizedString(winrt::hstring(ResourcePath));
    }

    void ShowSettingsError(
        _In_ HWND WindowHandle)
    {
        ::ShowErrorMessageDialog(WindowHandle, Mile::WinRT::ToHResultError());
    }

    std::wstring GetRelativeFilePath(
        std::wstring const& ConfigurationFilePath,
        std::wstring const& FilePath)
    {
        if (!ConfigurationFilePath.empty() && !FilePath.empty() &&
            !::PathIsRelativeW(FilePath.c_str()))
        {
            std::wstring RelativePath(MAX_PATH, L'\0');
            if (::PathRelativePathToW(
                &RelativePath[0],
                ConfigurationFilePath.c_str(),
                FILE_ATTRIBUTE_NORMAL,
                FilePath.c_str(),
                FILE_ATTRIBUTE_NORMAL))
            {
                RelativePath.resize(std::wcslen(RelativePath.c_str()));
                return RelativePath;
            }
        }
        return FilePath;
    }

    void InvalidSetting(
        winrt::hstring const& Name)
    {
        throw winrt::hresult_invalid_argument(winrt::hstring(
            Mile::FormatWideString(
                ::GetSettingsString(L"InvalidValue").c_str(),
                Name.c_str())));
    }

    template <typename ValueType>
    ValueType ReadSettingInteger(
        winrt::TextBox const& Input)
    {
        std::string Text = winrt::to_string(Input.Text());
        ValueType Value = 0;
        const char* End = Text.data() + Text.size();
        std::from_chars_result Result = std::from_chars(
            Text.data(),
            End,
            Value);
        if (Result.ec != std::errc() || Result.ptr != End)
        {
            ::InvalidSetting(winrt::AutomationProperties::GetName(Input));
        }
        return Value;
    }

    std::int32_t ReadSettingIndex(
        winrt::ComboBox const& Input)
    {
        std::int32_t Index = Input.SelectedIndex();
        if (Index < 0 ||
            static_cast<std::uint32_t>(Index) >= Input.Items().Size())
        {
            ::InvalidSetting(winrt::AutomationProperties::GetName(Input));
        }
        return Index;
    }

    std::vector<std::string> ReadSettingLines(
        winrt::TextBox const& Input)
    {
        std::vector<std::string> Result;
        std::string Text = winrt::to_string(Input.Text());
        std::replace(Text.begin(), Text.end(), '\r', '\n');
        std::istringstream Stream(Text);
        std::string Line;
        while (std::getline(Stream, Line))
        {
            if (!Line.empty())
            {
                Result.push_back(Line);
            }
        }
        return Result;
    }

    winrt::hstring MakeSettingLines(
        std::vector<std::string> const& Values)
    {
        std::string Result;
        for (std::string const& Value : Values)
        {
            if (!Result.empty())
            {
                Result.push_back('\n');
            }
            Result.append(Value);
        }
        return winrt::to_hstring(Result);
    }

    winrt::Grid AddSettingRow(
        winrt::StackPanel const& Panel,
        winrt::FrameworkElement const& Input,
        _In_ LPCWSTR LabelResource,
        _In_ LPCWSTR HelpResource,
        _In_ bool Visible)
    {
        winrt::Grid Row;
        Row.Visibility(Visible
            ? winrt::Visibility::Visible
            : winrt::Visibility::Collapsed);
        winrt::ColumnDefinition LabelColumn;
        LabelColumn.Width(winrt::GridLengthHelper::FromPixels(200));
        Row.ColumnDefinitions().Append(LabelColumn);
        Row.ColumnDefinitions().Append(winrt::ColumnDefinition());

        winrt::TextBlock Label;
        Label.Text(::GetSettingsString(LabelResource));
        Label.TextWrapping(winrt::TextWrapping::Wrap);
        Label.VerticalAlignment(winrt::VerticalAlignment::Center);
        Label.Margin(winrt::ThicknessHelper::FromLengths(0, 0, 12, 0));
        Row.Children().Append(Label);

        winrt::hstring Help = ::GetSettingsString(HelpResource);
        winrt::AutomationProperties::SetName(Input, Label.Text());
        winrt::AutomationProperties::SetHelpText(Input, Help);
        winrt::ToolTipService::SetToolTip(Input, winrt::box_value(Help));
        winrt::Grid::SetColumn(Input, 1);
        Row.Children().Append(Input);
        Panel.Children().Append(Row);
        return Row;
    }

    winrt::TextBox AddSettingTextBox(
        winrt::StackPanel const& Panel,
        _In_ LPCWSTR LabelResource,
        _In_ LPCWSTR HelpResource,
        winrt::hstring const& Value,
        _In_ bool Hidden)
    {
        winrt::TextBox Input;
        Input.Text(Value);
        Input.HorizontalAlignment(winrt::HorizontalAlignment::Stretch);
        ::AddSettingRow(Panel, Input, LabelResource, HelpResource, !Hidden);
        return Input;
    }

    winrt::CheckBox AddSettingCheckBox(
        winrt::StackPanel const& Panel,
        _In_ LPCWSTR LabelResource,
        _In_ LPCWSTR HelpResource,
        _In_ bool Value,
        _In_ bool Hidden)
    {
        winrt::CheckBox Input;
        Input.IsChecked(Value);
        Input.HorizontalAlignment(winrt::HorizontalAlignment::Left);
        ::AddSettingRow(Panel, Input, LabelResource, HelpResource, !Hidden);
        return Input;
    }

    std::vector<std::string> ReadRedirectionSelection(
        winrt::StackPanel const& Panel)
    {
        std::vector<std::string> Result;
        for (winrt::IInspectable const& Child : Panel.Children())
        {
            winrt::CheckBox Choice = Child.as<winrt::CheckBox>();
            if (Choice.IsChecked().Value())
            {
                Result.push_back(winrt::to_string(
                    winrt::unbox_value<winrt::hstring>(Choice.Tag())));
            }
        }
        return Result;
    }

    void PopulateRedirectionChoices(
        winrt::StackPanel const& Panel,
        std::map<std::string, winrt::hstring> const& Available,
        std::vector<std::string> const& Selected)
    {
        std::map<std::string, winrt::hstring> Choices = Available;
        for (std::string const& Key : Selected)
        {
            if (Choices.end() == Choices.find(Key))
            {
                Choices[Key] = winrt::hstring(Mile::FormatWideString(
                    ::GetSettingsString(L"Unavailable").c_str(),
                    winrt::to_hstring(Key).c_str()));
            }
        }

        winrt::StackPanel Updated;
        for (auto const& Choice : Choices)
        {
            winrt::CheckBox Input;
            Input.Content(winrt::box_value(Choice.second));
            Input.Tag(winrt::box_value(winrt::to_hstring(Choice.first)));
            Input.IsChecked(Selected.end() != std::find(
                Selected.begin(), Selected.end(), Choice.first));
            Updated.Children().Append(Input);
        }
        Panel.Children().Clear();
        while (Updated.Children().Size())
        {
            auto Child = Updated.Children().GetAt(0);
            Updated.Children().RemoveAt(0);
            Panel.Children().Append(Child);
        }
    }
}

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
        this->m_Configuration.Name = winrt::to_string(Name);
        if (ConfigurationFilePath.empty())
        {
            this->m_Configuration.GuestType = NanaBox::GuestType::Windows;
            this->m_Configuration.ProcessorCount = 2;
            this->m_Configuration.MemorySize = 2048;
            NanaBox::NetworkAdapterConfiguration NetworkAdapter;
            NetworkAdapter.Connected = true;
            this->m_Configuration.NetworkAdapters.push_back(NetworkAdapter);
        }
        this->UpdateWindowTitle();

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
                ICON_BIG,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
            ::SendMessageW(
                this->m_WindowHandle,
                WM_SETICON,
                ICON_SMALL,
                reinterpret_cast<LPARAM>(ApplicationIconHandle));
        }
    }

    VirtualMachineSettingsPage::~VirtualMachineSettingsPage()
    {
        if (auto Client = this->m_ApplyClient.get())
        {
            Client->OnDisconnected.remove(this->m_DisconnectedToken);
        }
    }

    void VirtualMachineSettingsPage::ReloadConfiguration()
    {
        auto Configuration = NanaBox::DeserializeConfiguration(
            ::ReadAllTextFromUtf8TextFile(this->m_ConfigurationFilePath));
        this->m_Configuration = Configuration;
        this->PopulateControls();
    }

    void VirtualMachineSettingsPage::UpdateWindowTitle()
    {
        LPCWSTR TitleResource = this->m_ConfigurationFilePath.empty()
            ? L"CreateTitle"
            : L"Title";
        if (!this->m_Configuration.Name.empty())
        {
            TitleResource = this->m_ConfigurationFilePath.empty()
                ? L"NamedCreateTitle"
                : L"NamedTitle";
        }
        winrt::hstring Name = winrt::to_hstring(this->m_Configuration.Name);
        ::SetWindowTextW(
            this->m_WindowHandle,
            Mile::FormatWideString(
                ::GetSettingsString(TitleResource).c_str(),
                Name.c_str()).c_str());
    }

    void VirtualMachineSettingsPage::InitializeComponent()
    {
        VirtualMachineSettingsPageT::InitializeComponent();
        if (this->m_ConfigurationFilePath.empty())
        {
            this->PopulateControls();
        }
        else
        {
            try
            {
                this->ReloadConfiguration();
            }
            catch (...)
            {
                this->PopulateControls();
                ::ShowSettingsError(this->m_WindowHandle);
            }
        }
    }

    void VirtualMachineSettingsPage::PopulateControls()
    {
        this->UpdateWindowTitle();
        this->ReloadButton().IsEnabled(!this->m_ConfigurationFilePath.empty());
        this->OpenButton().IsEnabled(!this->m_ConfigurationFilePath.empty());
        this->RuntimeNotice().Visibility(this->m_MainWindow
            ? winrt::Visibility::Visible
            : winrt::Visibility::Collapsed);

        {
            this->NameTextBox().Text(
                winrt::to_hstring(this->m_Configuration.Name));
            this->NameRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->ProcessorCountTextBox().Text(
                winrt::to_hstring(this->m_Configuration.ProcessorCount));
            this->ProcessorCountRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->MemorySizeTextBox().Text(
                winrt::to_hstring(this->m_Configuration.MemorySize));
            this->SecureBootCheckBox().IsChecked(
                this->m_Configuration.SecureBoot);
            this->SecureBootRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->TpmCheckBox().IsChecked(this->m_Configuration.Tpm);
            this->TpmRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->VirtualizationExtensionsCheckBox().IsChecked(
                this->m_Configuration.ExposeVirtualizationExtensions);
            this->VirtualizationExtensionsRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
        }

        {
            NanaBox::ComPortsConfiguration const& Configuration =
                this->m_Configuration.ComPorts;
            this->UefiConsoleComboBox().SelectedIndex(
                static_cast<std::int32_t>(Configuration.UefiConsole));
            this->UefiConsoleRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->ComPort1TextBox().Text(
                winrt::to_hstring(Configuration.ComPort1));
            this->ComPort2TextBox().Text(
                winrt::to_hstring(Configuration.ComPort2));
        }

        {
            NanaBox::GpuConfiguration const& Configuration =
                this->m_Configuration.Gpu;
            this->AssignmentModeComboBox().SelectedIndex(
                static_cast<std::int32_t>(Configuration.AssignmentMode));
            this->EnableHostDriverStoreCheckBox().IsChecked(
                Configuration.EnableHostDriverStore);
            this->EnableHostDriverStoreRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
        }

        {
            this->GpuDevicePanel().Children().Clear();
            this->m_GpuDeviceSettings.clear();
            this->AddGpuDeviceButton().IsEnabled(true);
            for (auto const& Configuration :
                this->m_Configuration.Gpu.SelectedDevices)
            {
                this->AddGpuDevice(Configuration.first, Configuration.second);
            }
        }

        {
            this->NetworkAdapterPanel().Children().Clear();
            this->m_NetworkAdapterSettings.clear();
            this->AddNetworkAdapterButton().IsEnabled(true);
            for (auto const& Configuration :
                this->m_Configuration.NetworkAdapters)
            {
                this->AddNetworkAdapter(Configuration, true);
            }
        }

        {
            this->ScsiDevicePanel().Children().Clear();
            this->m_ScsiDeviceSettings.clear();
            this->AddScsiDeviceButton().IsEnabled(true);
            for (auto const& Configuration :
                this->m_Configuration.ScsiDevices)
            {
                this->AddScsiDevice(Configuration, true);
            }
        }

        {
            this->StateFilesSection().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->GuestStateFileTextBox().Text(
                winrt::to_hstring(this->m_Configuration.GuestStateFile));
            this->RuntimeStateFileTextBox().Text(
                winrt::to_hstring(this->m_Configuration.RuntimeStateFile));
        }

        {
            NanaBox::KeyboardConfiguration const& Configuration =
                this->m_Configuration.Keyboard;
            this->RedirectKeyCombinationsCheckBox().IsChecked(
                Configuration.RedirectKeyCombinations);
            this->FullScreenHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.FullScreenHotkey));
            this->CtrlEscHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.CtrlEscHotkey));
            this->AltEscHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.AltEscHotkey));
            this->AltTabHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.AltTabHotkey));
            this->AltShiftTabHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.AltShiftTabHotkey));
            this->AltSpaceHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.AltSpaceHotkey));
            this->CtrlAltDelHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.CtrlAltDelHotkey));
            this->FocusReleaseLeftHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.FocusReleaseLeftHotkey));
            this->FocusReleaseRightHotkeyTextBox().Text(
                winrt::to_hstring(Configuration.FocusReleaseRightHotkey));
        }

        {
            NanaBox::EnhancedSessionConfiguration const& Configuration =
                this->m_Configuration.EnhancedSession;
            this->RedirectAudioCheckBox().IsChecked(
                Configuration.RedirectAudio);
            this->RedirectAudioCaptureCheckBox().IsChecked(
                Configuration.RedirectAudioCapture);
            this->RedirectDrivesCheckBox().IsChecked(
                Configuration.RedirectDrives);
            this->RedirectPrintersCheckBox().IsChecked(
                Configuration.RedirectPrinters);
            this->RedirectPortsCheckBox().IsChecked(
                Configuration.RedirectPorts);
            this->RedirectSmartCardsCheckBox().IsChecked(
                Configuration.RedirectSmartCards);
            this->RedirectClipboardCheckBox().IsChecked(
                Configuration.RedirectClipboard);
            this->RedirectDevicesCheckBox().IsChecked(
                Configuration.RedirectDevices);
            this->RedirectPOSDevicesCheckBox().IsChecked(
                Configuration.RedirectPOSDevices);
            this->RedirectDynamicDrivesCheckBox().IsChecked(
                Configuration.RedirectDynamicDrives);
            this->RedirectDynamicDevicesCheckBox().IsChecked(
                Configuration.RedirectDynamicDevices);
        }

        {
            ::PopulateRedirectionChoices(
                this->DrivePanel(),
                std::map<std::string, winrt::hstring>(),
                this->m_Configuration.EnhancedSession.Drives);
            this->RefreshDrives();
        }

        {
            ::PopulateRedirectionChoices(
                this->DevicePanel(),
                std::map<std::string, winrt::hstring>(),
                this->m_Configuration.EnhancedSession.Devices);
            this->RefreshDevices();
        }

        {
            this->ChipsetInformationSection().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            NanaBox::ChipsetInformationConfiguration const& Configuration =
                this->m_Configuration.ChipsetInformation;
            this->BaseBoardSerialNumberTextBox().Text(
                winrt::to_hstring(Configuration.BaseBoardSerialNumber));
            this->ChassisSerialNumberTextBox().Text(
                winrt::to_hstring(Configuration.ChassisSerialNumber));
            this->ChassisAssetTagTextBox().Text(
                winrt::to_hstring(Configuration.ChassisAssetTag));
            this->ManufacturerTextBox().Text(
                winrt::to_hstring(Configuration.Manufacturer));
            this->ProductNameTextBox().Text(
                winrt::to_hstring(Configuration.ProductName));
            this->ChipsetVersionTextBox().Text(
                winrt::to_hstring(Configuration.Version));
            this->SerialNumberTextBox().Text(
                winrt::to_hstring(Configuration.SerialNumber));
            this->UUIDTextBox().Text(
                winrt::to_hstring(Configuration.UUID));
            this->SKUNumberTextBox().Text(
                winrt::to_hstring(Configuration.SKUNumber));
            this->FamilyTextBox().Text(
                winrt::to_hstring(Configuration.Family));
        }

        {
            NanaBox::VideoMonitorConfiguration const& Configuration =
                this->m_Configuration.VideoMonitor;
            this->HorizontalResolutionTextBox().Text(
                winrt::to_hstring(Configuration.HorizontalResolution));
            this->HorizontalResolutionRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->VerticalResolutionTextBox().Text(
                winrt::to_hstring(Configuration.VerticalResolution));
            this->VerticalResolutionRow().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->DisableDpiScalingCheckBox().IsChecked(
                Configuration.DisableBasicSessionDpiScaling);
        }

        {
            this->PoliciesSection().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->PoliciesTextBox().Text(
                ::MakeSettingLines(this->m_Configuration.Policies));
        }

        {
            this->Plan9ShareSection().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->Plan9SharePanel().Children().Clear();
            this->m_Plan9ShareSettings.clear();
            this->AddPlan9ShareButton().IsEnabled(!this->m_MainWindow);
            if (!this->m_MainWindow)
            {
                for (auto const& Configuration :
                    this->m_Configuration.Plan9Shares)
                {
                    this->AddPlan9Share(Configuration);
                }
            }
        }

        {
            this->VirtualSmbShareSection().Visibility(this->m_MainWindow
                ? winrt::Visibility::Collapsed
                : winrt::Visibility::Visible);
            this->VirtualSmbSharePanel().Children().Clear();
            this->m_VirtualSmbShareSettings.clear();
            this->AddVirtualSmbShareButton().IsEnabled(!this->m_MainWindow);
            if (!this->m_MainWindow)
            {
                for (auto const& Configuration :
                    this->m_Configuration.VirtualSmbShares)
                {
                    this->AddVirtualSmbShare(Configuration);
                }
            }
        }

    }

    NanaBox::VirtualMachineConfiguration
        VirtualMachineSettingsPage::ReadConfiguration()
    {
        NanaBox::VirtualMachineConfiguration Result = this->m_Configuration;

        {
            if (!this->m_MainWindow)
            {
                Result.Name =
                    winrt::to_string(
                        this->NameTextBox().Text());
                Result.ProcessorCount =
                    ::ReadSettingInteger<std::uint32_t>(
                        this->ProcessorCountTextBox());
                Result.SecureBoot =
                    winrt::unbox_value<bool>(
                        this->SecureBootCheckBox().IsChecked());
                Result.Tpm =
                    winrt::unbox_value<bool>(
                        this->TpmCheckBox().IsChecked());
                Result.ExposeVirtualizationExtensions =
                    winrt::unbox_value<bool>(
                        this->VirtualizationExtensionsCheckBox().IsChecked());
            }
            Result.MemorySize =
                ::ReadSettingInteger<std::uint32_t>(
                    this->MemorySizeTextBox());

            if (!this->m_MainWindow && Result.Name.empty())
            {
                ::InvalidSetting(::GetSettingsString(L"NameLabel.Text"));
            }
            if (!this->m_MainWindow && !Result.ProcessorCount)
            {
                ::InvalidSetting(::GetSettingsString(
                    L"ProcessorCountLabel.Text"));
            }
            if (!Result.MemorySize)
            {
                ::InvalidSetting(::GetSettingsString(
                    L"MemorySizeLabel.Text"));
            }
        }

        {
            if (!this->m_MainWindow)
            {
                Result.ComPorts.UefiConsole =
                    static_cast<NanaBox::UefiConsoleMode>(
                        ::ReadSettingIndex(this->UefiConsoleComboBox()));
            }
            Result.ComPorts.ComPort1 =
                winrt::to_string(
                    this->ComPort1TextBox().Text());
            Result.ComPorts.ComPort2 =
                winrt::to_string(
                    this->ComPort2TextBox().Text());
        }

        {
            if (!this->m_MainWindow)
            {
                Result.Gpu.EnableHostDriverStore = winrt::unbox_value<bool>(
                    this->EnableHostDriverStoreCheckBox().IsChecked());
            }
            Result.Gpu.AssignmentMode = static_cast<NanaBox::GpuAssignmentMode>(
                ::ReadSettingIndex(this->AssignmentModeComboBox()));
        }

        {
            Result.Gpu.SelectedDevices.clear();
            for (GpuDeviceSettings const& Row : this->m_GpuDeviceSettings)
            {
                std::string DeviceInterface =
                    winrt::to_string(Row.DeviceInterface.Text());
                std::uint16_t PartitionId =
                    ::ReadSettingInteger<std::uint16_t>(Row.PartitionId);
                if (DeviceInterface.empty() ||
                    !Result.Gpu.SelectedDevices.emplace(
                        DeviceInterface,
                        PartitionId).second)
                {
                    ::InvalidSetting(::GetSettingsString(
                        L"GpuDeviceDeviceInterfaceLabel"));
                }
            }
        }

        {
            Result.NetworkAdapters.clear();
            std::set<std::string> EndpointIds;
            for (NetworkAdapterSettings const& Row :
                this->m_NetworkAdapterSettings)
            {
                NanaBox::NetworkAdapterConfiguration Configuration;
                Configuration.EndpointId = Row.EndpointId;
                Configuration.Connected =
                    Row.Connected.IsChecked().Value();
                Configuration.MacAddress =
                    winrt::to_string(Row.MacAddress.Text());
                Configuration.SuggestedSwitchId =
                    winrt::to_string(Row.SuggestedSwitchId.Text());
                Configuration.SuggestedSwitchName =
                    winrt::to_string(Row.SuggestedSwitchName.Text());
                Configuration.SuggestedSwitchSubnet =
                    winrt::to_string(Row.SuggestedSwitchSubnet.Text());
                if (this->m_MainWindow && Configuration.EndpointId.empty())
                {
                    GUID EndpointId;
                    winrt::check_hresult(::CoCreateGuid(&EndpointId));
                    Configuration.EndpointId = winrt::to_string(
                        ::FromGuid(EndpointId));
                }
                if (!Configuration.EndpointId.empty())
                {
                    winrt::guid EndpointId(Configuration.EndpointId);
                    std::string Key = winrt::to_string(::FromGuid(EndpointId));
                    if (!EndpointIds.insert(Key).second)
                    {
                        ::InvalidSetting(::GetSettingsString(
                            L"NetworkAdapterTitle.Text"));
                    }
                }
                Result.NetworkAdapters.push_back(Configuration);
            }
        }

        {
            Result.ScsiDevices.clear();
            for (ScsiDeviceSettings const& Row : this->m_ScsiDeviceSettings)
            {
                NanaBox::ScsiDeviceConfiguration Configuration;
                Configuration.Type = static_cast<NanaBox::ScsiDeviceType>(
                    ::ReadSettingIndex(Row.Type) + 1);
                Configuration.Path = winrt::to_string(Row.Path.Text());
                if (Configuration.Path.empty() &&
                    NanaBox::ScsiDeviceType::VirtualImage != Configuration.Type)
                {
                    if (!this->m_MainWindow)
                    {
                        continue;
                    }
                    ::InvalidSetting(::GetSettingsString(
                        L"ScsiDevicePathLabel"));
                }
                Result.ScsiDevices.push_back(Configuration);
            }
        }

        {
            if (!this->m_MainWindow)
            {
                Result.GuestStateFile =
                    winrt::to_string(
                        this->GuestStateFileTextBox().Text());
                Result.RuntimeStateFile =
                    winrt::to_string(
                        this->RuntimeStateFileTextBox().Text());
            }
        }

        {
            Result.Keyboard.RedirectKeyCombinations =
                winrt::unbox_value<bool>(
                    this->RedirectKeyCombinationsCheckBox().IsChecked());
            Result.Keyboard.FullScreenHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->FullScreenHotkeyTextBox());
            Result.Keyboard.CtrlEscHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->CtrlEscHotkeyTextBox());
            Result.Keyboard.AltEscHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->AltEscHotkeyTextBox());
            Result.Keyboard.AltTabHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->AltTabHotkeyTextBox());
            Result.Keyboard.AltShiftTabHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->AltShiftTabHotkeyTextBox());
            Result.Keyboard.AltSpaceHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->AltSpaceHotkeyTextBox());
            Result.Keyboard.CtrlAltDelHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->CtrlAltDelHotkeyTextBox());
            Result.Keyboard.FocusReleaseLeftHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->FocusReleaseLeftHotkeyTextBox());
            Result.Keyboard.FocusReleaseRightHotkey =
                ::ReadSettingInteger<std::int32_t>(
                    this->FocusReleaseRightHotkeyTextBox());
        }

        {
            Result.EnhancedSession.RedirectAudio =
                winrt::unbox_value<bool>(
                    this->RedirectAudioCheckBox().IsChecked());
            Result.EnhancedSession.RedirectAudioCapture =
                winrt::unbox_value<bool>(
                    this->RedirectAudioCaptureCheckBox().IsChecked());
            Result.EnhancedSession.RedirectDrives =
                winrt::unbox_value<bool>(
                    this->RedirectDrivesCheckBox().IsChecked());
            Result.EnhancedSession.RedirectPrinters =
                winrt::unbox_value<bool>(
                    this->RedirectPrintersCheckBox().IsChecked());
            Result.EnhancedSession.RedirectPorts =
                winrt::unbox_value<bool>(
                    this->RedirectPortsCheckBox().IsChecked());
            Result.EnhancedSession.RedirectSmartCards =
                winrt::unbox_value<bool>(
                    this->RedirectSmartCardsCheckBox().IsChecked());
            Result.EnhancedSession.RedirectClipboard =
                winrt::unbox_value<bool>(
                    this->RedirectClipboardCheckBox().IsChecked());
            Result.EnhancedSession.RedirectDevices =
                winrt::unbox_value<bool>(
                    this->RedirectDevicesCheckBox().IsChecked());
            Result.EnhancedSession.RedirectPOSDevices =
                winrt::unbox_value<bool>(
                    this->RedirectPOSDevicesCheckBox().IsChecked());
            Result.EnhancedSession.RedirectDynamicDrives =
                winrt::unbox_value<bool>(
                    this->RedirectDynamicDrivesCheckBox().IsChecked());
            Result.EnhancedSession.RedirectDynamicDevices =
                winrt::unbox_value<bool>(
                    this->RedirectDynamicDevicesCheckBox().IsChecked());
        }

        {
            Result.EnhancedSession.Drives =
                ::ReadRedirectionSelection(this->DrivePanel());
            Result.EnhancedSession.Devices =
                ::ReadRedirectionSelection(this->DevicePanel());
        }

        {
            if (!this->m_MainWindow)
            {
                Result.ChipsetInformation.BaseBoardSerialNumber =
                    winrt::to_string(
                        this->BaseBoardSerialNumberTextBox().Text());
                Result.ChipsetInformation.ChassisSerialNumber =
                    winrt::to_string(
                        this->ChassisSerialNumberTextBox().Text());
                Result.ChipsetInformation.ChassisAssetTag =
                    winrt::to_string(
                        this->ChassisAssetTagTextBox().Text());
                Result.ChipsetInformation.Manufacturer =
                    winrt::to_string(
                        this->ManufacturerTextBox().Text());
                Result.ChipsetInformation.ProductName =
                    winrt::to_string(
                        this->ProductNameTextBox().Text());
                Result.ChipsetInformation.Version =
                    winrt::to_string(
                        this->ChipsetVersionTextBox().Text());
                Result.ChipsetInformation.SerialNumber =
                    winrt::to_string(
                        this->SerialNumberTextBox().Text());
                Result.ChipsetInformation.UUID =
                    winrt::to_string(
                        this->UUIDTextBox().Text());
                Result.ChipsetInformation.SKUNumber =
                    winrt::to_string(
                        this->SKUNumberTextBox().Text());
                Result.ChipsetInformation.Family =
                    winrt::to_string(
                        this->FamilyTextBox().Text());
            }
        }

        {
            if (!this->m_MainWindow)
            {
                Result.VideoMonitor.HorizontalResolution =
                    ::ReadSettingInteger<std::uint16_t>(
                        this->HorizontalResolutionTextBox());
                Result.VideoMonitor.VerticalResolution =
                    ::ReadSettingInteger<std::uint16_t>(
                        this->VerticalResolutionTextBox());
            }
            Result.VideoMonitor.DisableBasicSessionDpiScaling =
                winrt::unbox_value<bool>(
                    this->DisableDpiScalingCheckBox().IsChecked());
        }

        {
            if (!this->m_MainWindow)
            {
                Result.Policies =
                    ::ReadSettingLines(this->PoliciesTextBox());
            }
        }

        {
            if (!this->m_MainWindow)
            {
                Result.Plan9Shares.clear();
                for (Plan9ShareSettings const& Row :
                    this->m_Plan9ShareSettings)
                {
                    NanaBox::Plan9ShareConfiguration Configuration;
                    Configuration.ReadOnly =
                        Row.ReadOnly.IsChecked().Value();
                    Configuration.Port =
                        ::ReadSettingInteger<std::uint32_t>(Row.Port);
                    Configuration.Path =
                        winrt::to_string(Row.Path.Text());
                    Configuration.Name =
                        winrt::to_string(Row.Name.Text());
                    if (Configuration.Path.empty() ||
                        Configuration.Name.empty())
                    {
                        ::InvalidSetting(::GetSettingsString(
                            L"Plan9ShareTitle.Text"));
                    }
                    if (Result.Gpu.EnableHostDriverStore &&
                        ("HostDriverStore" == Configuration.Name ||
                         "NanaBox.HostDrivers" == Configuration.Name ||
                         "NanaBox.HostLxssLib" == Configuration.Name))
                    {
                        ::InvalidSetting(::GetSettingsString(
                            L"Plan9ShareNameLabel"));
                    }
                    Result.Plan9Shares.push_back(Configuration);
                }
            }
        }

        {
            if (!this->m_MainWindow)
            {
                Result.VirtualSmbShares.clear();
                for (VirtualSmbShareSettings const& Row :
                    this->m_VirtualSmbShareSettings)
                {
                    NanaBox::VirtualSmbShareConfiguration Configuration;
                    Configuration.ReadOnly =
                        Row.ReadOnly.IsChecked().Value();
                    Configuration.Path =
                        winrt::to_string(Row.Path.Text());
                    Configuration.Name =
                        winrt::to_string(Row.Name.Text());
                    if (Configuration.Path.empty() ||
                        Configuration.Name.empty())
                    {
                        ::InvalidSetting(::GetSettingsString(
                            L"VirtualSmbShareTitle.Text"));
                    }
                    if (Result.Gpu.EnableHostDriverStore &&
                        ("NanaBox.HostDrivers" == Configuration.Name ||
                         "NanaBox.HostLxssLib" == Configuration.Name))
                    {
                        ::InvalidSetting(::GetSettingsString(
                            L"VirtualSmbShareNameLabel"));
                    }
                    Result.VirtualSmbShares.push_back(Configuration);
                }
            }
        }

        return NanaBox::DeserializeConfiguration(
            NanaBox::SerializeConfiguration(Result));
    }

    void VirtualMachineSettingsPage::ReloadButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            this->ReloadConfiguration();
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::OpenButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

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
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        ::LaunchDocumentation();
    }

    void VirtualMachineSettingsPage::ApplyButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            auto Configuration = this->ReadConfiguration();
            {
                std::wstring FilePath = this->m_ConfigurationFilePath;
                if (FilePath.empty())
                {
                    winrt::com_ptr<IFileSaveDialog> FileDialog =
                        winrt::create_instance<IFileSaveDialog>(
                            CLSID_FileSaveDialog);
                    DWORD Flags = 0;
                    winrt::check_hresult(FileDialog->GetOptions(&Flags));
                    Flags |= FOS_FORCEFILESYSTEM;
                    Flags |= FOS_NOCHANGEDIR;
                    Flags |= FOS_DONTADDTORECENT;
                    Flags |= FOS_OVERWRITEPROMPT;
                    winrt::check_hresult(FileDialog->SetOptions(Flags));

                    winrt::hstring FileTypeName =
                        ::GetSettingsString(L"ConfigurationFileTypeName");
                    COMDLG_FILTERSPEC FileType = {};
                    FileType.pszName = FileTypeName.c_str();
                    FileType.pszSpec = L"*.7b";
                    winrt::check_hresult(FileDialog->SetFileTypes(
                        1, &FileType));
                    winrt::check_hresult(FileDialog->SetDefaultExtension(
                        L"7b"));
                    winrt::check_hresult(FileDialog->SetFileName(
                        winrt::to_hstring(Configuration.Name + ".7b").c_str()));
                    HRESULT Result = FileDialog->Show(this->m_WindowHandle);
                    if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == Result)
                    {
                        return;
                    }
                    winrt::check_hresult(Result);
                    FilePath = ::GetFileSystemPathFromFileDialog(
                        FileDialog.get());
                    if (FilePath.empty())
                    {
                        return;
                    }
                    for (auto& Device : Configuration.ScsiDevices)
                    {
                        if (Device.Type ==
                            NanaBox::ScsiDeviceType::VirtualDisk ||
                            Device.Type ==
                            NanaBox::ScsiDeviceType::VirtualImage)
                        {
                            Device.Path = winrt::to_string(
                                ::GetRelativeFilePath(
                                    FilePath,
                                    winrt::to_hstring(Device.Path).c_str()));
                        }
                    }
                }

                std::string Content =
                    NanaBox::SerializeConfiguration(Configuration);
                ::WriteAllTextToUtf8TextFile(FilePath, Content);
                this->m_ConfigurationFilePath = FilePath;
            }
            this->m_Configuration = Configuration;
            this->PopulateControls();
            if (this->m_MainWindow)
            {
                if (auto Client = this->m_ApplyClient.get())
                {
                    Client->OnDisconnected.remove(this->m_DisconnectedToken);
                }
                auto Client = this->m_MainWindow->m_RdpClient;
                this->m_ApplyClient = Client->get_weak();
                auto Dispatcher = this->Dispatcher();
                auto Weak = this->get_weak();
                auto Generation = ++this->m_ApplyGeneration;
                this->m_DisconnectedToken = Client->OnDisconnected.add(
                    [Dispatcher, Weak, Generation](LONG DisconnectReason)
                {
                    UNREFERENCED_PARAMETER(DisconnectReason);
                    Dispatcher.RunAsync(
                        winrt::CoreDispatcherPriority::Normal,
                        [Weak, Generation]()
                    {
                        auto Instance = Weak.get();
                        if (Instance &&
                            Instance->m_ApplyGeneration == Generation)
                        {
                            ++Instance->m_ApplyGeneration;
                            if (auto Client = Instance->m_ApplyClient.get())
                            {
                                Client->OnDisconnected.remove(
                                    Instance->m_DisconnectedToken);
                            }
                            try
                            {
                                Instance->ReloadConfiguration();
                            }
                            catch (...)
                            {
                                ::ShowSettingsError(Instance->m_WindowHandle);
                            }
                        }
                    });
                });
                Client = nullptr;
                this->m_MainWindow->ReloadVirtualMachineSettings();
            }
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::RefreshDrives()
    {
        try
        {
            auto Selected = ::ReadRedirectionSelection(this->DrivePanel());
            if (!this->m_EnumerationClient)
            {
                this->m_EnumerationClient =
                    winrt::make_self<NanaBox::RdpClient>();
            }
            this->m_EnumerationClient->RescanDrives(false);
            std::map<std::string, winrt::hstring> Available;
            ULONG Count = this->m_EnumerationClient->DriveCount();
            for (ULONG Index = 0; Index < Count; ++Index)
            {
                auto Drive =
                    this->m_EnumerationClient->DriveByIndex(Index);
                ULONG LetterIndex = Drive.DriveLetterIndex();
                const std::string Letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                if (LetterIndex < Letters.size())
                {
                    std::string Key = Letters.substr(LetterIndex, 1);
                    Available[Key] = winrt::to_hstring(Key + ":");
                }
            }
            ::PopulateRedirectionChoices(
                this->DrivePanel(), Available, Selected);
            this->DriveRefreshError().Text(L"");
        }
        catch (...)
        {
            this->DriveRefreshError().Text(
                ::GetSettingsString(L"EnumerationFailed"));
        }
    }

    void VirtualMachineSettingsPage::RefreshDriveButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            this->RefreshDrives();
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::RefreshDevices()
    {
        try
        {
            auto Selected = ::ReadRedirectionSelection(this->DevicePanel());
            if (!this->m_EnumerationClient)
            {
                this->m_EnumerationClient =
                    winrt::make_self<NanaBox::RdpClient>();
            }
            this->m_EnumerationClient->RescanDevices(false);
            std::map<std::string, winrt::hstring> Available;
            ULONG Count = this->m_EnumerationClient->DeviceCount();
            for (ULONG Index = 0; Index < Count; ++Index)
            {
                auto Device =
                    this->m_EnumerationClient->DeviceByIndex(Index);
                std::string Key = winrt::to_string(
                    Device.DeviceInstanceId());
                winrt::hstring Label = winrt::to_hstring(Key);
                try
                {
                    winrt::hstring FriendlyName = Device.FriendlyName();
                    if (!FriendlyName.empty())
                    {
                        Label = FriendlyName;
                    }
                }
                catch (...)
                {
                }
                Available[Key] = Label;
            }
            ::PopulateRedirectionChoices(
                this->DevicePanel(), Available, Selected);
            this->DeviceRefreshError().Text(L"");
        }
        catch (...)
        {
            this->DeviceRefreshError().Text(
                ::GetSettingsString(L"EnumerationFailed"));
        }
    }

    void VirtualMachineSettingsPage::RefreshDeviceButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            this->RefreshDevices();
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::AddGpuDevice(
        std::string const& DeviceInterface,
        _In_ std::uint16_t PartitionId)
    {
        GpuDeviceSettings Row;
        Row.Container = winrt::StackPanel();
        Row.Container.Spacing(4);
        Row.Container.Margin(winrt::ThicknessHelper::FromLengths(
            8, 0, 0, 8));
        Row.DeviceInterface = ::AddSettingTextBox(
            Row.Container,
            L"GpuDeviceDeviceInterfaceLabel",
            L"GpuDeviceDeviceInterfaceHelp",
            winrt::to_hstring(DeviceInterface),
            false);
        Row.PartitionId = ::AddSettingTextBox(
            Row.Container,
            L"GpuDevicePartitionIdLabel",
            L"GpuDevicePartitionIdHelp",
            winrt::to_hstring(PartitionId),
            false);
        Row.PartitionId.Width(120);
        Row.PartitionId.HorizontalAlignment(winrt::HorizontalAlignment::Left);

        winrt::Windows::UI::Xaml::Controls::Button RemoveButton;
        RemoveButton.Content(winrt::box_value(
            ::GetSettingsString(L"Remove")));
        RemoveButton.Visibility(winrt::Visibility::Visible);
        RemoveButton.Click([
            Weak = this->get_weak(),
            WeakContainer = winrt::make_weak(Row.Container)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);

            auto Instance = Weak.get();
            if (!Instance)
            {
                return;
            }
            try
            {
                auto Container = WeakContainer.get();
                if (!Container)
                {
                    return;
                }
                std::uint32_t Index = 0;
                if (!Instance->GpuDevicePanel().Children().IndexOf(
                    Container, Index))
                {
                    return;
                }
                Instance->GpuDevicePanel().Children().RemoveAt(Index);
                auto& Settings = Instance->m_GpuDeviceSettings;
                Settings.erase(Settings.begin() + Index);
            }
            catch (...)
            {
                ::ShowSettingsError(Instance->m_WindowHandle);
            }
        });
        Row.Container.Children().Append(RemoveButton);
        this->m_GpuDeviceSettings.push_back(Row);
        try
        {
            this->GpuDevicePanel().Children().Append(Row.Container);
        }
        catch (...)
        {
            this->m_GpuDeviceSettings.pop_back();
            throw;
        }
    }

    void VirtualMachineSettingsPage::AddGpuDeviceButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            this->AddGpuDevice(std::string(), 65535);
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::AddNetworkAdapter(
        NanaBox::NetworkAdapterConfiguration const& Configuration,
        _In_ bool Existing)
    {
        NetworkAdapterSettings Row;
        Row.EndpointId = Configuration.EndpointId;
        Row.Container = winrt::StackPanel();
        Row.Container.Spacing(4);
        Row.Container.Margin(winrt::ThicknessHelper::FromLengths(
            8, 0, 0, 8));
        Row.Connected = ::AddSettingCheckBox(
            Row.Container,
            L"NetworkAdapterConnectedLabel",
            L"NetworkAdapterConnectedHelp",
            Configuration.Connected,
            false);
        Row.MacAddress = ::AddSettingTextBox(
            Row.Container,
            L"NetworkAdapterMacAddressLabel",
            L"NetworkAdapterMacAddressHelp",
            winrt::to_hstring(Configuration.MacAddress),
            false);
        Row.SuggestedSwitchId = ::AddSettingTextBox(
            Row.Container,
            L"NetworkAdapterSuggestedSwitchIdLabel",
            L"NetworkAdapterSuggestedSwitchIdHelp",
            winrt::to_hstring(Configuration.SuggestedSwitchId),
            this->m_MainWindow && Existing);
        Row.SuggestedSwitchName = ::AddSettingTextBox(
            Row.Container,
            L"NetworkAdapterSuggestedSwitchNameLabel",
            L"NetworkAdapterSuggestedSwitchNameHelp",
            winrt::to_hstring(Configuration.SuggestedSwitchName),
            this->m_MainWindow && Existing);
        Row.SuggestedSwitchSubnet = ::AddSettingTextBox(
            Row.Container,
            L"NetworkAdapterSuggestedSwitchSubnetLabel",
            L"NetworkAdapterSuggestedSwitchSubnetHelp",
            winrt::to_hstring(Configuration.SuggestedSwitchSubnet),
            this->m_MainWindow && Existing);

        winrt::Windows::UI::Xaml::Controls::Button RemoveButton;
        RemoveButton.Content(winrt::box_value(
            ::GetSettingsString(L"Remove")));
        RemoveButton.Visibility(winrt::Visibility::Visible);
        RemoveButton.Click([
            Weak = this->get_weak(),
            WeakContainer = winrt::make_weak(Row.Container)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);

            auto Instance = Weak.get();
            if (!Instance)
            {
                return;
            }
            try
            {
                auto Container = WeakContainer.get();
                if (!Container)
                {
                    return;
                }
                std::uint32_t Index = 0;
                if (!Instance->NetworkAdapterPanel().Children().IndexOf(
                    Container, Index))
                {
                    return;
                }
                Instance->NetworkAdapterPanel().Children().RemoveAt(Index);
                auto& Settings = Instance->m_NetworkAdapterSettings;
                Settings.erase(Settings.begin() + Index);
            }
            catch (...)
            {
                ::ShowSettingsError(Instance->m_WindowHandle);
            }
        });
        Row.Container.Children().Append(RemoveButton);
        this->m_NetworkAdapterSettings.push_back(Row);
        try
        {
            this->NetworkAdapterPanel().Children().Append(Row.Container);
        }
        catch (...)
        {
            this->m_NetworkAdapterSettings.pop_back();
            throw;
        }
    }

    void VirtualMachineSettingsPage::AddNetworkAdapterButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            NanaBox::NetworkAdapterConfiguration Configuration;
            Configuration.Connected = true;
            this->AddNetworkAdapter(Configuration);
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::BrowseScsiDevicePath(
        winrt::ComboBox const& Type,
        winrt::TextBox const& Path)
    {
        std::int32_t Index = Type.SelectedIndex();
        if (Index != 0 && Index != 1)
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

        COMDLG_FILTERSPEC FileType = {};
        FileType.pszName = Index == 0
            ? L"Virtual Hard Disk Files (*.vhd;*.vhdx)" : L"ISO (*.iso)";
        FileType.pszSpec = Index == 0 ? L"*.vhd;*.vhdx" : L"*.iso";
        winrt::check_hresult(FileDialog->SetFileTypes(1, &FileType));
        winrt::check_hresult(FileDialog->SetTitle(
            ::GetSettingsString(Index == 0
                ? L"VirtualDiskPathTip" : L"VirtualImagePathTip").c_str()));
        HRESULT Result = FileDialog->Show(this->m_WindowHandle);
        if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == Result)
        {
            return;
        }
        winrt::check_hresult(Result);
        std::wstring FilePath = ::GetFileSystemPathFromFileDialog(
            FileDialog.get());
        if (!FilePath.empty())
        {
            Path.Text(winrt::hstring(::GetRelativeFilePath(
                this->m_ConfigurationFilePath,
                FilePath)));
        }
    }

    void VirtualMachineSettingsPage::AddScsiDevice(
        NanaBox::ScsiDeviceConfiguration const& Configuration,
        _In_ bool Existing)
    {
        ScsiDeviceSettings Row;
        Row.Container = winrt::StackPanel();
        Row.Container.Spacing(4);
        Row.Container.Margin(winrt::ThicknessHelper::FromLengths(
            8, 0, 0, 8));
        Row.Type = winrt::ComboBox();
        Row.Type.MinWidth(120);
        Row.Type.HorizontalAlignment(winrt::HorizontalAlignment::Left);
        Row.Type.Items().Append(winrt::box_value(
            ::GetSettingsString(L"ChoiceVirtualDisk")));
        Row.Type.Items().Append(winrt::box_value(
            ::GetSettingsString(L"ChoiceVirtualImage")));
        Row.Type.Items().Append(winrt::box_value(
            ::GetSettingsString(L"ChoicePhysicalDevice")));
        Row.Type.SelectedIndex(
            static_cast<std::int32_t>(Configuration.Type) - 1);
        ::AddSettingRow(
            Row.Container,
            Row.Type,
            L"ScsiDeviceTypeLabel",
            L"ScsiDeviceTypeHelp",
            !(this->m_MainWindow && Existing));
        Row.Path = winrt::TextBox();
        Row.Path.Text(winrt::to_hstring(Configuration.Path));
        Row.Path.HorizontalAlignment(winrt::HorizontalAlignment::Stretch);
        winrt::Grid PathRow = ::AddSettingRow(
            Row.Container,
            Row.Path,
            L"ScsiDevicePathLabel",
            L"ScsiDevicePathHelp",
            true);

        winrt::Windows::UI::Xaml::Controls::Button BrowseButton;
        BrowseButton.Content(winrt::box_value(L"..."));
        BrowseButton.Margin(winrt::ThicknessHelper::FromLengths(8, 0, 0, 0));
        BrowseButton.Visibility(
            Configuration.Type == NanaBox::ScsiDeviceType::PhysicalDevice
                ? winrt::Visibility::Collapsed : winrt::Visibility::Visible);
        winrt::AutomationProperties::SetName(
            BrowseButton,
            ::GetSettingsString(L"BrowseScsiDevicePath"));
        winrt::ColumnDefinition BrowseColumn;
        BrowseColumn.Width(winrt::GridLengthHelper::Auto());
        PathRow.ColumnDefinitions().Append(BrowseColumn);
        winrt::Grid::SetColumn(BrowseButton, 2);
        PathRow.Children().Append(BrowseButton);
        Row.Type.SelectionChanged([
            WeakBrowse = winrt::make_weak(BrowseButton)](
                winrt::IInspectable const& Sender,
                winrt::SelectionChangedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Arguments);
            if (auto Button = WeakBrowse.get())
            {
                Button.Visibility(
                    Sender.as<winrt::ComboBox>().SelectedIndex() == 2
                        ? winrt::Visibility::Collapsed
                        : winrt::Visibility::Visible);
            }
        });
        BrowseButton.Click([
            Weak = this->get_weak(),
            WeakType = winrt::make_weak(Row.Type),
            WeakPath = winrt::make_weak(Row.Path)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);
            if (auto Instance = Weak.get())
            {
                try
                {
                    auto Type = WeakType.get();
                    auto Path = WeakPath.get();
                    if (Type && Path)
                    {
                        Instance->BrowseScsiDevicePath(Type, Path);
                    }
                }
                catch (...)
                {
                    ::ShowSettingsError(Instance->m_WindowHandle);
                }
            }
        });

        winrt::Windows::UI::Xaml::Controls::Button RemoveButton;
        RemoveButton.Content(winrt::box_value(
            ::GetSettingsString(L"Remove")));
        RemoveButton.Visibility(!(this->m_MainWindow && Existing)
            ? winrt::Visibility::Visible
            : winrt::Visibility::Collapsed);
        RemoveButton.Click([
            Weak = this->get_weak(),
            WeakContainer = winrt::make_weak(Row.Container)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);

            auto Instance = Weak.get();
            if (!Instance)
            {
                return;
            }
            try
            {
                auto Container = WeakContainer.get();
                if (!Container)
                {
                    return;
                }
                std::uint32_t Index = 0;
                if (!Instance->ScsiDevicePanel().Children().IndexOf(
                    Container, Index))
                {
                    return;
                }
                Instance->ScsiDevicePanel().Children().RemoveAt(Index);
                auto& Settings = Instance->m_ScsiDeviceSettings;
                Settings.erase(Settings.begin() + Index);
            }
            catch (...)
            {
                ::ShowSettingsError(Instance->m_WindowHandle);
            }
        });
        Row.Container.Children().Append(RemoveButton);
        this->m_ScsiDeviceSettings.push_back(Row);
        try
        {
            this->ScsiDevicePanel().Children().Append(Row.Container);
        }
        catch (...)
        {
            this->m_ScsiDeviceSettings.pop_back();
            throw;
        }
    }

    void VirtualMachineSettingsPage::AddScsiDeviceButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            NanaBox::ScsiDeviceConfiguration Configuration;
            Configuration.Type = NanaBox::ScsiDeviceType::VirtualDisk;
            this->AddScsiDevice(Configuration);
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::AddPlan9Share(
        NanaBox::Plan9ShareConfiguration const& Configuration)
    {
        Plan9ShareSettings Row;
        Row.Container = winrt::StackPanel();
        Row.Container.Spacing(4);
        Row.Container.Margin(winrt::ThicknessHelper::FromLengths(
            8, 0, 0, 8));
        Row.ReadOnly = ::AddSettingCheckBox(
            Row.Container,
            L"Plan9ShareReadOnlyLabel",
            L"Plan9ShareReadOnlyHelp",
            Configuration.ReadOnly,
            false);
        Row.Port = ::AddSettingTextBox(
            Row.Container,
            L"Plan9SharePortLabel",
            L"Plan9SharePortHelp",
            winrt::to_hstring(Configuration.Port),
            false);
        Row.Port.Width(120);
        Row.Port.HorizontalAlignment(winrt::HorizontalAlignment::Left);
        Row.Path = ::AddSettingTextBox(
            Row.Container,
            L"Plan9SharePathLabel",
            L"Plan9SharePathHelp",
            winrt::to_hstring(Configuration.Path),
            false);
        Row.Name = ::AddSettingTextBox(
            Row.Container,
            L"Plan9ShareNameLabel",
            L"Plan9ShareNameHelp",
            winrt::to_hstring(Configuration.Name),
            false);

        winrt::Windows::UI::Xaml::Controls::Button RemoveButton;
        RemoveButton.Content(winrt::box_value(
            ::GetSettingsString(L"Remove")));
        RemoveButton.Click([
            Weak = this->get_weak(),
            WeakContainer = winrt::make_weak(Row.Container)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);

            auto Instance = Weak.get();
            if (!Instance)
            {
                return;
            }
            try
            {
                auto Container = WeakContainer.get();
                if (!Container)
                {
                    return;
                }
                std::uint32_t Index = 0;
                if (!Instance->Plan9SharePanel().Children().IndexOf(
                    Container, Index))
                {
                    return;
                }
                Instance->Plan9SharePanel().Children().RemoveAt(Index);
                auto& Settings = Instance->m_Plan9ShareSettings;
                Settings.erase(Settings.begin() + Index);
            }
            catch (...)
            {
                ::ShowSettingsError(Instance->m_WindowHandle);
            }
        });
        Row.Container.Children().Append(RemoveButton);
        this->m_Plan9ShareSettings.push_back(Row);
        try
        {
            this->Plan9SharePanel().Children().Append(Row.Container);
        }
        catch (...)
        {
            this->m_Plan9ShareSettings.pop_back();
            throw;
        }
    }

    void VirtualMachineSettingsPage::AddPlan9ShareButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            NanaBox::Plan9ShareConfiguration Configuration;
            Configuration.Port = 50001;
            this->AddPlan9Share(Configuration);
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }

    void VirtualMachineSettingsPage::AddVirtualSmbShare(
        NanaBox::VirtualSmbShareConfiguration const& Configuration)
    {
        VirtualSmbShareSettings Row;
        Row.Container = winrt::StackPanel();
        Row.Container.Spacing(4);
        Row.Container.Margin(winrt::ThicknessHelper::FromLengths(
            8, 0, 0, 8));
        Row.ReadOnly = ::AddSettingCheckBox(
            Row.Container,
            L"VirtualSmbShareReadOnlyLabel",
            L"VirtualSmbShareReadOnlyHelp",
            Configuration.ReadOnly,
            false);
        Row.Path = ::AddSettingTextBox(
            Row.Container,
            L"VirtualSmbSharePathLabel",
            L"VirtualSmbSharePathHelp",
            winrt::to_hstring(Configuration.Path),
            false);
        Row.Name = ::AddSettingTextBox(
            Row.Container,
            L"VirtualSmbShareNameLabel",
            L"VirtualSmbShareNameHelp",
            winrt::to_hstring(Configuration.Name),
            false);

        winrt::Windows::UI::Xaml::Controls::Button RemoveButton;
        RemoveButton.Content(winrt::box_value(
            ::GetSettingsString(L"Remove")));
        RemoveButton.Click([
            Weak = this->get_weak(),
            WeakContainer = winrt::make_weak(Row.Container)](
                winrt::IInspectable const& Sender,
                winrt::RoutedEventArgs const& Arguments)
        {
            UNREFERENCED_PARAMETER(Sender);
            UNREFERENCED_PARAMETER(Arguments);

            auto Instance = Weak.get();
            if (!Instance)
            {
                return;
            }
            try
            {
                auto Container = WeakContainer.get();
                if (!Container)
                {
                    return;
                }
                std::uint32_t Index = 0;
                if (!Instance->VirtualSmbSharePanel().Children().IndexOf(
                    Container, Index))
                {
                    return;
                }
                Instance->VirtualSmbSharePanel().Children().RemoveAt(Index);
                auto& Settings = Instance->m_VirtualSmbShareSettings;
                Settings.erase(Settings.begin() + Index);
            }
            catch (...)
            {
                ::ShowSettingsError(Instance->m_WindowHandle);
            }
        });
        Row.Container.Children().Append(RemoveButton);
        this->m_VirtualSmbShareSettings.push_back(Row);
        try
        {
            this->VirtualSmbSharePanel().Children().Append(Row.Container);
        }
        catch (...)
        {
            this->m_VirtualSmbShareSettings.pop_back();
            throw;
        }
    }

    void VirtualMachineSettingsPage::AddVirtualSmbShareButtonClick(
        winrt::IInspectable const& Sender,
        winrt::RoutedEventArgs const& Arguments)
    {
        UNREFERENCED_PARAMETER(Sender);
        UNREFERENCED_PARAMETER(Arguments);

        try
        {
            NanaBox::VirtualSmbShareConfiguration Configuration;
            this->AddVirtualSmbShare(Configuration);
        }
        catch (...)
        {
            ::ShowSettingsError(this->m_WindowHandle);
        }
    }
}
