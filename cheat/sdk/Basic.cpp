#include "Basic.h"
#include "Engine.h"
#include "Containers.h"
#include "sdk.h"

#include <utils/math.h>

namespace SDK {

class UObject* FWeakObjectPtr::Get() const {
    return UObject::Objects.GetByIndex(ObjectIndex);
}
class UObject* FWeakObjectPtr::operator->() const {
    return UObject::Objects.GetByIndex(ObjectIndex);
}
bool FWeakObjectPtr::operator==(const FWeakObjectPtr& Other) const {
    return ObjectIndex == Other.ObjectIndex;
}
bool FWeakObjectPtr::operator!=(const FWeakObjectPtr& Other) const {
    return ObjectIndex != Other.ObjectIndex;
}
bool FWeakObjectPtr::operator==(const class UObject* Other) const {
    return ObjectIndex == Other->Index;
}
bool FWeakObjectPtr::operator!=(const class UObject* Other) const {
    return ObjectIndex != Other->Index;
}

FName::FName(const wchar_t* Name) : ComparisonIndex(0), Number(0) {
    FNameConstructorW(this, Name, FNAME_Add);
}
FName::FName(const char* Name) : ComparisonIndex(0), Number(0) {
    FNameConstructorW(this, String::NarrowToWide(Name).c_str(), FNAME_Add);
}

FString FName::ToFString() const {
    FString Out;
    FNameToString(this, &Out);
    return Out;
}
std::wstring FName::ToWString() const {
    FString FStr = ToFString();
    return FStr.ToWString();
}
std::string FName::ToString() const {
    FString FStr = ToFString();
    return FStr.ToString();
}

FString& FText::Get() const {
    uint32_t Offset = g_EngineVersion >= 5.f ? 0x30 : 0x28;
    FString& Text = *(FString*)((uint64_t)TextData + Offset);
    return Text;
}
std::string FText::ToString() const {
    if (!TextData)
        return "";

    FString& Text = Get();
    if (Text.IsValid()) {
        return Text.ToString();
    }

    return "";
}

bool FName::operator==(const FName& Other) const {
    return ComparisonIndex == Other.ComparisonIndex;
}
bool FName::operator!=(const FName& Other) const {
    return !operator==(Other);
}

void FName::operator=(const FName& Other) {
    ComparisonIndex = Other.ComparisonIndex;
    Number = Other.Number;
}
void FName::operator=(const wchar_t* Name) {
    *this = FName(Name);
}
void FName::operator=(const char* Name) {
    *this = FName(Name);
}

bool FVector::Normalize(float Tolerance) {
    const float SquareSum = X * X + Y * Y + Z * Z;
    if (SquareSum > Tolerance) {
        const float Scale = Math::InvSqrt(SquareSum);
        X *= Scale;
        Y *= Scale;
        Z *= Scale;
        return true;
    }
    return false;
}

const FLinearColor FLinearColor::White = FLinearColor(1.f, 1.f, 1.f, 1.f);
const FLinearColor FLinearColor::Gray = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
const FLinearColor FLinearColor::Black = FLinearColor(0.f, 0.f, 0.f, 1.f);
const FLinearColor FLinearColor::Red = FLinearColor(1.f, 0.f, 0.f, 1.f);
const FLinearColor FLinearColor::Green = FLinearColor(0.f, 1.f, 0.f, 1.f);
const FLinearColor FLinearColor::Blue = FLinearColor(0.f, 0.f, 1.f, 1.f);

} // namespace SDK
