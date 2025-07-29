#include "Engine.h"

#include <cheat/sdk/sdk.h>
#include <cheat/core.h>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

class FBatchedElements* FCanvas::GetBatchElements() {
    return FCanvasGetBatchedElements(this, EElementType::ET_Line, nullptr, nullptr,
                                     ESimpleElementBlendMode::SE_BLEND_MAX, FDepthFieldGlowInfo(), true);
}

FString UKismetSystemLibrary::GetEngineVersion() {
    static UFunction* Func = GetFunction("KismetSystemLibrary", "GetEngineVersion");
    struct {
        FString ReturnValue;
    } params_GetEngineVersion{};

    StaticClass()->ProcessEvent(Func, &params_GetEngineVersion);
    return std::move(params_GetEngineVersion.ReturnValue);
}
bool UKismetSystemLibrary::LineTraceSingle(UObject* WorldContextObject, const FVector& Start, const FVector& End,
                                           ETraceTypeQuery TraceChannel, bool bTraceComplex,
                                           TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace DrawDebugType,
                                           FHitResult& OutHit, bool bIgnoreSelf, const FLinearColor& TraceColor,
                                           const FLinearColor& TraceHitColor, float DrawTime) {
    return KismetSystemLibraryLineTraceSingle(WorldContextObject, Start, End, TraceChannel, bTraceComplex,
                                              ActorsToIgnore, EDrawDebugTrace::None, OutHit, bIgnoreSelf,
                                              FLinearColor::White, FLinearColor::White, 0.f);
}

UMaterialInstanceDynamic* UKismetMaterialLibrary::CreateDynamicMaterialInstance(class UObject* WorldContextObject,
                                                                                class UMaterialInterface* Parent,
                                                                                class FName OptionalName) {
    static UFunction* Func = GetFunction("KismetMaterialLibrary", "CreateDynamicMaterialInstance");
    if (g_EngineVersion >= 4.24) {
        struct {
            UObject*                  WorldContextObject;
            UMaterialInterface*       Parent;
            EMIDCreationFlags         CreationFlags;
            FName                     OptionalName;
            uint8_t                   Pad[0x7];
            UMaterialInstanceDynamic* ReturnValue;
        } params_CreateDynamicMaterialInstance1{};

        params_CreateDynamicMaterialInstance1.WorldContextObject = WorldContextObject;
        params_CreateDynamicMaterialInstance1.Parent = Parent;
        params_CreateDynamicMaterialInstance1.CreationFlags = EMIDCreationFlags::None;
        params_CreateDynamicMaterialInstance1.OptionalName = OptionalName;

        StaticClass()->ProcessEvent(Func, &params_CreateDynamicMaterialInstance1);
        return params_CreateDynamicMaterialInstance1.ReturnValue;
    } else if (g_EngineVersion >= 4.21) {
        struct {
            UObject*                  WorldContextObject;
            UMaterialInterface*       Parent;
            FName                     OptionalName;
            UMaterialInstanceDynamic* ReturnValue;
        } params_CreateDynamicMaterialInstance2{};

        params_CreateDynamicMaterialInstance2.WorldContextObject = WorldContextObject;
        params_CreateDynamicMaterialInstance2.Parent = Parent;
        params_CreateDynamicMaterialInstance2.OptionalName = OptionalName;

        StaticClass()->ProcessEvent(Func, &params_CreateDynamicMaterialInstance2);
        return params_CreateDynamicMaterialInstance2.ReturnValue;
    } else {
        struct {
            UObject*                  WorldContextObject;
            UMaterialInterface*       Parent;
            UMaterialInstanceDynamic* ReturnValue;
        } params_CreateDynamicMaterialInstance3{};

        params_CreateDynamicMaterialInstance3.WorldContextObject = WorldContextObject;
        params_CreateDynamicMaterialInstance3.Parent = Parent;

        StaticClass()->ProcessEvent(Func, &params_CreateDynamicMaterialInstance3);
        return params_CreateDynamicMaterialInstance3.ReturnValue;
    }
}

void UCanvas::K2_DrawLine(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness,
                          const FLinearColor& RenderColor) {
#if 0
    static UFunction* Func = GetFunction("Canvas", "K2_DrawLine");
    struct {
        FVector2D    ScreenPositionA;
        FVector2D    ScreenPositionB;
        float        Thickness;
        FLinearColor RenderColor;
    } params_K2_DrawLine{};

    params_K2_DrawLine.ScreenPositionA = ScreenPositionA;
    params_K2_DrawLine.ScreenPositionB = ScreenPositionB;
    params_K2_DrawLine.Thickness = Thickness;
    params_K2_DrawLine.RenderColor = RenderColor;

    if (Func)
        ProcessEvent(Func, &params_K2_DrawLine);
#else
    FCanvas* CanvasPtr = Canvas;
    if (!CanvasPtr)
        return;

    FBatchedElements* BatchElements = CanvasPtr->GetBatchElements();
    if (!BatchElements)
        return;

    FBatchedThickLines ThickLine = {};
    ThickLine.Start = FVector(ScreenPositionA.X, ScreenPositionA.Y, 0.f);
    ThickLine.End = FVector(ScreenPositionB.X, ScreenPositionB.Y, 0.f);
    ThickLine.Thickness = Thickness;
    ThickLine.Color = RenderColor;
    ThickLine.HitProxyColor = {};
    ThickLine.DepthBias = Thickness;
    ThickLine.bScreenSpace = 0;

    TArray<FBatchedThickLines>& BatchedThickLines = BatchElements->BatchedThickLines;
    BatchedThickLines.Add(ThickLine);
#endif
}
void UCanvas::K2_DrawText(UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition,
                          const FVector2D& Scale, const FLinearColor& RenderColor, float Kerning,
                          const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY,
                          bool bOutlined, const FLinearColor& OutlineColor) {
    static UFunction* Func = GetFunction("Canvas", "K2_DrawText");
    if (g_EngineVersion >= 4.22f) {
        struct {
            UFont*       RenderFont;
            FString      RenderText;
            FVector2D    ScreenPosition;
            FVector2D    Scale;
            FLinearColor RenderColor;
            float        Kerning;
            FLinearColor ShadowColor;
            FVector2D    ShadowOffset;
            bool         bCentreX;
            bool         bCentreY;
            bool         bOutlined;
            FLinearColor OutlineColor;
        } params_K2_DrawText_1{};

        params_K2_DrawText_1.RenderFont = RenderFont;
        params_K2_DrawText_1.RenderText = RenderText;
        params_K2_DrawText_1.ScreenPosition = {ScreenPosition.X, ScreenPosition.Y};
        params_K2_DrawText_1.Scale = Scale;
        params_K2_DrawText_1.RenderColor = RenderColor;
        params_K2_DrawText_1.Kerning = Kerning;
        params_K2_DrawText_1.ShadowColor = ShadowColor;
        params_K2_DrawText_1.ShadowOffset = ShadowOffset;
        params_K2_DrawText_1.bCentreX = bCentreX;
        params_K2_DrawText_1.bCentreY = bCentreY;
        params_K2_DrawText_1.bOutlined = bOutlined;
        params_K2_DrawText_1.OutlineColor = OutlineColor;

        ProcessEvent(Func, &params_K2_DrawText_1);
    } else {
        struct {
            UFont*       RenderFont;
            FString      RenderText;
            FVector2D    ScreenPosition;
            FLinearColor RenderColor;
            float        Kerning;
            FLinearColor ShadowColor;
            FVector2D    ShadowOffset;
            bool         bCentreX;
            bool         bCentreY;
            bool         bOutlined;
            FLinearColor OutlineColor;
        } params_K2_DrawText_2{};

        params_K2_DrawText_2.RenderFont = RenderFont;
        params_K2_DrawText_2.RenderText = RenderText;
        params_K2_DrawText_2.ScreenPosition = {ScreenPosition.X, ScreenPosition.Y};
        params_K2_DrawText_2.RenderColor = RenderColor;
        params_K2_DrawText_2.Kerning = Kerning;
        params_K2_DrawText_2.ShadowColor = ShadowColor;
        params_K2_DrawText_2.ShadowOffset = ShadowOffset;
        params_K2_DrawText_2.bCentreX = bCentreX;
        params_K2_DrawText_2.bCentreY = bCentreY;
        params_K2_DrawText_2.bOutlined = bOutlined;
        params_K2_DrawText_2.OutlineColor = OutlineColor;

        ProcessEvent(Func, &params_K2_DrawText_2);
    }
}

FTransform USceneComponent::getprop_ComponentToWorld() const {
    if (ComponentToWorld_Offset) {
        return *(FTransform*)((uintptr_t)this + ComponentToWorld_Offset);
    } else {
        static UFunction* Func = GetFunction("SceneComponent", "K2_GetComponentToWorld");
        struct {
            FTransform ReturnValue;
            uint8_t    Pad[0x30]; // No idea why, but this fixes crash on ProcessEvent on 3.5
        } params_ComponentToWorld{};

        ProcessEvent(Func, &params_ComponentToWorld);
        return params_ComponentToWorld.ReturnValue;
    }
}

void UPrimitiveComponent::SetMaterial(int32_t ElementIndex, UMaterialInterface* Material) {
    static UFunction* Func = GetFunction("PrimitiveComponent", "SetMaterial");
    struct {
        int32_t             ElementIndex;
        uint8_t             Pad[0x4];
        UMaterialInterface* Material;
    } params_SetMaterial{};

    params_SetMaterial.ElementIndex = ElementIndex;
    params_SetMaterial.Material = Material;

    ProcessEvent(Func, &params_SetMaterial);
}

TArray<UMaterialInterface*> UMeshComponent::GetMaterials() {
    static UFunction* Func = GetFunction("MeshComponent", "GetMaterials");
    struct {
        TArray<UMaterialInterface*> ReturnValue;
    } params_GetMaterials{};

    ProcessEvent(Func, &params_GetMaterials);
    return std::move(params_GetMaterials.ReturnValue);
}

TArray<FTransform>& USkinnedMeshComponent::getprop_ComponentSpaceTransformsArray() const {
    TArray<FTransform>& FirstArray = *(TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset);
    if (FirstArray.IsValid())
        return FirstArray;

    TArray<FTransform>& SecondArray =
        *(TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset + sizeof(TArray<FTransform>));
    return SecondArray; // Assume second array is valid
}
int32_t USkinnedMeshComponent::GetBoneIndex(FName BoneName) const {
    static UFunction* Func = GetFunction("SkinnedMeshComponent", "GetBoneIndex");
    struct {
        FName   BoneName;
        int32_t ReturnValue;
    } params_GetBoneIndex{};

    params_GetBoneIndex.BoneName = BoneName;

    ProcessEvent(Func, &params_GetBoneIndex);
    return params_GetBoneIndex.ReturnValue;
}

FTransform USkeletalMeshComponent::GetBoneMatrix(int32_t BoneIndex) const {
    TArray<FTransform>& Array = ComponentSpaceTransformsArray;
    if (Array.IsValid())
        return Array.GetByIndex(BoneIndex);

    return FTransform();
}
FVector USkeletalMeshComponent::GetBoneLocation(int32_t BoneIndex, FTransform ComponentToWrld) const {
    FTransform BoneMatrix = GetBoneMatrix(BoneIndex);
    FMatrix    Matrix = BoneMatrix.ToMatrixWithScale() * ComponentToWrld.ToMatrixWithScale();
    return FVector(Matrix.M[3][0], Matrix.M[3][1], Matrix.M[3][2]);
}

void UMaterialInstanceDynamic::SetVectorParameterValue(FName ParameterName, const FLinearColor& Value) {
    static UFunction* Func = GetFunction("MaterialInstanceDynamic", "SetVectorParameterValue");
    struct {
        FName        ParameterName;
        FLinearColor Value;
    } params_SetVectorParameterValue{};

    params_SetVectorParameterValue.ParameterName = ParameterName;
    params_SetVectorParameterValue.Value = Value;

    ProcessEvent(Func, &params_SetVectorParameterValue);
}
void UMaterialInstanceDynamic::SetScalarParameterValue(FName ParameterName, float Value) {
    static UFunction* Func = GetFunction("MaterialInstanceDynamic", "SetScalarParameterValue");
    struct {
        FName ParameterName;
        float Value;
    } params_SetScalarParameterValue{};

    params_SetScalarParameterValue.ParameterName = ParameterName;
    params_SetScalarParameterValue.Value = Value;

    ProcessEvent(Func, &params_SetScalarParameterValue);
}

FVector APlayerCameraManager::GetCameraLocation() const {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraLocation");
    struct {
        FVector ReturnValue;
    } params_GetCameraLocation{};

    ProcessEvent(Func, &params_GetCameraLocation);

    return params_GetCameraLocation.ReturnValue;
}
FRotator APlayerCameraManager::GetCameraRotation() const {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraRotation");
    struct {
        FRotator ReturnValue;
    } params_GetCameraRotation{};

    ProcessEvent(Func, &params_GetCameraRotation);

    return params_GetCameraRotation.ReturnValue;
}
float APlayerCameraManager::GetFOVAngle() const {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetFOVAngle");
    struct {
        float ReturnValue;
    } params_GetFOVAngle{};

    ProcessEvent(Func, &params_GetFOVAngle);

    return params_GetFOVAngle.ReturnValue;
}

void APlayerController::AddYawInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddYawInput");
    struct {
        float Val;
    } params_AddYawInput{};

    params_AddYawInput.Val = Val;

    ProcessEvent(Func, &params_AddYawInput);
}
void APlayerController::AddPitchInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddPitchInput");
    struct {
        float Val;
    } params_AddPitchInput{};

    params_AddPitchInput.Val = Val;

    ProcessEvent(Func, &params_AddPitchInput);
}
bool APlayerController::WasInputKeyJustReleased(const FKey& Key) const {
    static UFunction* Func = GetFunction("PlayerController", "WasInputKeyJustReleased");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_WasInputKeyJustReleased{};

    params_WasInputKeyJustReleased.Key = Key;

    ProcessEvent(Func, &params_WasInputKeyJustReleased);
    return params_WasInputKeyJustReleased.ReturnValue;
}
bool APlayerController::WasInputKeyJustPressed(const FKey& Key) const {
    static UFunction* Func = GetFunction("PlayerController", "WasInputKeyJustPressed");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_WasInputKeyJustPressed{};

    params_WasInputKeyJustPressed.Key = Key;

    ProcessEvent(Func, &params_WasInputKeyJustPressed);
    return params_WasInputKeyJustPressed.ReturnValue;
}
bool APlayerController::IsInputKeyDown(const FKey& Key) const {
    static UFunction* Func = GetFunction("PlayerController", "IsInputKeyDown");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_IsInputKeyDown{};

    params_IsInputKeyDown.Key = Key;

    ProcessEvent(Func, &params_IsInputKeyDown);
    return params_IsInputKeyDown.ReturnValue;
}
bool APlayerController::GetMousePosition(float& LocationX, float& LocationY) const {
    static UFunction* Func = GetFunction("PlayerController", "GetMousePosition");
    struct {
        float   LocationX;
        float   LocationY;
        bool    ReturnValue;
        uint8_t Pad[3];
    } params_GetMousePosition{};

    ProcessEvent(Func, &params_GetMousePosition);

    LocationX = params_GetMousePosition.LocationX;
    LocationY = params_GetMousePosition.LocationY;

    return params_GetMousePosition.ReturnValue;
}
void APlayerController::ServerChangeName(const FString& S) {
    static UFunction* Func = GetFunction("PlayerController", "ServerChangeName");
    struct {
        FString S;
    } params_ServerChangeName{};

    ProcessEvent(Func, &params_ServerChangeName);
}

FString APlayerState::GetPlayerName() const {
    static UFunction* Func = GetFunction("PlayerState", "GetPlayerName", true);
    if (Func) {
        struct {
            FString return_value;
        } params_GetPlayerName{};

        ProcessEvent(Func, &params_GetPlayerName);
        return std::move(params_GetPlayerName.return_value);
    }
    else {
        // Some old game versions, like 1.8, doesn't have a function for getting the player name
        return PlayerName;
    }
}

// --- Public Functions ----------------------------------------------

ULocalPlayer* GetLocalPlayer() {
    return Core::g_LocalPlayer;
}
APlayerController* GetLocalController() {
    return Core::g_LocalPlayerController;
}
APawn* GetLocalPawn() {
    return Core::g_LocalPawn;
}

FVector GetCameraLocation() {
    return Core::g_CameraLocation;
}
FRotator GetCameraRotation() {
    return Core::g_CameraRotation;
}
float GetFOV() {
    return Core::g_FOV;
}

UEngine* GetEngine() {
    static UEngine* GEngine = nullptr;
    if (!GEngine) {
        for (int i = 0; i < UObject::Objects.Num(); i++) {
            UObject* Obj = UObject::Objects.GetByIndex(i);
            if (!Obj)
                continue;

            if (Obj->IsA(UEngine::StaticClass()) && !Obj->IsDefaultObject()) {
                GEngine = static_cast<UEngine*>(Obj);
                break;
            }
        }
    }

    return GEngine;
}
UWorld* GetWorld() {
    return Core::g_World;
}
UCanvas* GetCanvas() {
    static UCanvas* GCanvas = nullptr;
    if (!GCanvas || !GCanvas->Class || !GCanvas->IsA(UCanvas::StaticClass())) {
        for (int i = 0; i < UObject::Objects.Num(); i++) {
            UObject* Obj = UObject::Objects.GetByIndex(i);
            if (!Obj)
                continue;

            if (Obj->IsA(UCanvas::StaticClass()) && !Obj->IsDefaultObject() && Obj->Name == "DebugCanvasObject") {
                GCanvas = static_cast<UCanvas*>(Obj);
                break;
            }
        }
    }

    return GCanvas;
}

bool IsPositionVisible(const FVector& Position, AActor* IgnoredActor, AActor* IgnoredActor2) {
    static TArray<AActor*> IgnoredActors(2);
    IgnoredActors.Clear();
    if (IgnoredActor) {
        IgnoredActors.Add(IgnoredActor);
    }
    if (IgnoredActor2) {
        IgnoredActors.Add(IgnoredActor2);
    }

    FHitResult Hit = {};
    bool       bHitSomething = UKismetSystemLibrary::LineTraceSingle(
        GetWorld(), Core::g_CameraLocation, Position, ETraceTypeQuery::TraceTypeQuery21, true, IgnoredActors,
        EDrawDebugTrace::None, Hit, true, FLinearColor(0.f, 0.f, 0.f, 0.f), FLinearColor(0.f, 0.f, 0.f, 0.f), 0.f);

    return !bHitSomething;
}

FVector Project3D(const FVector& Location) {
    UCanvas* Canvas = GetCanvas();
    if (!Canvas)
        return FVector();

    FMatrix& ViewProjectionMatrix = Canvas->ViewProjectionMatrix;
    float    X = Location.X * ViewProjectionMatrix.M[0][0] + Location.Y * ViewProjectionMatrix.M[1][0] +
              Location.Z * ViewProjectionMatrix.M[2][0] + ViewProjectionMatrix.M[3][0];
    float Y = Location.X * ViewProjectionMatrix.M[0][1] + Location.Y * ViewProjectionMatrix.M[1][1] +
              Location.Z * ViewProjectionMatrix.M[2][1] + ViewProjectionMatrix.M[3][1];
    float Z = Location.X * ViewProjectionMatrix.M[0][2] + Location.Y * ViewProjectionMatrix.M[1][2] +
              Location.Z * ViewProjectionMatrix.M[2][2] + ViewProjectionMatrix.M[3][2];
    float W = Location.X * ViewProjectionMatrix.M[0][3] + Location.Y * ViewProjectionMatrix.M[1][3] +
              Location.Z * ViewProjectionMatrix.M[2][3] + ViewProjectionMatrix.M[3][3];

    X /= W;
    Y /= W;
    Z /= W;

    float ScreenX = (X + 1.f) * Core::g_ScreenCenterX;
    float ScreenY = (1.f - Y) * Core::g_ScreenCenterY;
    float ScreenZ = Z;

    return FVector(ScreenX, ScreenY, ScreenZ);
}
FVector2D Project(const FVector& Location) {
    FVector ScreenLocation = Project3D(Location);

    // If the depth (Z) value is not in front of the camera.
    if (ScreenLocation.Z < 0.f)
        return FVector2D(-1.f, -1.f);

    return FVector2D(ScreenLocation.X, ScreenLocation.Y);
}

} // namespace SDK
