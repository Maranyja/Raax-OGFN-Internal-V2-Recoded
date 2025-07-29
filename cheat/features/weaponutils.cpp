#include "weaponutils.h"

#include <chrono>

#include <extern/imgui/imgui.h>
#include <cheat/core.h>
#include <config/config.h>
#include <drawing/drawing.h>
#include <utils/error.h>

namespace Features {
namespace WeaponUtils {

// --- State ---------------------------------------------------------

static State s;

State& GetState() {
    return s;
}

// --- Utility Functions ---------------------------------------------

SDK::FVector PredictProjectile(const SDK::FVector& Origin, const SDK::FVector& Target, const SDK::FVector& TargetVel,
                               float BulletSpeed, float GravityZ) {
    SDK::FVector Predicted = Target;
    for (int i = 0; i < 10; ++i) {
        SDK::FVector Delta = Predicted - Origin;
        float        t = Delta.Size() / std::fabs(BulletSpeed);
        Predicted = Target + TargetVel * t;
        Predicted.Z -= 0.5f * GravityZ * t * t;
    }
    return Predicted;
}

// --- Per-Frame Updates ---------------------------------------------

static void SetCurrentBone() {
    if (s.Config.Bone == Config::ConfigData::TargetBone::Random) {
        static std::mt19937                                                RNG{std::random_device{}()};
        static std::chrono::time_point<std::chrono::high_resolution_clock> LastBoneRefresh;

        auto  Now = std::chrono::high_resolution_clock::now();
        float Elapsed = std::chrono::duration<float>(Now - LastBoneRefresh).count();
        if (Elapsed >= s.Config.RandomBoneRefreshRate) {
            LastBoneRefresh = Now;
            std::uniform_int_distribution<int> Dist(0, 3);
            switch (Dist(RNG)) {
            case 0:
                s.CurrentBone = Cache::Player::BoneIdx::Head;
                break;
            case 1:
                s.CurrentBone = Cache::Player::BoneIdx::Neck;
                break;
            case 2:
                s.CurrentBone = Cache::Player::BoneIdx::Chest;
                break;
            case 3:
                s.CurrentBone = Cache::Player::BoneIdx::Pelvis;
                break;
            }
        }
    } else {
        s.CurrentBone = static_cast<Cache::Player::BoneIdx>(s.Config.Bone);
    }
}

static void UpdateGravityScales() {
    static std::vector<SDK::AFortProjectileBase*> Projectiles;
    SDK::GetAllActorsOfClass<SDK::AFortProjectileBase>(Projectiles);
    for (auto& Proj : Projectiles) {
        if (Proj->IsA(SDK::AFortProjectileBase::StaticClass())) {
            s.GravityScales[Proj->Class] = Proj->GravityScale;
        }
    }
}

static void DetectAmmoType() {
    auto* Pawn = SDK::Cast<SDK::AFortPawn>(SDK::GetLocalPawn());
    if (!Pawn) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    SDK::AFortWeapon* Weapon = Pawn->CurrentWeapon;
    if (!Weapon) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    static SDK::AFortWeapon* LastWeapon = nullptr;
    if (Weapon == LastWeapon)
        return;
    LastWeapon = Weapon;

    s.UseProjectile = false;
    SDK::UFortWeaponItemDefinition* WeaponData = Weapon->WeaponData;
    if (!WeaponData) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    if (auto* RangedWeaponData = SDK::Cast<SDK::UFortWeaponRangedItemDefinition>(WeaponData)) {
        if (auto* ProjTemp = RangedWeaponData->ProjectileTemplate.Get()) {
            if (auto* DefaultProjTemp = SDK::Cast<SDK::AFortProjectileBase>(ProjTemp->ClassDefaultObject)) {
                s.UseProjectile = true;
                s.ProjectileTemp = DefaultProjTemp;
            }
        }
    }

    SDK::UFortItemDefinition* AmmoData = WeaponData->AmmoData.Get();
    if (!AmmoData) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    if (!AmmoData->IsA(SDK::UFortItemDefinition::StaticClass()))
        Error::ThrowError("Open a GitHub issue and let me know which game version you are on!");

    auto AmmoName = AmmoData->DisplayName.ToString();
    if (AmmoName == "Ammo: Shells")
        s.CurrentAmmo = AmmoType::Shells;
    else if (AmmoName == "Ammo: Light Bullets")
        s.CurrentAmmo = AmmoType::Light;
    else if (AmmoName == "Ammo: Medium Bullets")
        s.CurrentAmmo = AmmoType::Medium;
    else if (AmmoName == "Ammo: Heavy Bullets")
        s.CurrentAmmo = AmmoType::Heavy;
    else
        s.CurrentAmmo = AmmoType::Other;
}

static void UpdateConfig() {
    s.Config = s.CurrentAmmo != AmmoType::Unknown && Config::g_Config.Aimbot.SplitAimbotByAmmo
                   ? [&] {
                         switch (s.CurrentAmmo) {
                         case AmmoType::Shells:
                             return Config::g_Config.Aimbot.ShellsAimbot;
                         case AmmoType::Light:
                             return Config::g_Config.Aimbot.LightAimbot;
                         case AmmoType::Medium:
                             return Config::g_Config.Aimbot.MediumAimbot;
                         case AmmoType::Heavy:
                             return Config::g_Config.Aimbot.HeavyAimbot;
                         default:
                             return Config::g_Config.Aimbot.OtherAimbot;
                         }
                     }()
                   : Config::g_Config.Aimbot.AllAimbot;
}

static void UpdateSmoothness() {
    static std::chrono::time_point<std::chrono::high_resolution_clock> LastFrameTime;
    auto                                                               Now = std::chrono::high_resolution_clock::now();
    float Elapsed = std::chrono::duration<float>(Now - LastFrameTime).count();
    LastFrameTime = Now;

    if (s.Config.Smoothness <= 1.f)
        s.EffectiveSmooth = s.Config.Smoothness;
    else
        s.EffectiveSmooth = std::clamp(s.Config.Smoothness * ((1.f / 120.f) / Elapsed), 1.f, FLT_MAX);
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    SetCurrentBone();
    UpdateGravityScales();
    DetectAmmoType();
    UpdateConfig();
    UpdateSmoothness();
}

} // namespace WeaponUtils
} // namespace Features
