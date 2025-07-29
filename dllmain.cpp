#include <Windows.h>

#include <globals.h>
#include <cheat/core.h>
#include <utils/log.h>

static DWORD Main(LPVOID hModule) {
#if CFG_MAINTHREAD
    bool Result = Core::Init(hModule);
    LOG(LOG_INFO, "Core::Init() -> %d", Result);
#endif

#if CFG_UNLOADTHREAD
    while (true) {
        if (GetAsyncKeyState(VK_F6) & 1) {
            LOG(LOG_INFO, "Unloading...");
            Core::Destroy();
            break;
        }

        Sleep(100);
    }

    // TODO: Wait until game thread and render thread are finished instead of arbitrary sleep
    Sleep(1000);
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
#endif
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
#if CFG_MAINTHREAD || CFG_UNLOADTHREAD
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, Main, hModule, 0, nullptr);
#else
        Main(hModule);
#endif
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
