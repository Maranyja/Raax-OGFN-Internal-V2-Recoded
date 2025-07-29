#include "drawtransition.h"

#include <cheat/sdk/sdk.h>
#include <cheat/hooks.h>
#include <cheat/tick/playertick.h>
#include <cheat/core.h>

namespace DrawTransition {

// --- Variables -----------------------------------------------------

void* DrawTransitionAddress = nullptr;

// --- Hook ----------------------------------------------------------

typedef void(__stdcall* t_DrawTransition)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);
inline t_DrawTransition o_DrawTransition = nullptr;
static void             h_DrawTransition(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas) {
    std::lock_guard<std::mutex> lock(Core::g_GameRenderThreadLock);

    Core::TickGameThread();

    if (o_DrawTransition)
        o_DrawTransition(_this, Canvas);
}

// --- Initialization ------------------------------------------------

bool Init() {
    SDK::UEngine* Engine = SDK::GetEngine();
    if (!Engine)
        return false;

    SDK::UGameViewportClient* GameViewport = Engine->GameViewport;
    if (!GameViewport || !GameViewport->VTable)
        return false;

    DrawTransitionAddress = GameViewport->VTable[SDK::UGameViewportClient::DrawTransition_Idx];
    return Hooks::CreateHook(DrawTransitionAddress, &h_DrawTransition, reinterpret_cast<void**>(&o_DrawTransition)) &&
           Hooks::EnableHook(DrawTransitionAddress);
}

void Destroy() {
    Hooks::RemoveHook(DrawTransitionAddress);
}

} // namespace DrawTransition
