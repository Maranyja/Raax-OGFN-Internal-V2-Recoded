#pragma once
#include <cheat/sdk/sdk.h>

namespace Tick {
namespace Pickup {

// --- Pickup Utility Functions --------------------------------------

SDK::FLinearColor GetTierColor(SDK::EFortItemTier Tier);

// --- Public Tick Functions -----------------------------------------

void TickGameThread();
void TickRenderThread();

} // namespace Pickup
} // namespace Tick
