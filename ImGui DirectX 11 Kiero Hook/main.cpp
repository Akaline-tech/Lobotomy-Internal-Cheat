#include "includes.h"
#include "game.h"
#include "Gui.h"
#include <Windows.h>

bool g_showMenu = false;

extern HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern Present oPresent;

extern void CleanupRenderTarget();
extern HWND window;
extern WNDPROC oWndProc;

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    bool init_hook = false;
    do {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }
        Sleep(100);
    } while (!init_hook);
    return TRUE;
}

DWORD WINAPI InitThread(LPVOID lpReserved)
{
    GameBackend::Start();
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        GameBackend::Stop();
        CleanupRenderTarget();
        if (oWndProc && window)
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        kiero::shutdown();
        break;
    }
    return TRUE;
}