#pragma once
#include "Engine.h"

#include <array>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

enum class EFortItemTier : uint8_t {
    No_Tier = 0,
    I = 1,
    II = 2,
    III = 3,
    IV = 4,
    V = 5,
    VI = 6,
    VII = 7,
    VIII = 8,
    IX = 9,
    X = 10,
    NumItemTierValues = 11,
    EFortItemTier_MAX = 12,
};

// TODO - Make these not inherit from UObject, as that is wrong

struct FFortItemEntry : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortItemEntry", FFortItemEntry);

  public:
    UPROPERTY(class UFortItemDefinition*, ItemDefinition);
};

struct FFortBaseWeaponStats : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortBaseWeaponStats", FFortBaseWeaponStats);

  public:
    UPROPERTY(float, ReloadTime);
    UPROPERTY(int32_t, AmmoCostPerFire);
    UPROPERTY(float, MinChargeTime);
    UPROPERTY(float, MaxChargeTime);
    UPROPERTY(float, ChargeDownTime);
    UPROPERTY(float, MinChargeDamageMultiplier);
    UPROPERTY(float, MaxChargeDamageMultiplier);
};

struct FFortMeleeWeaponStats : public FFortBaseWeaponStats {
  public:
    STATICCLASS_DEFAULTOBJECT("FortMeleeWeaponStats", FFortMeleeWeaponStats);

  public:
    UPROPERTY(float, SwingPlaySpeed);
};

struct FFortRangedWeaponStats : public FFortBaseWeaponStats {
  public:
    STATICCLASS_DEFAULTOBJECT("FortRangedWeaponStats", FFortRangedWeaponStats);

  public:
    UPROPERTY(float, Spread);
    UPROPERTY(float, SpreadDownsights);
    UPROPERTY(float, StandingStillSpreadMultiplier);
    UPROPERTY(float, AthenaCrouchingSpreadMultiplier);
    UPROPERTY(float, AthenaJumpingFallingSpreadMultiplier);
    UPROPERTY(float, AthenaSprintingSpreadMultiplier);
    UPROPERTY(float, MinSpeedForSpreadMultiplier);
    UPROPERTY(float, MaxSpeedForSpreadMultiplier);
    UPROPERTY(float, RecoilVert);
    UPROPERTY(float, RecoilVertScaleGamepad);
    UPROPERTY(float, RecoilHoriz);
    UPROPERTY(int32_t, BulletsPerCartridge);
    UPROPERTY(float, FiringRate);
};

class FZiplinePawnState : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("ZiplinePawnState", FZiplinePawnState);

  public:
    UPROPERTY_BITFIELD(bIsZiplining);
};

class FFortCameraInstanceEntry final : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortCameraInstanceEntry", FFortCameraInstanceEntry);

  public:
    UPROPERTY(class UFortCameraMode*, Camera);
};

class UFortSupplyDropInfo final : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortSupplyDropInfo", UFortSupplyDropInfo);

  public:
    UPROPERTY(TSubclassOf<class ABuildingContainer>, SupplyDropClass);
};

class AFortGameStateAthena : public AGameStateBase {
  public:
    STATICCLASS_DEFAULTOBJECT("FortGameStateAthena", AFortGameStateAthena);

  public:
    UPROPERTY(class AFortAthenaMapInfo*, MapInfo);
};

class AFortAthenaMapInfo : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortAthenaMapInfo", AFortAthenaMapInfo);

  public:
    UPROPERTY(TSubclassOf<class ABuildingContainer>, TreasureChestClass);
    UPROPERTY(TSubclassOf<class ABuildingContainer>, AmmoBoxClass);
    UPROPERTY(TSubclassOf<class ABuildingContainer>, LlamaClass);
    UPROPERTY(TSubclassOf<class ABuildingContainer>, SupplyDropClass); // Older versions of Fortnite
    UPROPERTY(TArray<class UFortSupplyDropInfo*>, SupplyDropInfoList); // Newer versions of Fortnite
};

class AFortPlayerController : public APlayerController {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerController", AFortPlayerController);

  public:
    static inline void (*pFire_Press)(void*) = nullptr;
    static inline void (*pFire_Release)(void*) = nullptr;

  public:
    void Fire_Press();
    void Fire_Release();
};

class AFortPawn : public ACharacter {
  public:
    static inline FTransform* (*pGetTargetingTransform)(
        FTransform* result, void* /* FGameplayAbilityTargetingLocationInfo* */ TargetingLocationInfo,
        uint8_t /* EFortAbilityTargetingSource */ Source) = nullptr;

  public:
    STATICCLASS_DEFAULTOBJECT("FortPawn", AFortPawn);

  public:
    UPROPERTY(class AFortWeapon*, CurrentWeapon);
    UPROPERTY_BITFIELD(bIsDying);
    UPROPERTY_BITFIELD(bIsDBNO);
};

class AFortPlayerPawn : public AFortPawn {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerPawn", AFortPlayerPawn);

  public:
    UPROPERTY(FZiplinePawnState, ZiplineState);

  public:
    std::array<USkeletalMeshComponent*, 8> GetCharacterPartSkeletalMeshComponents();

  public:
    void ServerReviveFromDBNO(SDK::AController* EventInstigator);
};

class AFortPlayerPawnAthena : public AFortPlayerPawn {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerPawnAthena", AFortPlayerPawnAthena);

  public:
    UPROPERTY(float, ReviveFromDBNOTime);
    UPROPERTY_BITFIELD(bADSWhileNotOnGround);
};

class AFortPlayerState : public APlayerState {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerState", AFortPlayerState);

  public:
    UPROPERTY(FString, Platform);
};

class AFortPlayerStateAthena : public AFortPlayerState {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerStateAthena", AFortPlayerStateAthena);

  public:
    UPROPERTY(uint8_t, TeamIndex);
};

class ABuildingContainer : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("BuildingContainer", ABuildingContainer);

  public:
    UPROPERTY_BITFIELD(bAlreadySearched);
};

class ABuildingActor : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("BuildingActor", ABuildingActor);
};

class AFortPickup : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPickup", AFortPickup);

  public:
    UPROPERTY(struct FFortItemEntry, PrimaryPickupItemEntry);
};

class AFortWeapon : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeapon", AFortWeapon);

  public:
    static inline int GetWeaponStats_Idx;

  public:
    UPROPERTY(class UFortWeaponItemDefinition*, WeaponData);
    UPROPERTY(int32_t, AmmoCount);
    UPROPERTY(float, LastFireTime);
    UPROPERTY_BITFIELD(bIgnoreTryToFireSlotCooldownRestriction);

  public:
    int32_t GetBulletsPerClip() const;

  public:
    FFortBaseWeaponStats* GetWeaponStats();
};

class AFortWeaponRanged : public AFortWeapon {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRanged", AFortWeaponRanged);
};

class AB_Melee_Generic_C : public AFortWeapon {
  public:
    STATICCLASS_DEFAULTOBJECT("B_Melee_Generic_C", AB_Melee_Generic_C);
};

class AFortProjectileBase : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortProjectileBase", AFortProjectileBase);

  public:
    UPROPERTY(float, GravityScale);

  public:
    float GetDefaultGravityScale();
    float GetDefaultSpeed(float InChargePercent);
};

class UFortItemDefinition : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortItemDefinition", UFortItemDefinition);

  public:
    UPROPERTY(FText, DisplayName);
    UPROPERTY(EFortItemTier, Tier);
};

enum class EFortWeaponTriggerType : uint8_t {
    OnPress = 0,
    Automatic = 1,
    OnRelease = 2,
    OnPressAndRelease = 3,
    EFortWeaponTriggerType_MAX = 4,
};

class UFortWeaponItemDefinition : public UFortItemDefinition {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponItemDefinition", UFortWeaponItemDefinition);

  public:
    UPROPERTY(TSoftObjectPtr<UFortItemDefinition>, AmmoData);
    UPROPERTY(EFortWeaponTriggerType, TriggerType);
};

class UFortWeaponRangedItemDefinition : public UFortWeaponItemDefinition {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRangedItemDefinition", UFortWeaponRangedItemDefinition);

  public:
    UPROPERTY(TSoftClassPtr<UClass>, ProjectileTemplate);
};

} // namespace SDK
