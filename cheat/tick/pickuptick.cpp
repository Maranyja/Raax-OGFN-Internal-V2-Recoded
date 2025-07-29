#include "pickuptick.h"

#include <format>

#include <cheat/cache/pickupcache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

namespace Tick {
namespace Pickup {

// --- Pickup Utility Functions --------------------------------------

SDK::FLinearColor GetTierColor(SDK::EFortItemTier Tier) {
    switch (Tier) {
        // Common
    case SDK::EFortItemTier::I:
        return SDK::FLinearColor(0.74f, 0.74f, 0.71f, 1.0f);

        // Uncommon
    case SDK::EFortItemTier::II:
        return SDK::FLinearColor(0.12f, 0.87f, 0.11f, 1.0f);

        // Rare
    case SDK::EFortItemTier::III:
        return SDK::FLinearColor(0.29f, 0.33f, 0.95f, 1.0f);

        // Epic
    case SDK::EFortItemTier::IV:
        return SDK::FLinearColor(0.65f, 0.27f, 0.82f, 1.0f);

        // Legendary
    case SDK::EFortItemTier::V:
        return SDK::FLinearColor(0.95f, 0.40f, 0.07f, 1.0f);

        // Mythic
    case SDK::EFortItemTier::VI:
        return SDK::FLinearColor(0.98f, 0.85f, 0.29f, 1.0f);

    default:
        return SDK::FLinearColor(0.74f, 0.74f, 0.71f, 1.0f);
    }

    return SDK::FLinearColor(1.f, 1.f, 1.f, 1.f);
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    Cache::Pickup::UpdateCache();
}

void TickRenderThread() {
    const auto& LootConfig = Config::g_Config.Visuals.Loot;
    if (!LootConfig.LootText)
        return;

    float MaxDist = LootConfig.LootMaxDistance;
    float FadeStart = LootConfig.LootFadeOff ? MaxDist * 0.2f : FLT_MAX;

    for (const auto& [_, Info] : Cache::Pickup::GetCachedPickups()) {
        // If the tier enum changes this check will break, but it doesnt change afaik so idc
        if (static_cast<int>(Info.Tier) < static_cast<int>(LootConfig.LootMinTier) + 1)
            continue;

        float DistM = Info.RootWorldLocation.Dist(Core::g_CameraLocation) / 100.f;
        if (DistM > MaxDist)
            continue;

        SDK::FLinearColor Color = GetTierColor(Info.Tier);
        if (DistM > FadeStart)
            Color.A = 1.0f - (DistM - FadeStart) / (MaxDist - FadeStart);

        SDK::FLinearColor OutlineColor = SDK::FLinearColor::Black;
        OutlineColor.A = Color.A;

        char Buffer[128];
        snprintf(Buffer, sizeof(Buffer), "%s\n[%d m]", Info.WeaponName.c_str(), static_cast<int>(DistM));
        Drawing::Text(Buffer, Info.RootScreenLocation, Color, 12.f, true, true, true, 1.f, OutlineColor);
    }
}

} // namespace Pickup
} // namespace Tick
