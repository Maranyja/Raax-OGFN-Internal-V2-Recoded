#include "triggerbot.h"

#include <chrono>

#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <cheat/features/weaponutils.h>
#include <config/config.h>
#include <drawing/drawing.h>
#include <utils/log.h>
#include <utils/math.h>

namespace Features {
namespace TriggerBot {

// --- Trigger Bot Data ----------------------------------------------

static bool PressingKey = false;

// --- Trigger Bot Utility Functions ---------------------------------

static bool ShouldRun() {
    auto&       s = WeaponUtils::GetState();
    const auto& Config = Config::g_Config.TriggerBot;
    return Config.Enabled && s.CurrentAmmo != WeaponUtils::AmmoType::Unknown && s.Config.UseInTriggerBot;
}

static bool IsCandidate(const Cache::Player::PlayerInfo& Info) {
    const auto& Config = Config::g_Config.TriggerBot;
    if (!Info.HeadVisible || Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM > Config.MaxDistance ||
        Info.TeamIndex == Core::g_LocalTeamIndex) {
        return false;
    }

    int BoneIdx = (int)Cache::Player::BoneIdx::Head;

    auto TargetWorldPos = Info.BoneWorldPos[BoneIdx];
    auto TargetScreenPos = Info.BoneScreenPos[BoneIdx];

    auto  AimRot = Math::FindLookAtRotation(Core::g_CameraLocation, TargetWorldPos);
    float DistDeg = Math::GetDegreeDistance(Core::g_CameraRotation, AimRot);
    float DistPix = TargetScreenPos.Dist({(float)Core::g_ScreenCenterX, (float)Core::g_ScreenCenterY});

    if (Math::IsOnScreen(TargetScreenPos) && (DistPix > Config.FOV * Core::g_PixelsPerDegree)) {
        return false;
    } else if (DistDeg > Config.FOV) {
        return false;
    }

    return true;
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    if (!ShouldRun()) {
        return;
    }

    bool FoundCandidate = false;
    for (const auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (IsCandidate(Info)) {
            FoundCandidate = true;
            break;
        }
    }

    static std::chrono::time_point<std::chrono::high_resolution_clock> LastVisibleTime;
    auto                                                               Now = std::chrono::high_resolution_clock::now();
    float Elapsed = std::chrono::duration<float>(Now - LastVisibleTime).count();

    const auto& Config = Config::g_Config.TriggerBot;
    if (FoundCandidate && (PressingKey || !Config.UseKeybind)) {
        if (Elapsed >= Config.FireDelayS) {
            auto* Controller = SDK::Cast<SDK::AFortPlayerController>(SDK::GetLocalController());
            if (Controller) {
                Controller->Fire_Press();
                Controller->Fire_Release();
            }
        }
    } else {
        LastVisibleTime = Now;
    }
}

void TickRenderThread() {
    if (!ShouldRun()) {
        return;
    }

    const auto& Config = Config::g_Config.TriggerBot;
    PressingKey = Input::IsKeyDown(Config.Keybind);

    SDK::FVector2D Center =
        SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX), static_cast<float>(Core::g_ScreenCenterY));
    Drawing::Circle(Center, Config.FOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::Green);
}

} // namespace TriggerBot
} // namespace Features
