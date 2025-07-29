#pragma once
#include "Basic.h"
#include "ObjectArray.h"
#include "FProperty.h"

#include <string>

/* @brief Sets up StaticClass() and GetDefaultObj() functions. */
#define STATICCLASS_DEFAULTOBJECT(ClassNameStr, Type)                                                                  \
    static UClass* StaticClass() {                                                                                     \
        static UClass* Clss = nullptr;                                                                                 \
        if (!Clss)                                                                                                     \
            Clss = UObject::FindObjectFast<UClass>(ClassNameStr, SDK::EClassCastFlags::Class);                         \
        return Clss;                                                                                                   \
    }                                                                                                                  \
    static Type* GetDefaultObj() {                                                                                     \
        static Type* Default = nullptr;                                                                                \
        if (!Default)                                                                                                  \
            Default = static_cast<Type*>(StaticClass()->ClassDefaultObject);                                           \
        return Default;                                                                                                \
    }                                                                                                                  \
    static constexpr const char* ClassName = ClassNameStr

namespace SDK {

/* @brief Struct used for finding properties. */
struct PropertyInfo {
    bool    Found = false;
    int32_t Offset = 0;
    uint8_t ByteMask = 0;
    union {
        class UProperty* Prop;
        class FProperty* FProp;
    };
};

#define UPROPERTY(Type, Name)                                                                                          \
    static const PropertyInfo& getpropinfo_##Name(bool SuppressFailure = false) {                                      \
        static PropertyInfo Prop =                                                                                     \
            GetPropertyInfo(ClassName /* assumes STATICCLASS_DEFAULTOBJECT is used*/, #Name, SuppressFailure);         \
        return Prop;                                                                                                   \
    }                                                                                                                  \
    inline void putprop_##Name(const Type& v) {                                                                        \
        const PropertyInfo& Prop = getpropinfo_##Name();                                                               \
        *reinterpret_cast<Type*>((uint8_t*)this + Prop.Offset) = const_cast<Type&>(v);                                 \
    }                                                                                                                  \
    inline Type& getprop_##Name() const {                                                                              \
        const PropertyInfo& Prop = getpropinfo_##Name();                                                               \
        return *reinterpret_cast<Type*>((uint8_t*)this + Prop.Offset);                                                 \
    }                                                                                                                  \
    __declspec(property(get = getprop_##Name, put = putprop_##Name)) Type Name

#define UPROPERTY_BITFIELD(Name)                                                                                       \
    static const PropertyInfo& getpropinfo_##Name(bool SuppressFailure = false) {                                      \
        static PropertyInfo Prop =                                                                                     \
            GetPropertyInfo(ClassName /* assumes STATICCLASS_DEFAULTOBJECT is used*/, #Name, SuppressFailure);         \
        return Prop;                                                                                                   \
    }                                                                                                                  \
    inline uint8_t* getbitfieldprop_##Name() const {                                                                   \
        const PropertyInfo& Prop = getpropinfo_##Name();                                                               \
        return reinterpret_cast<uint8_t*>((uint8_t*)this + Prop.Offset);                                               \
    }                                                                                                                  \
    inline void putprop_##Name(const bool& v) {                                                                        \
        const PropertyInfo& Prop = getpropinfo_##Name();                                                               \
        if (Prop.ByteMask) {                                                                                           \
            auto& ByteValue = *reinterpret_cast<uint8_t*>((uint8_t*)this + Prop.Offset);                               \
            ByteValue &= ~Prop.ByteMask;                                                                               \
            if (v) {                                                                                                   \
                ByteValue |= Prop.ByteMask;                                                                            \
            }                                                                                                          \
            return;                                                                                                    \
        }                                                                                                              \
        *reinterpret_cast<bool*>((uint8_t*)this + Prop.Offset) = const_cast<bool&>(v);                                 \
    }                                                                                                                  \
    inline bool getprop_##Name() const {                                                                               \
        const PropertyInfo& Prop = getpropinfo_##Name();                                                               \
        auto                Value = *reinterpret_cast<uint8_t*>((uint8_t*)this + Prop.Offset);                         \
        if (Prop.ByteMask)                                                                                             \
            return Value & Prop.ByteMask;                                                                              \
        return Value;                                                                                                  \
    }                                                                                                                  \
    __declspec(property(get = getprop_##Name, put = putprop_##Name)) bool Name

#define UPROPERTY_OFFSET(Type, Name, Offset)                                                                           \
    inline void putprop_##Name(const Type& v) {                                                                        \
        *reinterpret_cast<Type*>((uint8_t*)this + Offset) = const_cast<Type&>(v);                                      \
    }                                                                                                                  \
    inline Type& getprop_##Name() const {                                                                              \
        return *reinterpret_cast<Type*>((uint8_t*)this + Offset);                                                      \
    }                                                                                                                  \
    __declspec(property(get = getprop_##Name, put = putprop_##Name)) Type Name

class UObject {
  public:
    static inline TUObjectArray Objects;

  public:
    static inline int ProcessEvent_Idx;

  public:
    void**        VTable;
    int32_t       Flags;
    int32_t       Index;
    class UClass* Class;
    class FName   Name;
    UObject*      Outer;

  public:
    void ProcessEvent(class UFunction* Function, void* Parms) const;

    bool IsA(class UClass* Class) const;
    bool IsDefaultObject() const;

    bool HasTypeFlag(EClassCastFlags TypeFlag) const;

    std::string  GetName() const;
    std::wstring GetWName() const;
    std::string  GetFullName() const;
    std::wstring GetFullWName() const;

  public:
    template <typename UEType = UObject>
    static UEType* FindObject(const std::string& FullName, EClassCastFlags RequiredType = EClassCastFlags::None) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object)
                continue;

            if (Object->HasTypeFlag(RequiredType) && Object->GetFullName() == FullName)
                return static_cast<UEType*>(Object);
        }

        return nullptr;
    }

    template <typename UEType = UObject>
    static UEType* FindObjectFast(const FName& Name, EClassCastFlags RequiredType = EClassCastFlags::None) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object)
                continue;

            if (Object->HasTypeFlag(RequiredType) && Object->Name == Name)
                return static_cast<UEType*>(Object);
        }

        return nullptr;
    }

    template <typename UEType = UObject>
    static UEType* FindObjectFastWithOuter(const FName& Name, const FName& OuterName) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object || !Object->Outer)
                continue;

            if (Object->Name == Name && Object->Outer->Name == OuterName)
                return reinterpret_cast<UEType*>(Object);
        }

        return nullptr;
    }

    static PropertyInfo GetPropertyInfo(const FName& ClassName, const FName& PropertyName,
                                        bool SuppressFailure = false);
    static UFunction*   GetFunction(const FName& ClassName, const FName& FunctionName, bool SuppressFailure = false);
};
static_assert(alignof(UObject) == 0x000008, "Wrong alignment on UObject");
static_assert(sizeof(UObject) == 0x000028, "Wrong size on UObject");
static_assert(offsetof(UObject, VTable) == 0x000000, "Member 'UObject::VTable' has a wrong offset!");
static_assert(offsetof(UObject, Flags) == 0x000008, "Member 'UObject::Flags' has a wrong offset!");
static_assert(offsetof(UObject, Index) == 0x00000C, "Member 'UObject::Index' has a wrong offset!");
static_assert(offsetof(UObject, Class) == 0x000010, "Member 'UObject::Class' has a wrong offset!");
static_assert(offsetof(UObject, Name) == 0x000018, "Member 'UObject::Name' has a wrong offset!");
static_assert(offsetof(UObject, Outer) == 0x000020, "Member 'UObject::Outer' has a wrong offset!");

class UStruct : public UObject {
  public:
    static inline uint32_t SuperStruct_Offset;
    static inline uint32_t Children_Offset;
    static inline uint32_t ChildProperties_Offset;
    static inline uint32_t PropertiesSize_Offset;
    static inline uint32_t MinAlignment_Offset;

  public:
    UPROPERTY_OFFSET(class UStruct*, SuperStruct, SuperStruct_Offset);
    UPROPERTY_OFFSET(class UField*, Children, Children_Offset);
    UPROPERTY_OFFSET(class FField*, ChildProperties, ChildProperties_Offset);
    UPROPERTY_OFFSET(int32_t, PropertiesSize, PropertiesSize_Offset);
    UPROPERTY_OFFSET(int32_t, MinAlignment, MinAlignment_Offset);

  public:
    PropertyInfo     FindProperty(const FName& Name) const;
    class UFunction* FindFunction(const FName& Name) const;
};

class UClass : public UStruct {
  public:
    static inline uint32_t ClassCastFlags_Offset;
    static inline uint32_t ClassDefaultObject_Offset;

  public:
    UPROPERTY_OFFSET(EClassCastFlags, ClassCastFlags, ClassCastFlags_Offset);
    UPROPERTY_OFFSET(class UObject*, ClassDefaultObject, ClassDefaultObject_Offset);
};

class UField : public UObject {
  public:
    static inline uint32_t Next_Offset;

  public:
    UPROPERTY_OFFSET(class UField*, Next, Next_Offset);
};

class UProperty : public UField {
  public:
    static inline uint32_t Offset_Internal_Offset;

  public:
    UPROPERTY_OFFSET(int32_t, Offset_Internal, Offset_Internal_Offset);
};

class UBoolProperty : public UProperty {
  public:
    static inline uint32_t ByteMask_Offset;

  public:
    UPROPERTY_OFFSET(uint8_t, ByteMask, ByteMask_Offset);

  public:
    bool IsNativeBool() const;
};

class UFunction : public UField {
  public:
    static inline uint32_t FunctionFlags_Offset;
    static inline uint32_t FuncPtr_Offset;

  public:
    UPROPERTY_OFFSET(EFunctionFlags, FunctionFlags, FunctionFlags_Offset);
    UPROPERTY_OFFSET(void*, FuncPtr, FuncPtr_Offset);
};

template <typename T, bool ForceCast = false> inline T* Cast(UObject* Object) {
    if (ForceCast || Object && Object->IsA(T::StaticClass()))
        return (T*)Object;
    return nullptr;
}

} // namespace SDK
