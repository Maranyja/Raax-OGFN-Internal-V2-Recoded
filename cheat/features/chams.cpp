#include "chams.h"

#include <array>
#include <chrono>

#include <cheat/cache/playercache.h>
#include <cheat/core.h>
#include <cheat/sdk/sdk.h>
#include <config/config.h>
#include <utils/error.h>

namespace Features {
namespace Chams {

// --- Chams State Management ----------------------------------------

struct ChamsState {
    std::array<SDK::TArray<SDK::UMaterialInterface*>, 8> OriginalMaterials = {};
    SDK::UMaterialInstanceDynamic*                       ChamsMaterial = nullptr;
    bool                                                 HasChamsApplied = false;
    std::chrono::steady_clock::time_point                LastApplyTime;
};
std::unordered_map<SDK::AFortPlayerPawn*, ChamsState> ChamsAppliedPlayers;

// --- Chams Utility Functions ---------------------------------------

static SDK::UMaterial* GetMaterial() {
    static SDK::UMaterial* Material = SDK::UObject::FindObjectFast<SDK::UMaterial>("RezIn_Master");
    if (!Material)
        Error::ThrowError("Failed to find material for chams!");

    return Material;
}

static SDK::UMaterialInstanceDynamic* CreateMaterialInstance(SDK::AFortPlayerPawn* Pawn) {
    if (!Pawn)
        return nullptr;

    return SDK::UKismetMaterialLibrary::CreateDynamicMaterialInstance(Pawn, GetMaterial(), SDK::FName());
}

static void UpdateMaterialSettings(SDK::UMaterialInstanceDynamic* MaterialInstance, const SDK::FLinearColor& Color,
                                   float EmissionIntensity) {
    if (!MaterialInstance)
        return;

    static SDK::FName ChamsColorParams[] = {SDK::FName(L"WireFrameParameterHighlight"),
                                            SDK::FName(L"WireFrameFadeOffColor"), SDK::FName(L"Top Color"),
                                            SDK::FName(L"Bottom Color")};
    static SDK::FName EmissionParams[] = {SDK::FName(L"Emissive Modulation")};

    for (auto& Param : ChamsColorParams) {
        MaterialInstance->SetVectorParameterValue(Param, Color);
    }
    for (auto& Param : EmissionParams) {
        MaterialInstance->SetScalarParameterValue(Param, EmissionIntensity);
    }
}

static void ApplyChamsToPlayer(SDK::AFortPlayerPawn* Pawn, const SDK::FLinearColor& Color, float EmissionIntensity) {
    auto MeshComponents = Pawn->GetCharacterPartSkeletalMeshComponents();
    if (MeshComponents.empty())
        return;

    bool HasMaterialsToReplace = false;
    for (auto* Comp : MeshComponents) {
        if (Comp && Comp->GetMaterials().Num() > 0) {
            HasMaterialsToReplace = true;
            break;
        }
    }

    if (!HasMaterialsToReplace)
        return;

    auto& State = ChamsAppliedPlayers[Pawn];

    if (State.ChamsMaterial) {
        State.ChamsMaterial = nullptr;
    }

    State.ChamsMaterial = CreateMaterialInstance(Pawn);
    if (!State.ChamsMaterial) {
        ChamsAppliedPlayers.erase(Pawn);
        return;
    }

    UpdateMaterialSettings(State.ChamsMaterial, Color, EmissionIntensity);

    for (size_t i = 0; i < MeshComponents.size(); i++) {
        SDK::USkeletalMeshComponent* Comp = MeshComponents[i];
        if (!Comp)
            continue;

        if (!State.HasChamsApplied)
            State.OriginalMaterials[i] = Comp->GetMaterials();

        for (int j = 0; j < State.OriginalMaterials[i].Num(); j++) {
            Comp->SetMaterial(j, State.ChamsMaterial);
        }
    }

    State.HasChamsApplied = true;
    State.LastApplyTime = std::chrono::steady_clock::now();
}

static void RemoveChamsFromPlayer(SDK::AFortPlayerPawn* Pawn) {
    auto It = ChamsAppliedPlayers.find(Pawn);
    if (It == ChamsAppliedPlayers.end() || !It->second.HasChamsApplied)
        return;

    auto& State = It->second;

    auto MeshComponents = Pawn->GetCharacterPartSkeletalMeshComponents();
    for (size_t i = 0; i < MeshComponents.size(); i++) {
        SDK::USkeletalMeshComponent* Comp = MeshComponents[i];
        if (!Comp)
            continue;

        auto& OriginalMaterials = State.OriginalMaterials[i];
        for (int j = 0; j < OriginalMaterials.Num(); j++) {
            Comp->SetMaterial(j, OriginalMaterials[j]);
        }
    }

    if (State.ChamsMaterial)
        State.ChamsMaterial = nullptr;

    State.HasChamsApplied = false;
}

static void CleanupRemovedPlayers() {
    auto CachedPlayers = Cache::Player::GetCachedPlayers();
    for (auto It = ChamsAppliedPlayers.begin(); It != ChamsAppliedPlayers.end();) {
        if (CachedPlayers.find(It->first) == CachedPlayers.end()) {
            It = ChamsAppliedPlayers.erase(It);
        } else {
            ++It;
        }
    }
}

// --- Public Functions ----------------------------------------------

void Destroy() {
    for (auto& [Pawn, _] : ChamsAppliedPlayers) {
        RemoveChamsFromPlayer(Pawn);
    }

    ChamsAppliedPlayers.clear();
}

void TickGameThread() {
    auto&       Config = Config::g_Config.Visuals.Player;
    static bool LastChamsState = false;

    CleanupRemovedPlayers();

    if (LastChamsState && !Config.Chams) {
        for (auto& [Pawn, _] : ChamsAppliedPlayers) {
            RemoveChamsFromPlayer(Pawn);
        }
    }
    LastChamsState = Config.Chams;

    if (!Config.Chams)
        return;

    SDK::UMaterial* Material = GetMaterial();
    Material->bDisableDepthTest = Config.ChamsThroughWalls;
    Material->BlendMode = SDK::EBlendMode::BLEND_Additive;
    Material->WireFrame = Config.ChamsWireframe;

    constexpr auto ReapplyInterval = std::chrono::seconds(2);
    auto           CurrentTime = std::chrono::steady_clock::now();

    for (const auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (!Config.ChamsOnSelf && Info.Pawn == SDK::GetLocalPawn()) {
            RemoveChamsFromPlayer(Info.Pawn);
            continue;
        }

        auto It = ChamsAppliedPlayers.find(Info.Pawn);
        bool NeedsChams = (It == ChamsAppliedPlayers.end()) || !It->second.HasChamsApplied;
        bool NeedsReapply =
            (It != ChamsAppliedPlayers.end()) && (CurrentTime - It->second.LastApplyTime >= ReapplyInterval);

        if (NeedsChams || NeedsReapply) {
            ApplyChamsToPlayer(Info.Pawn, Config.ChamsColor, Config.ChamsEmissionIntensity);
        }
    }
}

} // namespace Chams
} // namespace Features
