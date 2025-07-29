#pragma once
#include <unordered_map>

#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Pickup {

// --- Cache Structures ----------------------------------------------

struct PickupInfo {
    SDK::AFortPickup*     Pickup = nullptr;
    SDK::USceneComponent* RootComponent = nullptr;

    SDK::FVector   RootWorldLocation;
    SDK::FVector2D RootScreenLocation;

    std::string        WeaponName;
    SDK::EFortItemTier Tier = SDK::EFortItemTier::No_Tier;

    bool SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PickupInfo>& GetCachedPickups();
void                                         UpdateCache();

} // namespace Pickup
} // namespace Cache
