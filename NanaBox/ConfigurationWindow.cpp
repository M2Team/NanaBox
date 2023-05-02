#include "ConfigurationWindow.h"

#include "ConfigurationWindowControl.h"
#include "ConfigurationViewModel.h"

#include "Utils.h"

BOOL NanaBox::ConfigurationWindow::PreTranslateMessage(
    MSG* pMsg)
{
    // Workaround for capturing Alt+F4 in applications with XAML Islands.
    // Reference: https://github.com/microsoft/microsoft-ui-xaml/issues/2408
    if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
    {
        ::SendMessageW(
            ::GetAncestor(pMsg->hwnd, GA_ROOT),
            pMsg->message,
            pMsg->wParam,
            pMsg->lParam);

        return TRUE;
    }

    return FALSE;
}


int NanaBox::ConfigurationWindow::OnCreate(
    LPCREATESTRUCT lpCreateStruct)
{
    UNREFERENCED_PARAMETER(lpCreateStruct);

    if (this->DefWindowProcW() != 0)
    {
        return -1;
    }

    return 0;
}

void NanaBox::ConfigurationWindow::OnDestroy()
{
    ::PostQuitMessage(0);
}

bool NanaBox::ConfigurationWindow::Initialize(std::filesystem::path const& path)
{
    this->m_ConfigurationControl =
        winrt::make<winrt::NanaBox::implementation::ConfigurationWindowControl>();

    m_ConfigurationPath = path;
    std::string ConfigurationFileContent = ::ReadAllTextFromUtf8TextFile(path);

    this->m_Configuration = NanaBox::DeserializeConfiguration(
        ConfigurationFileContent);
    auto viewModel = winrt::make<winrt::NanaBox::implementation::ConfigurationViewModel>(
        &this->m_Configuration);
    this->m_ConfigurationControl.ViewModel(viewModel);

    if (!this->Create(
        nullptr,
        this->rcDefault,
        L"[Configuration]",
        WS_OVERLAPPEDWINDOW,
        0,
        nullptr,
        winrt::get_abi(this->m_ConfigurationControl)))
    {
        return false;
    }

    return true;
}
