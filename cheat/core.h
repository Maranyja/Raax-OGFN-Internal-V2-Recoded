#pragma once
#include <mutex>

#include <cheat/sdk/Engine.h>

namespace Core {

// TODO - Make it so we don't need to expose this
SDK::UWorld* GetNewWorld();

// --- Initialization ------------------------------------------------

bool Init(void* hModule);
void Destroy();

// --- Public Tick Functions -----------------------------------------

void TickGameThread();
void TickRenderThread();

// --- Global Variables ----------------------------------------------

extern SDK::UWorld*               g_World;
extern SDK::ULocalPlayer*         g_LocalPlayer;
extern SDK::APlayerController*    g_LocalPlayerController;
extern SDK::APlayerCameraManager* g_LocalPlayerCameraManager;
extern SDK::APawn*                g_LocalPawn;
extern uint8_t                    g_LocalTeamIndex;

extern int32_t g_ScreenSizeX;
extern int32_t g_ScreenSizeY;
extern int32_t g_ScreenCenterX;
extern int32_t g_ScreenCenterY;
extern float   g_PixelsPerDegree;

extern SDK::FVector  g_CameraLocation;
extern SDK::FRotator g_CameraRotation;
extern float         g_FOV;

extern SDK::FVector g_LocalPlayerPos;

extern std::mutex g_GameRenderThreadLock;

} // namespace Core
