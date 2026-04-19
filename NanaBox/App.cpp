#include "pch.h"

#include "App.h"

#include <Mile.Xaml.h>

namespace winrt::NanaBox::implementation
{
    App::App()
    {
        // Workaround for unhandled exception at twinapi.appcore.dll
        // Fixes: https://github.com/M2Team/NanaZip/issues/400
        this->AddRef();
        winrt::check_hresult(::MileXamlGlobalInitialize());
    }

    void App::Close()
    {
        this->Exit();
        ::MileXamlGlobalUninitialize();
    }
}
