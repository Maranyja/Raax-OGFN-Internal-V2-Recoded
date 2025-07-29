#include "pickupcache.h"

#include <chrono>

#include <config/config.h>

namespace Cache {
namespace Pickup {

// --- Cache Data ----------------------------------------------------

static std::unordered_map<void*, PickupInfo> CachedPickups;

// --- Cache Utility Functions ---------------------------------------

static std::optional<PickupInfo> CreateNewPickupInfo(SDK::AFortPickup* Pickup) {
    PickupInfo Info;
    Info.Pickup = Pickup;
    Info.RootComponent = Info.Pickup->RootComponent;
    if (!Info.RootComponent)
        return std::nullopt;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    SDK::FFortItemEntry       ItemEntry = Pickup->PrimaryPickupItemEntry;
    SDK::UFortItemDefinition* ItemDefinition = ItemEntry.ItemDefinition;
    if (!ItemDefinition)
        return std::nullopt;

    Info.WeaponName = ItemDefinition->DisplayName.ToString();
    Info.Tier = ItemDefinition->Tier;

    Info.SeenThisFrame = true;
    return Info;
}

static bool UpdateExistingPickupInfo(PickupInfo& Info) {
    Info.RootComponent = Info.Pickup->RootComponent;
    if (!Info.RootComponent)
        return false;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    Info.SeenThisFrame = true;
    return true;
}

static void ResetPickupSeenFlags() {
    for (auto& [_, Cache] : CachedPickups) {
        Cache.SeenThisFrame = false;
    }
}

static void RemoveUnseenPickups() {
    std::erase_if(CachedPickups, [](const auto& Cache) { return !Cache.second.SeenThisFrame; });
}

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PickupInfo>& GetCachedPickups() {
    return CachedPickups;
}

void UpdateCache() {
    ResetPickupSeenFlags();

    const auto& Config = Config::g_Config.Visuals.Loot;
    if (Config.LootText) {
        static std::vector<SDK::AFortPickup*> PickupList;
        SDK::GetAllActorsOfClass<SDK::AFortPickup>(PickupList);
        for (const auto& Pickup : PickupList) {
            if (!CachedPickups.contains(Pickup)) {
                std::optional<PickupInfo> Info = CreateNewPickupInfo(Pickup);
                if (Info.has_value())
                    CachedPickups[Pickup] = Info.value();
            } else {
                PickupInfo& Info = CachedPickups.at(Pickup);
                if (!UpdateExistingPickupInfo(Info))
                    CachedPickups.erase(Pickup);
            }
        }
    }

    RemoveUnseenPickups();
}

} // namespace Pickup
} // namespace Cache
