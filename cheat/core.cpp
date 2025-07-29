#include "core.h"

#include <algorithm>
#include <filesystem>

#include <cheat/autorevertfeature.h>
#include <cheat/sdk/sdk.h>
#include <cheat/hooks.h>
#include <cheat/drawtransition.h>
#include <cheat/tick/playertick.h>
#include <cheat/tick/containertick.h>
#include <cheat/tick/pickuptick.h>
#include <cheat/features/weaponutils.h>
#include <cheat/features/radar.h>
#include <cheat/features/aimbot.h>
#include <cheat/features/triggerbot.h>
#include <cheat/features/exploits.h>
#include <cheat/features/chams.h>
#include <drawing/drawing.h>
#include <config/keybind.h>
#include <gui/mainwindow.h>
#include <gui/gui.h>
#include <utils/log.h>
#include <utils/math.h>
#include <globals.h>

namespace Core {

// --- Utility Functions ---------------------------------------------

SDK::UWorld* GetNewWorld() {
    SDK::UEngine* Engine = SDK::GetEngine();
    if (!Engine)
        return nullptr;

    SDK::UGameViewportClient* GameViewport = Engine->GameViewport;
    if (!GameViewport)
        return nullptr;

    return GameViewport->World;
}

static SDK::ULocalPlayer* GetNewLocalPlayer() {
    SDK::UEngine* Engine = SDK::GetEngine();
    if (!Engine)
        return nullptr;

    SDK::UGameViewportClient* GameViewport = Engine->GameViewport;
    if (!GameViewport)
        return nullptr;

    SDK::UGameInstance* GameInstance = GameViewport->GameInstance;
    if (!GameInstance)
        return nullptr;

    SDK::TArray<SDK::ULocalPlayer*>& LocalPlayers = GameInstance->LocalPlayers;
    if (!LocalPlayers.IsValid())
        return nullptr;

    return LocalPlayers.GetByIndex(0);
}

static SDK::APlayerController* GetNewPlayerController() {
    if (!g_LocalPlayer)
        return nullptr;

    return g_LocalPlayer->PlayerController;
}

static SDK::APlayerCameraManager* GetNewPlayerCameraManager() {
    if (!g_LocalPlayerController)
        return nullptr;

    return g_LocalPlayerController->PlayerCameraManager;
}

static SDK::APawn* GetNewPawn() {
    if (!g_LocalPlayerController)
        return nullptr;

    return g_LocalPlayerController->AcknowledgedPawn;
}

static float GetNewCameraFOV() {
    if (!g_LocalPlayerCameraManager)
        return 0.f;

    return g_LocalPlayerCameraManager->GetFOVAngle();
}
static SDK::FVector GetNewCameraLocation() {
    if (!g_LocalPlayerCameraManager)
        return SDK::FVector();

    return g_LocalPlayerCameraManager->GetCameraLocation();
}
static SDK::FRotator GetNewCameraRotation() {
    if (!g_LocalPlayerCameraManager)
        return SDK::FRotator();

    return g_LocalPlayerCameraManager->GetCameraRotation();
}

static void UpdateGlobalVariables() {
    g_World = GetNewWorld();
    g_LocalPlayer = GetNewLocalPlayer();
    g_LocalPlayerController = GetNewPlayerController();
    g_LocalPlayerCameraManager = GetNewPlayerCameraManager();
    g_LocalPawn = GetNewPawn();

    SDK::UCanvas* Canvas = SDK::GetCanvas();
    if (Canvas) {
        g_ScreenSizeX = SDK::GetCanvas()->SizeX;
        g_ScreenSizeY = SDK::GetCanvas()->SizeY;
        g_ScreenCenterX = g_ScreenSizeX / 2;
        g_ScreenCenterY = g_ScreenSizeY / 2;
    }

    g_CameraLocation = GetNewCameraLocation();
    g_CameraRotation = GetNewCameraRotation();
    g_FOV = GetNewCameraFOV();

    g_PixelsPerDegree = g_ScreenSizeX / Math::RadiansToDegrees(
                                            (2.f * tan(0.5f * Math::DegreesToRadians(std::clamp(g_FOV, 0.f, 120.f)))));

    g_LocalTeamIndex = -1;
    if (g_LocalPawn) {
        SDK::USceneComponent* RootComponent = g_LocalPawn->RootComponent;
        if (RootComponent) {
            g_LocalPlayerPos = RootComponent->RelativeLocation;
        }

        SDK::AFortPlayerStateAthena* PlayerState = SDK::Cast<SDK::AFortPlayerStateAthena>(g_LocalPawn->PlayerState);
        if (PlayerState) {
            g_LocalTeamIndex = PlayerState->TeamIndex;
        }
    }
}

// --- Initialization ------------------------------------------------

bool Init(void* hModule) {
#if CFG_USELOGGING
    Log::InitLogger();
#endif
#if CFG_SHOWCONSOLE
    Log::CreateConsole();
#endif
    LOG(LOG_TRACE, "Setting up core...");
    LOG(LOG_INFO, "Image Base: %llX", Memory::GetImageBase());
    LOG(LOG_INFO, "Module Base: %p", hModule);
    return Hooks::Init() && SDK::Init() && GUI::Init() && DrawTransition::Init();
}

void Destroy() {
    GUI::Destroy();
    DrawTransition::Destroy();
    Hooks::Destroy();
    AutoRevertFeature::Destroy();
    Features::Chams::Destroy();

#if CFG_SHOWCONSOLE
    Log::DestroyConsole();
#endif
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    UpdateGlobalVariables();

    AutoRevertFeature::Tick();

    Tick::Container::TickGameThread();
    Tick::Pickup::TickGameThread();
    Tick::Player::TickGameThread();

    Features::WeaponUtils::TickGameThread();
    Features::Aimbot::TickGameThread();
    Features::TriggerBot::TickGameThread();
    Features::Chams::TickGameThread();
    Features::Exploits::TickGameThread();

    // For engine builds, render and game thread is merged since rendering happens at DrawTransition ticks, which is
    // when TickGameThread is triggered
#ifdef _ENGINE
    TickRenderThread();
#endif
}

void TickRenderThread() {
    Drawing::Tick();

    Tick::Container::TickRenderThread();
    Tick::Pickup::TickRenderThread();
    Tick::Player::TickRenderThread();

    Features::Radar::TickRenderThread();
    Features::Aimbot::TickRenderThread();
    Features::TriggerBot::TickRenderThread();

    Keybind::Tick();

    GUI::MainWindow::Tick();
}

// --- Global Variables ----------------------------------------------

SDK::UWorld*               g_World = nullptr;
SDK::ULocalPlayer*         g_LocalPlayer = nullptr;
SDK::APlayerController*    g_LocalPlayerController = nullptr;
SDK::APlayerCameraManager* g_LocalPlayerCameraManager = nullptr;
SDK::APawn*                g_LocalPawn = nullptr;
uint8_t                    g_LocalTeamIndex = 0;

int32_t g_ScreenSizeX = 0;
int32_t g_ScreenSizeY = 0;
int32_t g_ScreenCenterX = 0;
int32_t g_ScreenCenterY = 0;
float   g_PixelsPerDegree = 0.f;

SDK::FVector  g_CameraLocation = SDK::FVector();
SDK::FRotator g_CameraRotation = SDK::FRotator();
float         g_FOV = 0.f;

SDK::FVector g_LocalPlayerPos = SDK::FVector();

std::mutex g_GameRenderThreadLock;

} // namespace Core
