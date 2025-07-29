#pragma once
#include <unordered_map>

#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Container {

// --- Cache Structures ----------------------------------------------

enum class ContainerType { NONE = 0, Chest, AmmoBox, Llama, SupplyDrop };

struct ContainerInfo {
    SDK::ABuildingContainer* Actor = nullptr;
    SDK::USceneComponent*    RootComponent = nullptr;

    SDK::FVector   RootWorldLocation;
    SDK::FVector2D RootScreenLocation;

    ContainerType Type = ContainerType::NONE;
    bool          AlreadySearched = false;

    bool SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, ContainerInfo>&     GetCachedContainers();

void UpdateCache();

} // namespace Container
} // namespace Cache
