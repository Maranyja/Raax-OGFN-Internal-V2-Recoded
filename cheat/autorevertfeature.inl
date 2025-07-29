#pragma once
#include <utils/memory.h>

namespace AutoRevertFeature {

// --- Auto Revert Classes -------------------------------------------

template <typename T>
AutoRevertFeature<T>::AutoRevertFeature(T* Address, const bool* Enabled)
    : Address(Address), OriginalValue(*Address), Enabled(Enabled) {}

template <typename T> AutoRevertFeature<T>::~AutoRevertFeature() {
    if (Memory::IsValidAndWritable(Address))
        *Address = OriginalValue;
}

template <typename T>
bool AutoRevertFeature<T>::IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const {
    return this->Address == Address && this->Enabled == Enabled;
}

template <typename T> bool AutoRevertFeature<T>::Tick() {
    if (!*Enabled && Memory::IsValidAndWritable(Address)) {
        *Address = OriginalValue;
        return false;
    }
    return true;
}

template <typename T>
AutoRevertScalingFeature<T>::AutoRevertScalingFeature(T* Address, T TargetValue, const float* Scalar,
                                                      const bool* Enabled)
    : Address(Address), OriginalValue(*Address), TargetValue(TargetValue), LastScalarValue(*Scalar), FirstTick(true),
      Scalar(Scalar), Enabled(Enabled) {}

template <typename T> AutoRevertScalingFeature<T>::~AutoRevertScalingFeature() {
    if (Memory::IsValidAndWritable(Address))
        *Address = OriginalValue;
}

template <typename T>
bool AutoRevertScalingFeature<T>::IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const {
    return this->Address == Address && this->Enabled == Enabled;
}

template <typename T> bool AutoRevertScalingFeature<T>::Tick() {
    if (!*Enabled)
        return false;

    if (FirstTick || *Scalar != LastScalarValue) {
        if (!Memory::IsValidAndWritable(Address))
            return false;
        *Address = static_cast<T>(OriginalValue + ((TargetValue - OriginalValue) * *Scalar));
        LastScalarValue = *Scalar;
        FirstTick = false;
    }
    return true;
}

// --- Public Creation Functions -------------------------------------

template <typename T> void Create(T* Address, const bool* Enabled) {
    auto Feature = std::make_unique<AutoRevertFeature<T>>(Address, Enabled);
    for (const auto& ExistingFeature : g_AutoRevertFeatures) {
        if (ExistingFeature->IsDuplicate(Address, Enabled, 0))
            return;
    }
    g_AutoRevertFeatures.push_back(std::move(Feature));
}

template <typename T> void CreateScaling(T* Address, T TargetValue, const float* Scalar, const bool* Enabled) {
    auto Feature = std::make_unique<AutoRevertScalingFeature<T>>(Address, TargetValue, Scalar, Enabled);
    for (const auto& ExistingFeature : g_AutoRevertFeatures) {
        if (ExistingFeature->IsDuplicate(Address, Enabled, 0))
            return;
    }
    g_AutoRevertFeatures.push_back(std::move(Feature));
}

} // namespace AutoRevertFeature
