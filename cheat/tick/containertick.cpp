#include "containertick.h"

#include <format>

#include <cheat/cache/containercache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

namespace Tick {
namespace Container {

// --- Container Utility Functions -----------------------------------

static void RenderContainer(const SDK::FVector2D& RootScreenLocation, const std::string& FormatStr,
                            const SDK::FLinearColor& Color, float DistM, float MaxDist, float FadeStart) {
    if (DistM <= MaxDist) {
        float Alpha = 1.0f;
        if (DistM > FadeStart)
            Alpha = 1.0f - ((DistM - FadeStart) / (MaxDist - FadeStart));

        SDK::FLinearColor OutlineColor = SDK::FLinearColor::Black;
        OutlineColor.A = Alpha;

        char Buffer[128];
        snprintf(Buffer, sizeof(Buffer), FormatStr.c_str(), static_cast<int>(DistM));
        Drawing::Text(Buffer, RootScreenLocation, SDK::FLinearColor(Color.R, Color.G, Color.B, Alpha), 12.f, true, true,
                      true, 1.f, OutlineColor);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    Cache::Container::UpdateCache();
}
void TickRenderThread() {
    const auto& LootConfig = Config::g_Config.Visuals.Loot;
    bool        ChestText = LootConfig.ChestText;
    bool        AmmoBoxText = LootConfig.AmmoBoxText;
    bool        SupplyDropText = LootConfig.SupplyDropText;
    bool        LlamaText = LootConfig.LlamaText;
    if (!ChestText && !AmmoBoxText && !SupplyDropText && !LlamaText)
        return;

    float MaxDistChest = LootConfig.ChestMaxDistance;
    float MaxDistAmmoBox = LootConfig.AmmoBoxMaxDistance;
    float MaxDistSupplyDrop = LootConfig.SupplyDropMaxDistance;
    float MaxDistLlama = LootConfig.LlamaMaxDistance;
    float FadeStartChest = LootConfig.ChestFadeOff ? MaxDistChest * 0.2f : FLT_MAX;
    float FadeStartAmmoBox = LootConfig.AmmoBoxFadeOff ? MaxDistAmmoBox * 0.2f : FLT_MAX;
    float FadeStartSupplyDrop = LootConfig.SupplyDropFadeOff ? MaxDistSupplyDrop * 0.2f : FLT_MAX;
    float FadeStartLlama = LootConfig.LlamaFadeOff ? MaxDistLlama * 0.2f : FLT_MAX;

    for (const auto& [_, Info] : Cache::Container::GetCachedContainers()) {
        if (Info.AlreadySearched)
            continue;

        float DistM = Info.RootWorldLocation.Dist(Core::g_CameraLocation) / 100.f;
        if (Info.Type == Cache::Container::ContainerType::Chest && ChestText) {
            RenderContainer(Info.RootScreenLocation, "Chest\n[%d m]", SDK::FLinearColor(0.95f, 0.68f, 0.1f, 1.f), DistM,
                            MaxDistChest, FadeStartChest);
        } else if (Info.Type == Cache::Container::ContainerType::AmmoBox && AmmoBoxText) {
            RenderContainer(Info.RootScreenLocation, "Ammo Box\n[%d m]", SDK::FLinearColor(0.1f, 0.6f, 0.f, 1.f), DistM,
                            MaxDistAmmoBox, FadeStartAmmoBox);
        } else if (Info.Type == Cache::Container::ContainerType::SupplyDrop && SupplyDropText) {
            RenderContainer(Info.RootScreenLocation, "SupplyDrop\n[%d m]", SDK::FLinearColor(0.2f, 0.4f, 1.f, 1.f),
                            DistM, MaxDistSupplyDrop, FadeStartSupplyDrop);
        } else if (Info.Type == Cache::Container::ContainerType::Llama && LlamaText) {
            RenderContainer(Info.RootScreenLocation, "Llama\n[%d m]", SDK::FLinearColor(0.8f, 0.3f, 1.f, 1.f), DistM,
                            MaxDistLlama, FadeStartLlama);
        }
    }
}

} // namespace Container
} // namespace Tick
