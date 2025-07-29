#pragma once
#include "FMemory.h"

#include <cstdint>
#include <string>

#include <utils/string.h>

namespace SDK {

template <typename ElementType> class TArray {
  protected:
    static constexpr uint64_t ElementAlign = alignof(ElementType);
    static constexpr uint64_t ElementSize = sizeof(ElementType);

  protected:
    ElementType* Data;
    int32_t      NumElements;
    int32_t      MaxElements;

  public:
    TArray() : Data(nullptr), NumElements(0), MaxElements(0) {}
    TArray(int32_t Size)
        : Data(static_cast<ElementType*>(FMemory::Malloc(Size * ElementSize, ElementAlign))), NumElements(0),
          MaxElements(Size) {}

    TArray(const TArray& Other) : Data(nullptr), NumElements(0), MaxElements(0) { CopyFrom(Other); }
    TArray(TArray&& Other) noexcept : Data(Other.Data), NumElements(Other.NumElements), MaxElements(Other.MaxElements) {
        Other.Data = nullptr;
        Other.NumElements = 0;
        Other.MaxElements = 0;
    }
    ~TArray() { Free(); }

  public:
    TArray& operator=(const TArray& Other) {
        if (this != &Other) {
            CopyFrom(Other);
        }
        return *this;
    }
    TArray& operator=(TArray&& Other) noexcept {
        if (this != &Other) {
            Free();
            Data = Other.Data;
            NumElements = Other.NumElements;
            MaxElements = Other.MaxElements;
            Other.Data = nullptr;
            Other.NumElements = 0;
            Other.MaxElements = 0;
        }
        return *this;
    }

  public:
    inline bool IsValid() const { return Data != nullptr; }
    inline bool IsValidIndex(int32_t Index) const { return Data && Index >= 0 && Index < NumElements; }

    inline int32_t      Num() const { return NumElements; }
    inline int32_t      Max() const { return MaxElements; }
    inline int32_t      GetSlack() const { return MaxElements - NumElements; }
    inline ElementType* GetData() { return Data; }
    inline ElementType  GetByIndex(int32_t Idx) { return Data[Idx]; }

  public:
    inline bool Add(const ElementType& Element) {
        if (GetSlack() <= 0) {
            const int32_t NewCapacity = (MaxElements == 0) ? 8 : MaxElements * 2;

            ElementType* NewData = static_cast<ElementType*>(FMemory::Malloc(NewCapacity * ElementSize, ElementAlign));
            if (!NewData)
                return false;

            if (Data) {
                memcpy(NewData, Data, NumElements * ElementSize);
                FMemory::Free(Data);
            }

            Data = NewData;
            MaxElements = NewCapacity;
        }

        Data[NumElements] = Element;
        NumElements++;
        return true;
    }
    inline bool Remove(int32_t Index) {
        if (!IsValidIndex(Index))
            return false;

        NumElements--;

        for (int i = Index; i < NumElements; i++) {
            /* NumElements was decremented, acessing i + 1 is safe */
            Data[i] = Data[i + 1];
        }

        return true;
    }
    inline void Clear() { NumElements = 0; }

  public:
    inline void CopyFrom(const TArray& Other) {
        if (this == &Other || Other.NumElements == 0)
            return;

        if (!Data || MaxElements < Other.NumElements) {
            Free();
            Data = static_cast<ElementType*>(FMemory::Malloc(Other.NumElements * ElementSize, ElementAlign));
            MaxElements = Other.NumElements;
        }

        NumElements = Other.NumElements;
        memcpy(Data, Other.Data, Other.NumElements * ElementSize);
    }
    inline void Free() noexcept {
        if (Data)
            FMemory::Free(Data);

        NumElements = 0;
        MaxElements = 0;
    }

  public:
    inline ElementType&       operator[](int32_t Index) { return Data[Index]; }
    inline const ElementType& operator[](int32_t Index) const { return Data[Index]; }
};

class FString : public TArray<wchar_t> {
  public:
    FString() : TArray<wchar_t>() {}
    FString(const wchar_t* Str) {
        if (!Str)
            return;

        const uint32_t Length = static_cast<uint32_t>(wcslen(Str));
        const uint32_t NullTerminatedLength = Length + 1;

        Data = static_cast<wchar_t*>(FMemory::Malloc(NullTerminatedLength * sizeof(wchar_t), 0));
        NumElements = NullTerminatedLength;
        MaxElements = NullTerminatedLength;

        memcpy(Data, Str, Length * sizeof(wchar_t));
        Data[Length] = L'\0';
    }

    FString(const FString& Other) : TArray<wchar_t>(Other) {}
    FString(FString&& Other) noexcept : TArray<wchar_t>(std::move(Other)) {}

    FString& operator=(FString&& Other) noexcept {
        TArray<wchar_t>::operator=(std::move(Other));
        return *this;
    }
    FString& operator=(const FString& Other) {
        TArray<wchar_t>::operator=(Other);
        return *this;
    }

  public:
    inline std::wstring ToWString() const { return IsValid() ? std::wstring(Data, NumElements - 1) : L""; }
    inline std::string  ToString() const { return String::WideToNarrow(ToWString()); }

  public:
    inline wchar_t*       CStr() { return Data; }
    inline const wchar_t* CStr() const { return Data; }
};

} // namespace SDK
