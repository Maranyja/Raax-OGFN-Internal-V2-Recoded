#pragma once
#include "Containers.h"

#include <cstdint>
#include <string>
#include <immintrin.h>

#include <globals.h>

namespace SDK {

enum EFindName { FNAME_Find, FNAME_Add };

class FName {
  public:
    static inline void (*FNameToString)(const FName*, class FString*) = nullptr;
    static inline void (*FNameConstructorW)(FName*, const wchar_t*, EFindName) = nullptr;

  public:
    int32_t ComparisonIndex;
    int32_t Number;

  public:
    FName() : ComparisonIndex(0), Number(0) {}
    FName(const wchar_t* Name);
    FName(const char* Name);

  public:
    class FString ToFString() const;
    std::wstring  ToWString() const;
    std::string   ToString() const;

    bool operator==(const FName& Other) const;
    bool operator!=(const FName& Other) const;

    void operator=(const FName& Other);
    void operator=(const wchar_t* Name);
    void operator=(const char* Name);
};
static_assert(alignof(FName) == 0x000004, "Wrong alignment on FName");
static_assert(sizeof(FName) == 0x000008, "Wrong size on FName");
static_assert(offsetof(FName, ComparisonIndex) == 0x000000, "Member 'FName::ComparisonIndex' has a wrong offset!");
static_assert(offsetof(FName, Number) == 0x000004, "Member 'FName::Number' has a wrong offset!");

class FText {
  public:
    void*   TextData;
    uint8_t Pad[0x10];

  public:
    FString&    Get() const;
    std::string ToString() const;
};
static_assert(alignof(FText) == 0x000008, "Wrong alignment on FText");
static_assert(sizeof(FText) == 0x000018, "Wrong size on FText");
static_assert(offsetof(FText, TextData) == 0x000000, "Member 'FText::TextData' has a wrong offset!");

class FWeakObjectPtr {
  public:
    int32_t ObjectIndex;
    int32_t ObjectSerialNumber;

  public:
    class UObject* Get() const;
    class UObject* operator->() const;
    bool           operator==(const FWeakObjectPtr& Other) const;
    bool           operator!=(const FWeakObjectPtr& Other) const;
    bool           operator==(const class UObject* Other) const;
    bool           operator!=(const class UObject* Other) const;
};
static_assert(alignof(FWeakObjectPtr) == 0x000004, "Wrong alignment on FWeakObjectPtr");
static_assert(sizeof(FWeakObjectPtr) == 0x000008, "Wrong size on FWeakObjectPtr");
static_assert(offsetof(FWeakObjectPtr, ObjectIndex) == 0x000000,
              "Member 'FWeakObjectPtr::ObjectIndex' has a wrong offset!");
static_assert(offsetof(FWeakObjectPtr, ObjectSerialNumber) == 0x000004,
              "Member 'FWeakObjectPtr::ObjectSerialNumber' has a wrong offset!");

template <typename TObjectID> class TPersistentObjectPtr {
  public:
    FWeakObjectPtr WeakPtr;
    int32_t        TagAtLastTest;
    TObjectID      ObjectID;

  public:
    class UObject* Get() const { return WeakPtr.Get(); }
    class UObject* operator->() const { return WeakPtr.Get(); }
};
struct FSoftObjectPath {
  public:
    FName   AssetPathName;
    FString SubPathString;
};
static_assert(alignof(FSoftObjectPath) == 0x000008, "Wrong alignment on FSoftObjectPath");
static_assert(sizeof(FSoftObjectPath) == 0x000018, "Wrong size on FSoftObjectPath");
static_assert(offsetof(FSoftObjectPath, AssetPathName) == 0x000000,
              "Member 'FSoftObjectPath::AssetPathName' has a wrong offset!");
static_assert(offsetof(FSoftObjectPath, SubPathString) == 0x000008,
              "Member 'FSoftObjectPath::SubPathString' has a wrong offset!");

static_assert(offsetof(TPersistentObjectPtr<FSoftObjectPath>, WeakPtr) == 0x000000,
              "Member 'TPersistentObjectPtr::WeakPtr' has a wrong offset!");
static_assert(offsetof(TPersistentObjectPtr<FSoftObjectPath>, TagAtLastTest) == 0x000008,
              "Member 'TPersistentObjectPtr::TagAtLastTest' has a wrong offset!");
// static_assert(offsetof(TPersistentObjectPtr<FSoftObjectPath>, ObjectID) == 0x00000C,
//               "Member 'TPersistentObjectPtr::ObjectID' has a wrong offset!");

class FSoftObjectPtr : public TPersistentObjectPtr<FSoftObjectPath> {};

template <typename UEType> class TSoftObjectPtr : public FSoftObjectPtr {
  public:
    UEType* Get() const { return static_cast<UEType*>(TPersistentObjectPtr::Get()); }
    UEType* operator->() const { return static_cast<UEType*>(TPersistentObjectPtr::Get()); }
};

template <typename UEType> class TSoftClassPtr : public FSoftObjectPtr {
  public:
    UEType* Get() const { return (UEType*)TPersistentObjectPtr::Get(); }
    UEType* operator->() const { return Get(); }
};

template <typename ClassType> class TSubclassOf {
    class UClass* ClassPtr;

  public:
    TSubclassOf() = default;
    inline TSubclassOf(UClass* Class) : ClassPtr(Class) {}
    inline UClass* Get() { return ClassPtr; }

    inline operator UClass*() const { return ClassPtr; }
    template <typename Target, typename = std::enable_if<std::is_base_of_v<Target, ClassType>, bool>::type>
    inline operator TSubclassOf<Target>() const {
        return ClassPtr;
    }
    inline UClass*      operator->() { return ClassPtr; }
    inline TSubclassOf& operator=(UClass* Class) {
        ClassPtr = Class;
        return *this;
    }
    inline bool operator==(const TSubclassOf& Other) const { return ClassPtr == Other.ClassPtr; }
    inline bool operator!=(const TSubclassOf& Other) const { return ClassPtr != Other.ClassPtr; }
    inline bool operator==(UClass* Other) const { return ClassPtr == Other; }
    inline bool operator!=(UClass* Other) const { return ClassPtr != Other; }
};

enum class EFunctionFlags : uint32_t {
    None = 0x00000000,

    Final = 0x00000001,
    RequiredAPI = 0x00000002,
    BlueprintAuthorityOnly = 0x00000004,
    BlueprintCosmetic = 0x00000008,
    Net = 0x00000040,
    NetReliable = 0x00000080,
    NetRequest = 0x00000100,
    Exec = 0x00000200,
    Native = 0x00000400,
    Event = 0x00000800,
    NetResponse = 0x00001000,
    Static = 0x00002000,
    NetMulticast = 0x00004000,
    UbergraphFunction = 0x00008000,
    MulticastDelegate = 0x00010000,
    Public = 0x00020000,
    Private = 0x00040000,
    Protected = 0x00080000,
    Delegate = 0x00100000,
    NetServer = 0x00200000,
    HasOutParms = 0x00400000,
    HasDefaults = 0x00800000,
    NetClient = 0x01000000,
    DLLImport = 0x02000000,
    BlueprintCallable = 0x04000000,
    BlueprintEvent = 0x08000000,
    BlueprintPure = 0x10000000,
    EditorOnly = 0x20000000,
    Const = 0x40000000,
    NetValidate = 0x80000000,

    AllFlags = 0xFFFFFFFF,
};
inline bool operator&(EFunctionFlags Left, EFunctionFlags Right) {
    using T = std::underlying_type_t<EFunctionFlags>;
    return (static_cast<T>(Left) & static_cast<T>(Right)) == static_cast<T>(Right);
}
inline constexpr SDK::EFunctionFlags operator|(SDK::EFunctionFlags Left, SDK::EFunctionFlags Right) {
    using T = std::underlying_type_t<EFunctionFlags>;
    return static_cast<EFunctionFlags>(static_cast<T>(Left) | static_cast<T>(Right));
}

enum class EClassCastFlags : uint64_t {
    None = 0x0000000000000000,

    Field = 0x0000000000000001,
    Int8Property = 0x0000000000000002,
    Enum = 0x0000000000000004,
    Struct = 0x0000000000000008,
    ScriptStruct = 0x0000000000000010,
    Class = 0x0000000000000020,
    ByteProperty = 0x0000000000000040,
    IntProperty = 0x0000000000000080,
    FloatProperty = 0x0000000000000100,
    UInt64Property = 0x0000000000000200,
    ClassProperty = 0x0000000000000400,
    UInt32Property = 0x0000000000000800,
    InterfaceProperty = 0x0000000000001000,
    NameProperty = 0x0000000000002000,
    StrProperty = 0x0000000000004000,
    Property = 0x0000000000008000,
    ObjectProperty = 0x0000000000010000,
    BoolProperty = 0x0000000000020000,
    UInt16Property = 0x0000000000040000,
    Function = 0x0000000000080000,
    StructProperty = 0x0000000000100000,
    ArrayProperty = 0x0000000000200000,
    Int64Property = 0x0000000000400000,
    DelegateProperty = 0x0000000000800000,
    NumericProperty = 0x0000000001000000,
    MulticastDelegateProperty = 0x0000000002000000,
    ObjectPropertyBase = 0x0000000004000000,
    WeakObjectProperty = 0x0000000008000000,
    LazyObjectProperty = 0x0000000010000000,
    SoftObjectProperty = 0x0000000020000000,
    TextProperty = 0x0000000040000000,
    Int16Property = 0x0000000080000000,
    DoubleProperty = 0x0000000100000000,
    SoftClassProperty = 0x0000000200000000,
    Package = 0x0000000400000000,
    Level = 0x0000000800000000,
    Actor = 0x0000001000000000,
    PlayerController = 0x0000002000000000,
    Pawn = 0x0000004000000000,
    SceneComponent = 0x0000008000000000,
    PrimitiveComponent = 0x0000010000000000,
    SkinnedMeshComponent = 0x0000020000000000,
    SkeletalMeshComponent = 0x0000040000000000,
    Blueprint = 0x0000080000000000,
    DelegateFunction = 0x0000100000000000,
    StaticMeshComponent = 0x0000200000000000,
    MapProperty = 0x0000400000000000,
    SetProperty = 0x0000800000000000,
    EnumProperty = 0x0001000000000000,
};
inline bool operator&(EClassCastFlags Left, EClassCastFlags Right) {
    using T = std::underlying_type_t<EClassCastFlags>;
    return (static_cast<T>(Left) & static_cast<T>(Right)) == static_cast<T>(Right);
}
inline constexpr EClassCastFlags operator|(EClassCastFlags Left, EClassCastFlags Right) {
    using T = std::underlying_type_t<EClassCastFlags>;
    return static_cast<EClassCastFlags>(static_cast<T>(Left) | static_cast<T>(Right));
}

struct FVector {
  public:
    float X;
    float Y;
    float Z;

  public:
    inline FVector() : X(0.f), Y(0.f), Z(0.f) {}
    inline FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {}

  public:
    FVector operator+(const FVector& Other) const { return FVector(X + Other.X, Y + Other.Y, Z + Other.Z); }
    FVector operator-(const FVector& Other) const { return FVector(X - Other.X, Y - Other.Y, Z - Other.Z); }
    FVector operator*(const FVector& Other) const { return FVector(X * Other.X, Y * Other.Y, Z * Other.Z); }
    FVector operator/(const FVector& Other) const { return FVector(X / Other.X, Y / Other.Y, Z / Other.Z); }
    FVector operator+=(const FVector& Other) {
        X += Other.X;
        Y += Other.Y;
        Z += Other.Z;
        return *this;
    }
    FVector operator-=(const FVector& Other) {
        X -= Other.X;
        Y -= Other.Y;
        Z -= Other.Z;
        return *this;
    }
    FVector operator*=(const FVector& Other) {
        X *= Other.X;
        Y *= Other.Y;
        Z *= Other.Z;
        return *this;
    }
    FVector operator/=(const FVector& Other) {
        X /= Other.X;
        Y /= Other.Y;
        Z /= Other.Z;
        return *this;
    }
    FVector operator*(float Scale) const { return FVector(X * Scale, Y * Scale, Z * Scale); }
    FVector operator/(float Scale) const {
        const float RScale = 1.f / Scale;
        return FVector(X * RScale, Y * RScale, Z * RScale);
    }
    FVector operator+=(float Scale) {
        X += Scale;
        Y += Scale;
        Z += Scale;
        return *this;
    }
    FVector operator-=(float Scale) {
        X -= Scale;
        Y -= Scale;
        Z -= Scale;
        return *this;
    }
    FVector operator*=(float Scale) {
        X *= Scale;
        Y *= Scale;
        Z *= Scale;
        return *this;
    }
    FVector operator/=(float Scale) {
        const float RScale = 1.f / Scale;
        X *= RScale;
        Y *= RScale;
        Z *= RScale;
        return *this;
    }

    bool operator==(const FVector& Other) const { return X == Other.X && Y == Other.Y && Z == Other.Z; }
    bool operator!=(const FVector& Other) const { return X != Other.X || Y != Other.Y || Z != Other.Z; }

    float   Size() const { return sqrtf(X * X + Y * Y + Z * Z); }
    float   Dot(const FVector& Other) const { return X * Other.X + Y * Other.Y + Z * Other.Z; }
    FVector Cross(const FVector& Other) const {
        return FVector(Y * Other.Z - Z * Other.Y, Z * Other.X - X * Other.Z, X * Other.Y - Y * Other.X);
    }
    float Dist(const FVector& Other) const {
        return sqrtf(powf((Other.X - X), 2.f) + powf((Other.Y - Y), 2.f) + powf((Other.Z - Z), 2.f));
    }

    bool Normalize(float Tolerance = 1.e-8f);
};

struct FVector2D {
  public:
    float X;
    float Y;

  public:
    inline constexpr FVector2D() : X(0.f), Y(0.f) {}
    inline constexpr FVector2D(float X, float Y) : X(X), Y(Y) {}

  public:
    FVector2D operator+(const FVector2D& Other) const { return FVector2D(X + Other.X, Y + Other.Y); }
    FVector2D operator-(const FVector2D& Other) const { return FVector2D(X - Other.X, Y - Other.Y); }
    FVector2D operator*(const FVector2D& Other) const { return FVector2D(X * Other.X, Y * Other.Y); }
    FVector2D operator/(const FVector2D& Other) const { return FVector2D(X / Other.X, Y / Other.Y); }
    FVector2D operator+=(const FVector2D& Other) {
        X += Other.X;
        Y += Other.Y;
        return *this;
    }
    FVector2D operator-=(const FVector2D& Other) {
        X -= Other.X;
        Y -= Other.Y;
        return *this;
    }
    FVector2D operator*=(const FVector2D& Other) {
        X *= Other.X;
        Y *= Other.Y;
        return *this;
    }
    FVector2D operator/=(const FVector2D& Other) {
        X /= Other.X;
        Y /= Other.Y;
        return *this;
    }
    FVector2D operator*(float Scale) const { return FVector2D(X * Scale, Y * Scale); }
    FVector2D operator/(float Scale) const {
        const float RScale = 1.f / Scale;
        return FVector2D(X * RScale, Y * RScale);
    }
    FVector2D operator+=(float Scale) {
        X += Scale;
        Y += Scale;
        return *this;
    }
    FVector2D operator-=(float Scale) {
        X -= Scale;
        Y -= Scale;
        return *this;
    }
    FVector2D operator*=(float Scale) {
        X *= Scale;
        Y *= Scale;
        return *this;
    }
    FVector2D operator/=(float Scale) {
        const float RScale = 1.f / Scale;
        X *= RScale;
        Y *= RScale;
        return *this;
    }

    bool operator==(const FVector2D& Other) const { return X == Other.X && Y == Other.Y; }
    bool operator!=(const FVector2D& Other) const { return X != Other.X || Y != Other.Y; }

    float Dot(const FVector2D& Other) const { return X * Other.X + Y * Other.Y; }
    float Cross(const FVector2D& Other) const { return X * Other.Y - Y * Other.X; }
    float Dist(const FVector2D& Other) const { return sqrtf(powf((Other.X - X), 2.f) + powf((Other.Y - Y), 2.f)); }
};

struct FRotator {
  public:
    float Pitch;
    float Yaw;
    float Roll;

  public:
    inline FRotator() : Pitch(0.f), Yaw(0.f), Roll(0.f) {}
    inline FRotator(float Pitch, float Yaw, float Roll) : Pitch(Pitch), Yaw(Yaw), Roll(Roll) {}

  public:
    FRotator operator+(const FRotator& Other) const {
        return FRotator(Pitch + Other.Pitch, Yaw + Other.Yaw, Roll + Other.Roll);
    }
    FRotator operator-(const FRotator& Other) const {
        return FRotator(Pitch - Other.Pitch, Yaw - Other.Yaw, Roll - Other.Roll);
    }
    FRotator operator*(const FRotator& Other) const {
        return FRotator(Pitch * Other.Pitch, Yaw * Other.Yaw, Roll * Other.Roll);
    }
    FRotator operator/(const FRotator& Other) const {
        return FRotator(Pitch / Other.Pitch, Yaw / Other.Yaw, Roll / Other.Roll);
    }
    FRotator operator+=(const FRotator& Other) {
        Pitch += Other.Pitch;
        Yaw += Other.Yaw;
        Roll += Other.Roll;
        return *this;
    }
    FRotator operator-=(const FRotator& Other) {
        Pitch -= Other.Pitch;
        Yaw -= Other.Yaw;
        Roll -= Other.Roll;
        return *this;
    }
    FRotator operator*=(const FRotator& Other) {
        Pitch *= Other.Pitch;
        Yaw *= Other.Yaw;
        Roll *= Other.Roll;
        return *this;
    }
    FRotator operator/=(const FRotator& Other) {
        Pitch /= Other.Pitch;
        Yaw /= Other.Yaw;
        Roll /= Other.Roll;
        return *this;
    }
    FRotator operator*(float Scale) const { return FRotator(Pitch * Scale, Yaw * Scale, Roll * Scale); }
    FRotator operator/(float Scale) const {
        const float RScale = 1.f / Scale;
        return FRotator(Pitch * RScale, Yaw * RScale, Roll * RScale);
    }
    FRotator operator+=(float Scale) {
        Pitch += Scale;
        Yaw += Scale;
        Roll += Scale;
        return *this;
    }
    FRotator operator-=(float Scale) {
        Pitch -= Scale;
        Yaw -= Scale;
        Roll -= Scale;
        return *this;
    }
    FRotator operator*=(float Scale) {
        Pitch *= Scale;
        Yaw *= Scale;
        Roll *= Scale;
        return *this;
    }
    FRotator operator/=(float Scale) {
        const float RScale = 1.f / Scale;
        Pitch *= RScale;
        Yaw *= RScale;
        Roll *= RScale;
        return *this;
    }

    bool operator==(const FRotator& Other) const {
        return Pitch == Other.Pitch && Yaw == Other.Yaw && Roll == Other.Roll;
    }
    bool operator!=(const FRotator& Other) const {
        return Pitch != Other.Pitch || Yaw != Other.Yaw || Roll != Other.Roll;
    }

    float Size() const { return sqrtf(Pitch * Pitch + Yaw * Yaw + Roll * Roll); }
    float Dot(const FRotator& Other) const { return Pitch * Other.Pitch + Yaw * Other.Yaw + Roll * Other.Roll; }

    FRotator Cross(const FRotator& Other) const {
        return FRotator(Yaw * Other.Roll - Roll * Other.Yaw, Roll * Other.Pitch - Pitch * Other.Roll,
                        Pitch * Other.Yaw - Yaw * Other.Pitch);
    }
    FRotator Normalize() {
        while (Yaw > 180.f)
            Yaw -= 360.f;
        while (Yaw < -180.f)
            Yaw += 360.f;

        while (Roll > 180.f)
            Roll -= 360.f;
        while (Roll < -180.f)
            Roll += 360.f;

        while (Pitch > 180.f)
            Pitch -= 360.f;
        while (Pitch < -180.f)
            Pitch += 360.f;

        return *this;
    }
};

struct FMatrix {
  public:
    float M[4][4];

  public:
    FMatrix operator*(const FMatrix& B) const {
#if CFG_AVXOPTIMISATIONS
        FMatrix Result = {};

        __m128 RightRow[4] = {};
        __m128 ResultRow[4] = {};

        for (int i = 0; i < 4; ++i) {
            RightRow[i] = _mm_loadu_ps(B.M[i]);
        }

        __m128 Zero = _mm_setzero_ps();
        for (int i = 0; i < 4; ++i) {
            ResultRow[i] = Zero;
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                __m128 a = _mm_set1_ps(this->M[i][j]);
                ResultRow[i] = _mm_add_ps(ResultRow[i], _mm_mul_ps(a, RightRow[j]));
            }
            _mm_storeu_ps(Result.M[i], ResultRow[i]);
        }

        return Result;
#else
        FMatrix Out;
        for (uint8_t r = 0; r < 4; r++) {
            for (uint8_t c = 0; c < 4; c++) {
                float Sum = 0.f;
                for (uint8_t i = 0; i < 4; i++)
                    Sum += M[r][i] * B.M[i][c];
                Out.M[r][c] = Sum;
            }
        }

        return Out;
#endif
    }
};

struct FQuat {
  public:
    float X;
    float Y;
    float Z;
    float W;

  public:
    inline FQuat() : X(0.f), Y(0.f), Z(0.f), W(0.f) {}
    inline FQuat(float X, float Y, float Z, float W) : X(X), Y(Y), Z(Z), W(W) {}
};

struct FTransform {
  public:
    FQuat   Rotation;
    FVector Translation;
    int     Pad1 = 0;
    FVector Scale;
    int     Pad2 = 0;

  public:
    FMatrix ToMatrixWithScale() const {
#if CFG_AVXOPTIMISATIONS
        FMatrix m = {};

        m.M[3][0] = Translation.X;
        m.M[3][1] = Translation.Y;
        m.M[3][2] = Translation.Z;
        m.M[3][3] = 1.0f;

        __m128 vec_X = _mm_set1_ps(Rotation.X);
        __m128 vec_Y = _mm_set1_ps(Rotation.Y);
        __m128 vec_Z = _mm_set1_ps(Rotation.Z);
        __m128 vec_W = _mm_set1_ps(Rotation.W);

        __m128 vec_XY = _mm_mul_ps(vec_X, vec_Y);
        __m128 vec_XZ = _mm_mul_ps(vec_X, vec_Z);
        __m128 vec_YZ = _mm_mul_ps(vec_Y, vec_Z);
        __m128 vec_WX = _mm_mul_ps(vec_W, vec_X);
        __m128 vec_WY = _mm_mul_ps(vec_W, vec_Y);
        __m128 vec_WZ = _mm_mul_ps(vec_W, vec_Z);

        __m128 vec_XX = _mm_mul_ps(vec_X, vec_X);
        __m128 vec_YY = _mm_mul_ps(vec_Y, vec_Y);
        __m128 vec_ZZ = _mm_mul_ps(vec_Z, vec_Z);

        float yy2_plus_zz2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_YY, vec_ZZ));
        float xx2_plus_zz2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_XX, vec_ZZ));
        float xx2_plus_yy2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_XX, vec_YY));

        m.M[0][0] = (1.0f - yy2_plus_zz2) * Scale.X;
        m.M[1][1] = (1.0f - xx2_plus_zz2) * Scale.Y;
        m.M[2][2] = (1.0f - xx2_plus_yy2) * Scale.Z;

        __m128 row0_b = _mm_mul_ps(_mm_add_ps(vec_XY, vec_WZ), _mm_set1_ps(2.0f * Scale.X));
        __m128 row0_c = _mm_mul_ps(_mm_sub_ps(vec_XZ, vec_WY), _mm_set1_ps(2.0f * Scale.X));
        m.M[0][1] = _mm_cvtss_f32(row0_b);
        m.M[0][2] = _mm_cvtss_f32(row0_c);

        __m128 row1_b = _mm_mul_ps(_mm_sub_ps(vec_XY, vec_WZ), _mm_set1_ps(2.0f * Scale.Y));
        __m128 row1_c = _mm_mul_ps(_mm_add_ps(vec_YZ, vec_WX), _mm_set1_ps(2.0f * Scale.Y));
        m.M[1][0] = _mm_cvtss_f32(row1_b);
        m.M[1][2] = _mm_cvtss_f32(row1_c);

        __m128 row2_b = _mm_mul_ps(_mm_add_ps(vec_XZ, vec_WY), _mm_set1_ps(2.0f * Scale.Z));
        __m128 row2_c = _mm_mul_ps(_mm_sub_ps(vec_YZ, vec_WX), _mm_set1_ps(2.0f * Scale.Z));
        m.M[2][0] = _mm_cvtss_f32(row2_b);
        m.M[2][1] = _mm_cvtss_f32(row2_c);

        m.M[0][3] = 0.0f;
        m.M[1][3] = 0.0f;
        m.M[2][3] = 0.0f;

        return m;
#else
        FMatrix m;
        m.M[3][0] = Translation.X;
        m.M[3][1] = Translation.Y;
        m.M[3][2] = Translation.Z;

        float x2 = Rotation.X + Rotation.X;
        float y2 = Rotation.Y + Rotation.Y;
        float z2 = Rotation.Z + Rotation.Z;

        float xx2 = Rotation.X * x2;
        float yy2 = Rotation.Y * y2;
        float zz2 = Rotation.Z * z2;
        m.M[0][0] = (1.f - (yy2 + zz2)) * Scale.X;
        m.M[1][1] = (1.f - (xx2 + zz2)) * Scale.Y;
        m.M[2][2] = (1.f - (xx2 + yy2)) * Scale.Z;

        float yz2 = Rotation.Y * z2;
        float wx2 = Rotation.W * x2;
        m.M[2][1] = (yz2 - wx2) * Scale.Z;
        m.M[1][2] = (yz2 + wx2) * Scale.Y;

        float xy2 = Rotation.X * y2;
        float wz2 = Rotation.W * z2;
        m.M[1][0] = (xy2 - wz2) * Scale.Y;
        m.M[0][1] = (xy2 + wz2) * Scale.X;

        float xz2 = Rotation.X * z2;
        float wy2 = Rotation.W * y2;
        m.M[2][0] = (xz2 + wy2) * Scale.Z;
        m.M[0][2] = (xz2 - wy2) * Scale.X;

        m.M[0][3] = 0.f;
        m.M[1][3] = 0.f;
        m.M[2][3] = 0.f;
        m.M[3][3] = 1.f;

        return m;
#endif
    }
};

struct FLinearColor {
  public:
    float R;
    float G;
    float B;
    float A;

  public:
    inline constexpr FLinearColor() : R(0), G(0), B(0), A(0) {}
    inline constexpr FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {}

  public:
    bool operator==(const FLinearColor& Other) const {
        return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
    }
    bool operator!=(const FLinearColor& Other) const { return !(*this == Other); }

  public:
    std::string ToStr() const {
        return std::to_string((int)(R * 255.f)) + "/" + std::to_string((int)(G * 255.f)) + "/" +
               std::to_string((int)(B * 255.f)) + "/" + std::to_string((int)(A * 255.f));
    }

  public:
    static const FLinearColor White;
    static const FLinearColor Gray;
    static const FLinearColor Black;
    static const FLinearColor Red;
    static const FLinearColor Green;
    static const FLinearColor Blue;
};

struct FColor {
  public:
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t A;
};

} // namespace SDK
