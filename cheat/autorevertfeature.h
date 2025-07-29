#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

namespace AutoRevertFeature {

// --- Auto Revert Classes -------------------------------------------

class IAutoRevertFeature {
  public:
    virtual ~IAutoRevertFeature() = default;
    virtual bool IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const = 0;
    virtual bool Tick() = 0;
};

template <typename T> class AutoRevertFeature : public IAutoRevertFeature {
  private:
    T*          Address;
    T           OriginalValue;
    const bool* Enabled;

  public:
    AutoRevertFeature(T* Address, const bool* Enabled);
    ~AutoRevertFeature() override;
    bool IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const override;
    bool Tick() override;
};

class AutoRevertBitFieldFeature : public IAutoRevertFeature {
  private:
    uint8_t*    Address;
    bool        OriginalValue;
    uint8_t     BitMask;
    const bool* Enabled;

  public:
    AutoRevertBitFieldFeature(uint8_t* Address, const bool* Enabled, uint8_t BitMask);
    ~AutoRevertBitFieldFeature() override;
    bool IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const override;
    bool Tick() override;
};

template <typename T> class AutoRevertScalingFeature : public IAutoRevertFeature {
  private:
    T*           Address;
    T            OriginalValue;
    T            TargetValue;
    bool         FirstTick;
    float        LastScalarValue;
    const float* Scalar;
    const bool*  Enabled;

  public:
    AutoRevertScalingFeature(T* Address, T TargetValue, const float* Scalar, const bool* Enabled);
    ~AutoRevertScalingFeature() override;
    bool IsDuplicate(void* Address, const bool* Enabled, uint8_t BitMask) const override;
    bool Tick() override;
};

// --- Public Creation Functions -------------------------------------

template <typename T> void Create(T* Address, const bool* Enabled);
void                       CreateBitField(uint8_t* Address, const bool* Enabled, uint8_t BitMask);
template <typename T> void CreateScaling(T* Address, T TargetValue, const float* Scalar, const bool* Enabled);

// --- Public Initialization & Tick Functions ------------------------

void Tick();
void Destroy();

// --- Global Variables ----------------------------------------------

extern std::vector<std::unique_ptr<IAutoRevertFeature>> g_AutoRevertFeatures;

} // namespace AutoRevertFeature

#include "autorevertfeature.inl"
