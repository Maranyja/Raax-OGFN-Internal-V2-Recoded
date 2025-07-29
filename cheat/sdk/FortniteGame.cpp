#include "FortniteGame.h"

#include <utils/log.h>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

void AFortPlayerController::Fire_Press() {
    if (pFire_Press)
        pFire_Press(this);
}
void AFortPlayerController::Fire_Release() {
    if (pFire_Release)
        pFire_Release(this);
}

void AFortPlayerPawn::ServerReviveFromDBNO(AController* EventInstigator) {
    static UFunction* Func = GetFunction("FortPlayerPawn", "ServerReviveFromDBNO");
    struct {
        AController* EventInstigator;
    } params_ServerReviveFromDBNO{};

    params_ServerReviveFromDBNO.EventInstigator = EventInstigator;

    ProcessEvent(Func, &params_ServerReviveFromDBNO);
}

std::array<USkeletalMeshComponent*, 8> AFortPlayerPawn::GetCharacterPartSkeletalMeshComponents() {
    static PropertyInfo Prop = GetPropertyInfo("FortPlayerPawn", "CharacterPartSkeletalMeshComponents");
    std::array<USkeletalMeshComponent*, 8> Components = {};

    for (int i = 0; i < 8; i++) {
        auto Mesh = *(USkeletalMeshComponent**)((uintptr_t)this + Prop.Offset + (i * 8));
        if (Mesh && Mesh->IsA(USkeletalMeshComponent::StaticClass())) {
            Components[i] = Mesh;
        }
    }

    return Components;
}

int32_t AFortWeapon::GetBulletsPerClip() const {
    static UFunction* Func = GetFunction("FortWeapon", "GetBulletsPerClip");
    struct {
        int32_t ReturnValue;
    } params_GetBulletsPerClip{};

    ProcessEvent(Func, &params_GetBulletsPerClip);
    return params_GetBulletsPerClip.ReturnValue;
}
FFortBaseWeaponStats* AFortWeapon::GetWeaponStats() {
    FFortBaseWeaponStats* (*GetWeaponStatsFunc)(class AFortWeapon* Object) =
        (decltype(GetWeaponStatsFunc))this->VTable[GetWeaponStats_Idx];
    return GetWeaponStatsFunc(this);
}

float AFortProjectileBase::GetDefaultGravityScale() {
    static UFunction* Func = GetFunction("FortProjectileBase", "GetDefaultGravityScale");
    struct {
        float ReturnValue;
    } params_GetDefaultGravityScale{};

    ProcessEvent(Func, &params_GetDefaultGravityScale);
    return params_GetDefaultGravityScale.ReturnValue;
}

float AFortProjectileBase::GetDefaultSpeed(float InChargePercent) {
    // TODO: find another way to do this for older versions (3.5) that dont have AFortProjectileBase::GetDefaultSpeed

    static UFunction* Func = GetFunction("FortProjectileBase", "GetDefaultSpeed", true);
    if (Func) {
        struct {
            float InChargePercent;
            float ReturnValue;
        } params_GetDefaultSpeed{};

        params_GetDefaultSpeed.InChargePercent = InChargePercent;

        ProcessEvent(Func, &params_GetDefaultSpeed);
        return params_GetDefaultSpeed.ReturnValue;
    } else {
        static bool DisplayedWarning = false;
        if (!DisplayedWarning) {
            LOG(LOG_WARN, "Default projectile speed will likely be wrong! Check AFortProjectileBase::GetDefaultSpeed.");
            DisplayedWarning = true;
        }
        return 30000.f; // Common default bullet speed
    }
}

} // namespace SDK
