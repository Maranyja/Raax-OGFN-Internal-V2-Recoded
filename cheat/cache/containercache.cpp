#include "containercache.h"

#include <optional>
#include <chrono>

#include <config/config.h>
#include <utils/error.h>

namespace Cache {
namespace Container {

// --- Cache Data ----------------------------------------------------

static std::unordered_map<void*, ContainerInfo> CachedContainers;

// --- Cache Utility Functions ---------------------------------------

static std::optional<ContainerInfo> CreateNewInfo(SDK::ABuildingContainer* Actor, ContainerType Type) {
    ContainerInfo Info = {};
    Info.Actor = Actor;
    Info.RootComponent = Info.Actor->RootComponent;
    if (!Info.RootComponent)
        return std::nullopt;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    Info.Type = Type;
    Info.AlreadySearched = Info.Actor->bAlreadySearched;

    Info.SeenThisFrame = true;
    return Info;
}

static bool UpdateExistingInfo(ContainerInfo& Info) {
    Info.RootComponent = Info.Actor->RootComponent;
    if (!Info.RootComponent)
        return false;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    Info.AlreadySearched = Info.Actor->bAlreadySearched;

    Info.SeenThisFrame = true;
    return true;
}

static void ResetSeenFlags() {
    for (auto& [_, Info] : CachedContainers) {
        Info.SeenThisFrame = false;
    }
}

static void RemoveUnseen() {
    std::erase_if(CachedContainers, [](const auto& Entry) { return !Entry.second.SeenThisFrame; });
}

static void CacheActorsOfType(std::vector<SDK::ABuildingContainer*>& ActorList, SDK::UClass* Class,
                              ContainerType Type) {
    SDK::GetAllActorsOfClassAllLevels(ActorList, Class);
    for (const auto& Actor : ActorList) {
        if (!CachedContainers.contains(Actor)) {
            auto Info = CreateNewInfo(Actor, Type);
            if (Info.has_value())
                CachedContainers[Actor] = Info.value();
        } else {
            auto& Info = CachedContainers.at(Actor);
            if (!UpdateExistingInfo(Info))
                CachedContainers.erase(Actor);
        }
    }
}

SDK::AFortAthenaMapInfo* GetMapInfo() {
    SDK::UWorld* World = SDK::GetWorld();
    if (!World)
        return nullptr;

    SDK::AGameStateBase* GameState = World->GameState;
    if (!GameState)
        return nullptr;

    auto GameStateAthena = SDK::Cast<SDK::AFortGameStateAthena>(GameState);
    if (!GameStateAthena)
        return nullptr;

    return GameStateAthena->MapInfo;
}

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, ContainerInfo>& GetCachedContainers() {
    return CachedContainers;
}

void UpdateCache() {
    ResetSeenFlags();

    SDK::AFortAthenaMapInfo* MapInfo = GetMapInfo();
    if (MapInfo) {
        const auto& Config = Config::g_Config.Visuals.Loot;
        if (Config.ChestText && MapInfo->TreasureChestClass) {
            static std::vector<SDK::ABuildingContainer*> ChestList;
            CacheActorsOfType(ChestList, MapInfo->TreasureChestClass, ContainerType::Chest);
        }

        if (Config.AmmoBoxText && MapInfo->AmmoBoxClass) {
            static std::vector<SDK::ABuildingContainer*> AmmoBoxList;
            CacheActorsOfType(AmmoBoxList, MapInfo->TreasureChestClass, ContainerType::AmmoBox);
        }

        if (Config.SupplyDropText) {
            static std::vector<SDK::ABuildingContainer*> SupplyDropList;
            if (MapInfo->getpropinfo_SupplyDropClass(true).Found) {
                CacheActorsOfType(SupplyDropList, MapInfo->SupplyDropClass, ContainerType::SupplyDrop);
            } else if (MapInfo->getpropinfo_SupplyDropInfoList(true).Found) {
                SDK::TArray<SDK::UFortSupplyDropInfo*>& SupplyDropInfoList = MapInfo->SupplyDropInfoList;
                for (int i = 0; i < SupplyDropInfoList.Num(); i++) {
                    SDK::UFortSupplyDropInfo* SupplyDropInfo = SupplyDropInfoList[i];
                    if (SupplyDropInfo && SupplyDropInfo->SupplyDropClass) {
                        CacheActorsOfType(SupplyDropList, SupplyDropInfo->SupplyDropClass, ContainerType::SupplyDrop);
                    }
                }
            } else {
                Error::ThrowError("Failed to find supply drop class! Please open an issue on GitHub.");
            }
        }

        if (Config.LlamaText && MapInfo->getpropinfo_LlamaClass(true).Found && MapInfo->LlamaClass) {
            static std::vector<SDK::ABuildingContainer*> LlamaList;
            CacheActorsOfType(LlamaList, MapInfo->LlamaClass, ContainerType::Llama);
        }
    }

    RemoveUnseen();
}

} // namespace Container
} // namespace Cache
