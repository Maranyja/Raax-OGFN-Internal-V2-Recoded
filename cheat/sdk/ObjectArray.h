#pragma once
#include <cstdint>
#include <stddef.h>

namespace SDK {

struct FUObjectItem {
    class UObject* Object;
    uint8_t        Pad_0[0x10];
};
static_assert(alignof(FUObjectItem) == 0x000008, "Wrong alignment on FUObjectItem");
static_assert(sizeof(FUObjectItem) == 0x000018, "Wrong size on FUObjectItem");
static_assert(offsetof(FUObjectItem, Object) == 0x000000, "Member 'FUObjectItem::Object' has a wrong offset!");

class Chunked_TUObjectArray {
  public:
    enum {
        ElementsPerChunk = 0x10000,
    };

  public:
    static inline auto DecryptPtr = [](void* ObjPtr) -> uint8_t* { return reinterpret_cast<uint8_t*>(ObjPtr); };

    FUObjectItem** Objects;
    uint8_t        Pad_0[0x08];
    int32_t        MaxElements;
    int32_t        NumElements;
    int32_t        MaxChunks;
    int32_t        NumChunks;

  public:
    inline int32_t        Max() const { return MaxElements; }
    inline int32_t        Num() const { return NumElements; }
    inline FUObjectItem** GetDecrytedObjPtr() const { return reinterpret_cast<FUObjectItem**>(DecryptPtr(Objects)); }
    inline class UObject* GetByIndex(const int32_t Index) const {
        if (Index < 0 || Index > NumElements)
            return nullptr;

        const int32_t ChunkIndex = Index / ElementsPerChunk;
        const int32_t InChunkIdx = Index % ElementsPerChunk;

        return GetDecrytedObjPtr()[ChunkIndex][InChunkIdx].Object;
    }
};
static_assert(alignof(Chunked_TUObjectArray) == 0x000008, "Wrong alignment on Chunked_TUObjectArray");
static_assert(sizeof(Chunked_TUObjectArray) == 0x000020, "Wrong size on Chunked_TUObjectArray");
static_assert(offsetof(Chunked_TUObjectArray, Objects) == 0x000000,
              "Member 'Chunked_TUObjectArray::Objects' has a wrong offset!");
static_assert(offsetof(Chunked_TUObjectArray, MaxElements) == 0x000010,
              "Member 'Chunked_TUObjectArray::MaxElements' has a wrong offset!");
static_assert(offsetof(Chunked_TUObjectArray, NumElements) == 0x000014,
              "Member 'Chunked_TUObjectArray::NumElements' has a wrong offset!");
static_assert(offsetof(Chunked_TUObjectArray, MaxChunks) == 0x000018,
              "Member 'Chunked_TUObjectArray::MaxChunks' has a wrong offset!");
static_assert(offsetof(Chunked_TUObjectArray, NumChunks) == 0x00001C,
              "Member 'Chunked_TUObjectArray::NumChunks' has a wrong offset!");

class Fixed_TUObjectArray {
  public:
    static inline auto DecryptPtr = [](void* ObjPtr) -> uint8_t* { return reinterpret_cast<uint8_t*>(ObjPtr); };

  public:
    FUObjectItem* Objects;
    int32_t       MaxElements;
    int32_t       NumElements;

  public:
    inline int32_t        Max() const { return MaxElements; }
    inline int32_t        Num() const { return NumElements; }
    inline FUObjectItem*  GetDecrytedObjPtr() const { return reinterpret_cast<FUObjectItem*>(DecryptPtr(Objects)); }
    inline class UObject* GetByIndex(const int32_t Index) const {
        if (Index < 0 || Index > NumElements)
            return nullptr;

        return GetDecrytedObjPtr()[Index].Object;
    }
};
static_assert(alignof(Fixed_TUObjectArray) == 0x000008, "Wrong alignment on Fixed_TUObjectArray");
static_assert(sizeof(Fixed_TUObjectArray) == 0x000010, "Wrong size on Fixed_TUObjectArray");
static_assert(offsetof(Fixed_TUObjectArray, Objects) == 0x000000,
              "Member 'Fixed_TUObjectArray::Objects' has a wrong offset!");
static_assert(offsetof(Fixed_TUObjectArray, MaxElements) == 0x000008,
              "Member 'Fixed_TUObjectArray::MaxElements' has a wrong offset!");
static_assert(offsetof(Fixed_TUObjectArray, NumElements) == 0x00000C,
              "Member 'Fixed_TUObjectArray::NumElements' has a wrong offset!");

class TUObjectArray {
  private:
    void* m_ObjectArray;
    bool  m_IsChunked;

  public:
    TUObjectArray() : m_ObjectArray(nullptr), m_IsChunked(false) {}

  public:
    inline void Initialize(void* Address, bool IsChunked) {
        m_ObjectArray = Address;
        m_IsChunked = IsChunked;
    }
    inline int32_t Max() const {
        return m_IsChunked ? static_cast<Chunked_TUObjectArray*>(m_ObjectArray)->Max()
                           : static_cast<Fixed_TUObjectArray*>(m_ObjectArray)->Max();
    }
    inline int32_t Num() const {
        return m_IsChunked ? static_cast<Chunked_TUObjectArray*>(m_ObjectArray)->Num()
                           : static_cast<Fixed_TUObjectArray*>(m_ObjectArray)->Num();
    }
    inline class UObject* GetByIndex(int32_t Index) const {
        return m_IsChunked ? static_cast<Chunked_TUObjectArray*>(m_ObjectArray)->GetByIndex(Index)
                           : static_cast<Fixed_TUObjectArray*>(m_ObjectArray)->GetByIndex(Index);
    }
};

} // namespace SDK
