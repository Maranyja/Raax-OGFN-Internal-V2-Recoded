#include "autorevertfeature.h"

namespace AutoRevertFeature {

// --- Auto Revert Classes -------------------------------------------

AutoRevertBitFieldFeature::AutoRevertBitFieldFeature(uint8_t* Address, const bool* Enabled, uint8_t BitMask)
    : Address(Address), OriginalValue(*Address & BitMask), // Store ONLY the original state of our bit
      BitMask(BitMask), Enabled(Enabled) {
    if (*Enabled && Memory::IsValidAndWritable(Address)) {
        *Address |= BitMask;
    }
}

AutoRevertBitFieldFeature::~AutoRevertBitFieldFeature() {
    if (Memory::IsValidAndWritable(Address)) {
        *Address = (*Address & ~BitMask) | OriginalValue;
    }
}

bool AutoRevertBitFieldFeature::IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const {
    return this->Address == Address && this->Enabled == Enabled && this->BitMask == BitMask;
}

bool AutoRevertBitFieldFeature::Tick() {
    if (!Memory::IsValidAndWritable(Address))
        return false;

    if (*Enabled) {
        *Address |= BitMask;
    } else {
        *Address = (*Address & ~BitMask) | OriginalValue;
        return false;
    }

    return true;
}

// --- Public Creation Functions -------------------------------------

void CreateBitField(uint8_t* Address, const bool* Enabled, uint8_t BitMask) {
    auto Feature = std::make_unique<AutoRevertBitFieldFeature>(Address, Enabled, BitMask);
    for (const auto& ExistingFeature : g_AutoRevertFeatures) {
        if (ExistingFeature->IsDuplicate(Address, Enabled, BitMask))
            return;
    }
    g_AutoRevertFeatures.push_back(std::move(Feature));
}

// --- Public Initialization & Tick Functions ------------------------

void Tick() {
    auto It = g_AutoRevertFeatures.begin();
    while (It != g_AutoRevertFeatures.end()) {
        if (!(*It)->Tick())
            It = g_AutoRevertFeatures.erase(It);
        else
            ++It;
    }
}

void Destroy() {
    g_AutoRevertFeatures.clear();
}

// --- Global Variables ----------------------------------------------

std::vector<std::unique_ptr<IAutoRevertFeature>> g_AutoRevertFeatures;

} // namespace AutoRevertFeature
