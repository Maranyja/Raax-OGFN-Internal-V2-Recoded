#include "aimbot.h"

#include <unordered_map>
#include <vector>
#include <random>

#include <cheat/features/weaponutils.h>
#include <cheat/input.h>
#include <cheat/hooks.h>
#include <extern/imgui/imgui.h>
#include <cheat/sdk/sdk.h>
#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <config/config.h>
#include <drawing/drawing.h>
#include <utils/math.h>
#include <utils/error.h>

namespace Features {
namespace Aimbot {

// --- Target & Global Data ------------------------------------------

struct TargetInfo {
    SDK::AFortPawn* Pawn = nullptr;
    SDK::FRotator   Rotation;
    SDK::FRotator   RotationDelta;
    SDK::FVector2D  ScreenPos;
    SDK::FVector    WorldPos;
    float           DistDeg = 0.f;
    float           DistanceM = 0.f;
    float           DistCombined = 0.f;
    bool            InDeadZone = false;
    bool            Visible = false;

    void Reset() { Pawn = nullptr; }
} static Target;

static float WorldGravityZ = 0.f;

// --- Hook Functions ------------------------------------------------

static SDK::FTransform* (*o_GetTargetingTransform)(SDK::FTransform* result, void* TargetingLocationInfo,
                                                   uint8_t Source) = nullptr;
static SDK::FTransform* h_GetTargetingTransform(SDK::FTransform* result, void* TargetingLocationInfo, uint8_t Source) {
    SDK::FTransform* Return = o_GetTargetingTransform(result, TargetingLocationInfo, Source);
    if (!Target.Pawn || !TargetingLocationInfo)
        return Return;

    if (Config::g_Config.Aimbot.BulletTP) {
        Return->Translation = Target.WorldPos;
        Return->Translation.Z += 50.f; // Makes sure always hits target bone
    } else if (Config::g_Config.Aimbot.SilentAim) {
        // doesnt work, no idea why, probably some 3rd person camera lag bullshit since it works with first person guns.
        // please kill me im not adding 2 more hooks for getplayerviewpoint and getviewpoint when logically you should
        // be able to do it right here
        Return->Rotation = Math::RotatorToQuat(Target.Rotation);
    }

    return Return;
}

static void ApplyHooks() {
    // Cannot apply hooks since we couldn't find the function address
    if (!SDK::AFortPawn::pGetTargetingTransform)
        return;

    static bool AppliedHooks = false;
    if (!AppliedHooks && (Config::g_Config.Aimbot.BulletTP || Config::g_Config.Aimbot.SilentAim)) {
        AppliedHooks = true;
        if (!Hooks::CreateHook(SDK::AFortPawn::pGetTargetingTransform, &h_GetTargetingTransform,
                               reinterpret_cast<void**>(&o_GetTargetingTransform))) {
            Error::ThrowError("Failed to create GetTargetingTransform hook!");
        }

        if (!Hooks::EnableHook(SDK::AFortPawn::pGetTargetingTransform)) {
            Error::ThrowError("Failed to enable GetTargetingTransform hook!");
        }
    }
}

// --- Global Data Functions -----------------------------------------

static void UpdateWorldGravityZ() {
    SDK::UWorld* World = SDK::GetWorld();
    if (!World)
        return;

    SDK::ULevel* PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel)
        return;

    SDK::AWorldSettings* WorldSettings = PersistentLevel->WorldSettings;
    if (!WorldSettings)
        return;

    WorldGravityZ = WorldSettings->WorldGravityZ;
}

// --- Target Functions ----------------------------------------------

static bool EvaluateTarget(const Cache::Player::PlayerInfo& Info) {
    auto& s = WeaponUtils::GetState();
    if (Info.DistanceM > s.Config.MaxDistance)
        return false;

    if (s.Config.VisibleCheck && !Config::g_Config.Aimbot.BulletTP && !Info.HeadVisible)
        return false;

    int BoneIdx = static_cast<int>(s.CurrentBone);

    SDK::FVector   TargetWorldPos = Info.BoneWorldPos[BoneIdx];
    SDK::FVector2D TargetScreenPos = Info.BoneScreenPos[BoneIdx];
    float          DistanceM = Info.DistanceM;

    if (s.UseProjectile && s.ProjectileTemp && Config::g_Config.Aimbot.BulletPrediction) {
        float GravityZ = WorldGravityZ * s.GravityScales[s.ProjectileTemp->Class];
        TargetWorldPos = WeaponUtils::PredictProjectile(Core::g_CameraLocation, TargetWorldPos,
                                                        Info.Pawn->RootComponent->ComponentVelocity,
                                                        s.ProjectileTemp->GetDefaultSpeed(1.f), GravityZ);
    }

    SDK::FRotator AimRot = Math::FindLookAtRotation(Core::g_CameraLocation, TargetWorldPos);
    float         DistDeg = Math::GetDegreeDistance(Core::g_CameraRotation, AimRot);
    float         DistPix = TargetScreenPos.Dist({(float)Core::g_ScreenCenterX, (float)Core::g_ScreenCenterY});
    float         DistCombined = DistDeg + DistanceM * 0.25f;

    bool InDeadZone = false;
    if (Math::IsOnScreen(TargetScreenPos)) {
        if (DistPix > s.Config.FOV * Core::g_PixelsPerDegree)
            return false;
        InDeadZone = DistPix <= s.Config.DeadzoneFOV * Core::g_PixelsPerDegree;
    } else {
        if (DistDeg > s.Config.FOV)
            return false;
        InDeadZone = DistDeg <= s.Config.DeadzoneFOV;
    }

    if (Target.Pawn && Info.Pawn != Target.Pawn) {
        if ((s.Config.Selection == Config::ConfigData::TargetSelection::Distance && DistanceM > Target.DistanceM) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Degrees && DistDeg > Target.DistDeg) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Combined && DistCombined > Target.DistCombined))
            return false;
    }

    Target.Pawn = Info.Pawn;
    Target.Rotation = AimRot;
    Target.ScreenPos = TargetScreenPos;
    Target.WorldPos = TargetWorldPos;
    Target.DistDeg = DistDeg;
    Target.DistanceM = DistanceM;
    Target.DistCombined = DistCombined;
    Target.InDeadZone = InDeadZone;
    Target.Visible = Info.HeadVisible;

    SDK::FRotator RotationDelta(AimRot.Pitch - Core::g_CameraRotation.Pitch, AimRot.Yaw - Core::g_CameraRotation.Yaw,
                                0.f);
    RotationDelta.Normalize();
    RotationDelta = RotationDelta / s.EffectiveSmooth;
    RotationDelta.Normalize();
    Target.RotationDelta = RotationDelta;
    return true;
}

static void AcquireTarget() {
    auto& s = WeaponUtils::GetState();

    bool Found = false;
    if (Target.Pawn) {
        for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
            if (Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance || Info.IsDBNO || Info.IsDead)
                continue;

            if (Info.Pawn == Target.Pawn) {
                Found = EvaluateTarget(Info);
                break;
            }
        }
    }
    if (!Found)
        Target.Reset();
    else if (s.IsTargeting && s.Config.StickyTarget)
        return;

    for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance ||
            Info.TeamIndex == Core::g_LocalTeamIndex || Info.IsDBNO || Info.IsDead)
            continue;
        EvaluateTarget(Info);
    }
}

// --- Aimbot Functions ----------------------------------------------

static void ApplyAimbot() {
    auto& s = WeaponUtils::GetState();
    if (!s.Config.Enabled || s.CurrentAmmo == WeaponUtils::AmmoType::Unknown || !Target.Pawn || !s.IsTargeting)
        return;

    if (s.Config.UseDeadzone && Target.InDeadZone)
        return;

    SDK::APlayerController* Controller = SDK::GetLocalController();
    if (Controller) {
        Controller->AddPitchInput(Target.RotationDelta.Pitch / Controller->InputPitchScale);
        Controller->AddYawInput(Target.RotationDelta.Yaw / Controller->InputYawScale);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    ApplyHooks();
    AcquireTarget();
    ApplyAimbot();
}

void TickRenderThread() {
    auto& s = WeaponUtils::GetState();
    if (!s.Config.Enabled || s.CurrentAmmo == WeaponUtils::AmmoType::Unknown)
        return;

    s.IsTargeting = Input::IsKeyDown(Config::g_Config.Aimbot.AimbotKeybind);
    SDK::FVector2D Center =
        SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX), static_cast<float>(Core::g_ScreenCenterY));
    SDK::FLinearColor TargetColor =
        Target.Visible ? Config::g_Config.Color.PrimaryColorVisible : Config::g_Config.Color.PrimaryColorHidden;

    if (s.Config.ShowFOV)
        Drawing::Circle(Center, s.Config.FOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::White);

    if (s.Config.UseDeadzone && s.Config.ShowDeadzoneFOV)
        Drawing::Circle(Center, s.Config.DeadzoneFOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::Red);

    if (Config::g_Config.Aimbot.ShowTargetLine && Target.Pawn)
        Drawing::Line(Center, Target.ScreenPos, TargetColor);
}

} // namespace Aimbot
} // namespace Features
