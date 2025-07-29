#pragma once
#include "CoreUObject.h"
#include "Containers.h"
#include "Basic.h"

#include <vector>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

enum class ETraceTypeQuery : uint8_t {
    TraceTypeQuery1 = 0,
    TraceTypeQuery2 = 1,
    TraceTypeQuery3 = 2,
    TraceTypeQuery4 = 3,
    TraceTypeQuery5 = 4,
    TraceTypeQuery6 = 5,
    TraceTypeQuery7 = 6,
    TraceTypeQuery8 = 7,
    TraceTypeQuery9 = 8,
    TraceTypeQuery10 = 9,
    TraceTypeQuery11 = 10,
    TraceTypeQuery12 = 11,
    TraceTypeQuery13 = 12,
    TraceTypeQuery14 = 13,
    TraceTypeQuery15 = 14,
    TraceTypeQuery16 = 15,
    TraceTypeQuery17 = 16,
    TraceTypeQuery18 = 17,
    TraceTypeQuery19 = 18,
    TraceTypeQuery20 = 19,
    TraceTypeQuery21 = 20,
    TraceTypeQuery22 = 21,
    TraceTypeQuery23 = 22,
    TraceTypeQuery24 = 23,
    TraceTypeQuery25 = 24,
    TraceTypeQuery26 = 25,
    TraceTypeQuery27 = 26,
    TraceTypeQuery28 = 27,
    TraceTypeQuery29 = 28,
    TraceTypeQuery30 = 29,
    TraceTypeQuery31 = 30,
    TraceTypeQuery32 = 31,
    TraceTypeQuery_MAX = 32,
    ETraceTypeQuery_MAX = 33,
};

enum class EDrawDebugTrace : uint8_t {
    None = 0,
    ForOneFrame = 1,
    ForDuration = 2,
    Persistent = 3,
    EDrawDebugTrace_MAX = 4,
};

enum class EMIDCreationFlags : uint8_t {
    None = 0,
    Transient = 1,
    EMIDCreationFlags_MAX = 2,
};

enum class EBlendMode : uint8_t {
    BLEND_Opaque = 0,
    BLEND_Masked = 1,
    BLEND_Translucent = 2,
    BLEND_Additive = 3,
    BLEND_Modulate = 4,
    BLEND_AlphaComposite = 5,
    BLEND_MAX = 6,
};

struct FHitResult {
  public:
    char UnknownData[0x100]; // Dummy data
};

struct FKey {
  public:
    class FName KeyName;
    uint8_t     Pad[16];
};

enum EElementType {
    ET_Box,
    ET_DebugQuad,
    ET_Text,
    ET_ShapedText,
    ET_Spline,
    ET_Line,
    ET_Gradient,
    ET_Viewport,
    ET_Border,
    ET_Custom,
    ET_CustomVerts,
    ET_PostProcessPass,
    ET_Count,
};
enum ESimpleElementBlendMode {
    SE_BLEND_Opaque = 0,
    SE_BLEND_Masked,
    SE_BLEND_Translucent,
    SE_BLEND_Additive,
    SE_BLEND_Modulate,
    SE_BLEND_MaskedDistanceField,
    SE_BLEND_MaskedDistanceFieldShadowed,
    SE_BLEND_TranslucentDistanceField,
    SE_BLEND_TranslucentDistanceFieldShadowed,
    SE_BLEND_AlphaComposite,
    SE_BLEND_AlphaHoldout,
    SE_BLEND_AlphaBlend,
    SE_BLEND_TranslucentAlphaOnly,
    SE_BLEND_TranslucentAlphaOnlyWriteAlpha,
    SE_BLEND_RGBA_MASK_START,
    SE_BLEND_RGBA_MASK_END = SE_BLEND_RGBA_MASK_START + 31,
    SE_BLEND_MAX,
};
struct FDepthFieldGlowInfo {
    uint32_t     bEnableGlow : 1 = 0;
    FLinearColor GlowColor;
    FVector2D    GlowOuterRadius;
    FVector2D    GlowInnerRadius;
};
struct FBatchedThickLines {
    FVector      Start;
    FVector      End;
    float        Thickness;
    FLinearColor Color;
    FColor       HitProxyColor;
    float        DepthBias;
    uint32_t     bScreenSpace;
};
class FHitProxyId {
    int32_t index;
};

class FCanvas {
  public:
    static inline class FBatchedElements* (*FCanvasGetBatchedElements)(FCanvas*, EElementType, void*, void*,
                                                                       ESimpleElementBlendMode,
                                                                       const FDepthFieldGlowInfo&, bool);

  public:
    class FBatchedElements* GetBatchElements();
};

class FBatchedElements {
  public:
    static inline uint32_t BatchedThickLines_Offset;

  public:
    UPROPERTY_OFFSET(TArray<FBatchedThickLines>, BatchedThickLines, BatchedThickLines_Offset);
};

class UKismetSystemLibrary : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("KismetSystemLibrary", UKismetSystemLibrary);

  public:
    static inline bool (*KismetSystemLibraryLineTraceSingle)(UObject*, const FVector&, const FVector&, ETraceTypeQuery,
                                                             bool, TArray<class AActor*>&, EDrawDebugTrace, FHitResult&,
                                                             bool, const FLinearColor&, const FLinearColor&, float);

  public:
    static FString GetEngineVersion();
    static bool    LineTraceSingle(UObject* WorldContextObject, const FVector& Start, const FVector& End,
                                   ETraceTypeQuery TraceChannel, bool bTraceComplex, TArray<AActor*>& ActorsToIgnore,
                                   EDrawDebugTrace DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf,
                                   const FLinearColor& TraceColor, const FLinearColor& TraceHitColor, float DrawTime);
};

class UKismetMaterialLibrary : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("KismetMaterialLibrary", UKismetMaterialLibrary);

  public:
    static class UMaterialInstanceDynamic* CreateDynamicMaterialInstance(class UObject*            WorldContextObject,
                                                                         class UMaterialInterface* Parent,
                                                                         class FName               OptionalName);
};

class UEngine : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Engine", UEngine);

  public:
    UPROPERTY(class UGameViewportClient*, GameViewport);
};

class UWorld : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("World", UWorld);

  public:
    UPROPERTY(class ULevel*, PersistentLevel);
    UPROPERTY(TArray<class ULevel*>, Levels);
    UPROPERTY(class AGameStateBase*, GameState);
};

class ULevel : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Level", ULevel);

  public:
    static inline uint32_t Actors_Offset;

  public:
    UPROPERTY_OFFSET(TArray<class AActor*>, Actors, Actors_Offset);
    UPROPERTY(class AWorldSettings*, WorldSettings);
};

class UGameInstance : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("GameInstance", UGameInstance);

  public:
    UPROPERTY(TArray<class ULocalPlayer*>, LocalPlayers);
};

class UGameViewportClient : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("GameViewportClient", UGameViewportClient);

  public:
    static inline int DrawTransition_Idx;

  public:
    UPROPERTY(class UWorld*, World);
    UPROPERTY(class UGameInstance*, GameInstance);
};

class UFont : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Font", UFont);

  public:
    UPROPERTY(int32_t, LegacyFontSize);
};

class UCanvas : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Canvas", UCanvas);

  public:
    static inline uint32_t ViewProjectionMatrix_Offset;
    static inline uint32_t Canvas_Offset;

  public:
    UPROPERTY_OFFSET(FCanvas*, Canvas, Canvas_Offset);
    UPROPERTY_OFFSET(FMatrix, ViewProjectionMatrix, ViewProjectionMatrix_Offset);
    UPROPERTY(int32_t, SizeX);
    UPROPERTY(int32_t, SizeY);

  public:
    void K2_DrawLine(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness,
                     const FLinearColor& RenderColor);
    void K2_DrawText(UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition,
                     const FVector2D& Scale, const FLinearColor& RenderColor, float Kerning,
                     const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY,
                     bool bOutlined, const FLinearColor& OutlineColor);
};

class UPlayer : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Player", UPlayer);

  public:
    UPROPERTY(class APlayerController*, PlayerController);
};

class ULocalPlayer : public UPlayer {
  public:
    STATICCLASS_DEFAULTOBJECT("LocalPlayer", ULocalPlayer);
};

class USceneComponent : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("SceneComponent", USceneComponent);

  public:
    static inline uint32_t ComponentToWorld_Offset;

  public:
    UPROPERTY(FVector, RelativeLocation);
    UPROPERTY(FVector, ComponentVelocity);
    FTransform                                                      getprop_ComponentToWorld() const;
    __declspec(property(get = getprop_ComponentToWorld)) FTransform ComponentToWorld;
};

class UPrimitiveComponent : public USceneComponent {
  public:
    STATICCLASS_DEFAULTOBJECT("PrimitiveComponent", UPrimitiveComponent);

  public:
    void SetMaterial(int32_t ElementIndex, UMaterialInterface* Material);
};

class UMeshComponent : public UPrimitiveComponent {
  public:
    STATICCLASS_DEFAULTOBJECT("MeshComponent", UMeshComponent);

  public:
    TArray<UMaterialInterface*> GetMaterials();
};

class USkinnedMeshComponent : public UMeshComponent {
  public:
    STATICCLASS_DEFAULTOBJECT("SkinnedMeshComponent", USkinnedMeshComponent);

  public:
    static inline uint32_t ComponentSpaceTransformsArray_Offset;

  public:
    TArray<FTransform>&                                               getprop_ComponentSpaceTransformsArray() const;
    __declspec(property(get = getprop_ComponentSpaceTransformsArray)) TArray<FTransform> ComponentSpaceTransformsArray;

  public:
    int32_t GetBoneIndex(FName BoneName) const;
};

class USkeletalMeshComponent : public USkinnedMeshComponent {
  public:
    STATICCLASS_DEFAULTOBJECT("SkeletalMeshComponent", USkeletalMeshComponent);

  public:
    FTransform GetBoneMatrix(int32_t BoneIndex) const;
    FVector    GetBoneLocation(int32_t BoneIndex, FTransform ComponentToWrld) const;
};

class UMaterialInterface : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("MaterialInterface", UMaterialInterface);
};

class UMaterial : public UMaterialInterface {
  public:
    STATICCLASS_DEFAULTOBJECT("Material", UMaterial);

  public:
    UPROPERTY_BITFIELD(bDisableDepthTest);
    UPROPERTY(EBlendMode, BlendMode);
    UPROPERTY_BITFIELD(WireFrame);
};

class UMaterialInstanceDynamic : public UMaterialInterface {
  public:
    STATICCLASS_DEFAULTOBJECT("MaterialInstanceDynamic", UMaterialInstanceDynamic);

  public:
    void SetVectorParameterValue(FName ParameterName, const FLinearColor& Value);
    void SetScalarParameterValue(FName ParameterName, float Value);
};

class AActor : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("Actor", AActor);

  public:
    UPROPERTY(class USceneComponent*, RootComponent);
};

class AGameStateBase : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("GameStateBase", AGameStateBase);

  public:
    UPROPERTY(float, GameState);
};

class AWorldSettings : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("WorldSettings", AWorldSettings);

  public:
    UPROPERTY(float, WorldGravityZ);
};

class APlayerCameraManager : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("PlayerCameraManager", APlayerCameraManager);

  public:
    FVector  GetCameraLocation() const;
    FRotator GetCameraRotation() const;
    float    GetFOVAngle() const;
};

class AController : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("Controller", AController);
};

class APlayerController : public AController {
  public:
    STATICCLASS_DEFAULTOBJECT("PlayerController", APlayerController);

  public:
    UPROPERTY(class APawn*, AcknowledgedPawn);
    UPROPERTY(class APlayerCameraManager*, PlayerCameraManager);
    UPROPERTY(float, InputYawScale);
    UPROPERTY(float, InputPitchScale);

  public:
    void AddYawInput(float Val);
    void AddPitchInput(float Val);
    bool WasInputKeyJustReleased(const FKey& Key) const;
    bool WasInputKeyJustPressed(const FKey& Key) const;
    bool IsInputKeyDown(const FKey& Key) const;
    bool GetMousePosition(float& LocationX, float& LocationY) const;
    void ServerChangeName(const FString& S);
};

class APawn : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("Pawn", APawn);

  public:
    UPROPERTY(class APlayerState*, PlayerState);
};

class ACharacter : public APawn {
  public:
    STATICCLASS_DEFAULTOBJECT("Character", ACharacter);

  public:
    UPROPERTY(class USkeletalMeshComponent*, Mesh);
};

class APlayerState : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("PlayerState", APlayerState);

  public:
    UPROPERTY(FString, PlayerName);

  public:
    FString GetPlayerName() const;
};

// --- Public Functions ----------------------------------------------

ULocalPlayer*      GetLocalPlayer();
APlayerController* GetLocalController();
APawn*             GetLocalPawn();

FVector  GetCameraLocation();
FRotator GetCameraRotation();
float    GetFOV();

UEngine* GetEngine();
UWorld*  GetWorld();
UCanvas* GetCanvas();

bool IsPositionVisible(const FVector& Position, AActor* IgnoredActor = nullptr, AActor* IgnoredActor2 = nullptr);

FVector   Project3D(const FVector& Location);
FVector2D Project(const FVector& Location);

template <typename UEType = AActor, bool ClearVector = true>
    requires std::is_base_of_v<AActor, UEType>
inline void GetAllActorsOfClass(std::vector<UEType*>& OutVector, UClass* Class = nullptr,
                                ULevel* Level = GetWorld()->PersistentLevel) {
    if constexpr (ClearVector)
        OutVector.clear();

    if (!Level)
        return;

    UClass* SearchClass = Class ? Class : UEType::StaticClass();
    if (!SearchClass)
        return;

    TArray<AActor*>& Actors = Level->Actors;
    if (!Actors.IsValid() || Actors.Num() <= 0)
        return;

    for (int i = 0; i < Actors.Num(); i++) {
        AActor* Actor = Actors.GetByIndex(i);
        if (!Actor)
            continue;

        if (Actor->IsA(SearchClass))
            OutVector.push_back(static_cast<UEType*>(Actor));
    }
}

template <typename UEType = AActor, bool ClearVector = true>
inline void GetAllActorsOfClassAllLevels(std::vector<UEType*>& OutVector, UClass* Class = nullptr) {
    if constexpr (ClearVector)
        OutVector.clear();

    UClass* SearchClass = Class ? Class : UEType::StaticClass();
    if (!SearchClass)
        return;

    TArray<ULevel*>& Levels = GetWorld()->Levels;
    if (!Levels.IsValid() || Levels.Num() <= 0)
        return;

    for (int i = 0; i < Levels.Num(); i++) {
        GetAllActorsOfClass<UEType, false>(OutVector, SearchClass, Levels.GetByIndex(i));
    }
}

} // namespace SDK
