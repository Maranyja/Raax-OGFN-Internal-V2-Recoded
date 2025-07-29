#pragma once
#include <unordered_map>

#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Player {

// --- Cache Structures ----------------------------------------------

enum class BoneIdx : int {
    MIN = 0,
    Head = 0,
    Neck,
    Pelvis,
    Chest,

    L_Shoulder,
    L_Elbow,
    L_Hand,
    R_Shoulder,
    R_Elbow,
    R_Hand,

    L_Thigh,
    L_Knee,
    L_Foot,
    R_Thigh,
    R_Knee,
    R_Foot,

    NUM
};

struct PlayerInfo {
    // Required for cache to be valid

    SDK::AFortPlayerPawnAthena*  Pawn = nullptr;
    SDK::AFortPlayerStateAthena* PlayerState = nullptr;
    SDK::USkeletalMeshComponent* Mesh = nullptr;
    SDK::USceneComponent*        RootComponent = nullptr;

    // Weapon Info

    SDK::AFortWeapon*  CurrentWeapon = nullptr;
    SDK::EFortItemTier WeaponTier = SDK::EFortItemTier::No_Tier;
    std::string        WeaponName;
    int32_t            BulletsPerClip = 0;
    int32_t            AmmoCount = 0;

    // Player Info

    std::string PlayerName;
    std::string Platform;
    uint8_t     TeamIndex = 0;
    float       DistanceM = 0.f;
    bool        IsDBNO = false;
    bool        IsDead = false;
    bool        HeadVisible = false;
    bool        IsOnScreen = false;

    // Bone and position info

    int32_t        BoneIndicies[(int)BoneIdx::NUM] = {};
    SDK::FVector   RootWorldLocation;
    SDK::FVector   RootComponentVelocity;
    SDK::FVector   BoneWorldPos[(int)BoneIdx::NUM];
    SDK::FVector2D BoneScreenPos[(int)BoneIdx::NUM];
    SDK::FVector2D BoundTopLeft, BoundBottomRight;
    SDK::FVector   BoundCorners3D[8];
    SDK::FVector2D BoundCorners2D[8];
    SDK::FVector2D BoxTop, BoxMiddle, BoxBottom;

    bool SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PlayerInfo>& GetCachedPlayers();
void                                         UpdateCache();

} // namespace Player
} // namespace Cache
