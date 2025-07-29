#pragma once
#include "Basic.h"

namespace SDK {

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FFieldClass {
  public:
    FName           Name;
    uint64_t        Id;
    EClassCastFlags CastFlags;
    int32_t         ClassFlags;
    uint8_t         Pad_74C3[0x4];
    FFieldClass*    SuperClass;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FField {
  public:
    void*        Vft;
    FFieldClass* Class;
    char         Owner[0x10];
    FField*      Next;
    FName        Name;
    int32_t      Flags;

    bool HasTypeFlag(EClassCastFlags TypeFlag) const {
        return TypeFlag != EClassCastFlags::None ? Class->CastFlags & TypeFlag : true;
    }
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FProperty : public FField {
  public:
    uint8_t  Pad_74C4[0x8];
    int32_t  ElementSize;
    uint64_t PropertyFlags;
    uint8_t  Pad_74C5[0x4];
    int32_t  Offset;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FBoolProperty : public FProperty {
  public:
    uint8_t Pad_74C7[0x28];
    uint8_t FieldSize;
    uint8_t ByteOffset;
    uint8_t ByteMask;
    uint8_t FieldMask;
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

} // namespace SDK
