#pragma once
#include <unordered_map>
#include <vector>
#include <random>

#include <cheat/cache/playercache.h>
#include <cheat/sdk/sdk.h>
#include <config/config.h>

namespace Features {
namespace WeaponUtils {

// --- State ---------------------------------------------------------

enum class AmmoType { Shells, Light, Medium, Heavy, Other, Unknown };

struct State {
    Config::ConfigData::AimbotConfig::AimbotAmmoConfig Config;
    AmmoType                                           CurrentAmmo = AmmoType::Shells;
    bool                                               UseProjectile = false;
    SDK::AFortProjectileBase*                          ProjectileTemp = nullptr;
    std::unordered_map<SDK::UClass*, float>            GravityScales;
    float                                              EffectiveSmooth = 1.f;
    Cache::Player::BoneIdx                             CurrentBone = Cache::Player::BoneIdx::Head;
    bool                                               IsTargeting = false;
};

State& GetState();

// --- Utility Functions ---------------------------------------------

SDK::FVector PredictProjectile(const SDK::FVector& Origin, const SDK::FVector& Target, const SDK::FVector& TargetVel,
                               float BulletSpeed, float GravityZ);

// --- Public Tick Functions -----------------------------------------

void TickGameThread();

} // namespace WeaponUtils
} // namespace Features
