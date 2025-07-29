#include "CoreUObject.h"
#include "sdk.h"

#include <format>

#include <utils/log.h>
#include <utils/error.h>

namespace SDK {

void UObject::ProcessEvent(class UFunction* Function, void* Parms) const {
    void (*PE)(const UObject* Object, class UFunction* Function, void* Parms) = (decltype(PE))VTable[ProcessEvent_Idx];
    PE(this, Function, Parms);
}

bool UObject::IsA(class UClass* Clss) const {
    for (UStruct* Super = Class; Super; Super = Super->SuperStruct) {
        if (Super == Clss)
            return true;
    }

    return false;
}
bool UObject::IsDefaultObject() const {
    return (Flags & 0x10) == 0x10;
}

bool UObject::HasTypeFlag(EClassCastFlags TypeFlag) const {
    return TypeFlag != EClassCastFlags::None ? Class->ClassCastFlags & TypeFlag : true;
}

std::string UObject::GetName() const {
    return Name.ToString();
}
std::wstring UObject::GetWName() const {
    return Name.ToWString();
}
std::string UObject::GetFullName() const {
    if (Class) {
        std::string Temp;
        for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer) {
            Temp = NextOuter->GetName() + "." + Temp;
        }

        return Class->GetName() + " " + Temp + GetName();
    }

    return "None";
}
std::wstring UObject::GetFullWName() const {
    if (Class) {
        std::wstring Temp;
        for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer) {
            Temp = NextOuter->GetWName();
        }

        return Class->GetWName() + L" " + Temp + GetWName();
    }

    return L"None";
}

PropertyInfo UObject::GetPropertyInfo(const FName& ClassName, const FName& PropertyName, bool SuppressFailure) {
    const PropertyInfo Info = FindObjectFast<UStruct>(ClassName, EClassCastFlags::Struct)->FindProperty(PropertyName);

    if (!SuppressFailure && !Info.Found)
        Error::ThrowError(std::format("Failed to find property.\nClass: {}\nProperty: {}", ClassName.ToString(),
                                      PropertyName.ToString()));

    return Info;
}
UFunction* UObject::GetFunction(const FName& ClassName, const FName& FunctionName, bool SuppressFailure) {
    UFunction* const Function = FindObjectFast<UStruct>(ClassName, EClassCastFlags::Struct)->FindFunction(FunctionName);

    if (!SuppressFailure && !Function)
        Error::ThrowError(std::format("Failed to find function.\nClass: {}\nFunction: {}", ClassName.ToString(),
                                      FunctionName.ToString()));

    return Function;
}

PropertyInfo UStruct::FindProperty(const FName& Name) const {
    PropertyInfo Result = {.Found = false};

    if (SDK::g_EngineVersion < 4.25) {
        for (UField* Child = Children; Child; Child = Child->Next) {
            if (!Child->HasTypeFlag(EClassCastFlags::Property))
                continue;

            UProperty* Property = static_cast<UProperty*>(Child);
            if (Property->Name == Name) {
                Result.Found = true;
                Result.Offset = Property->Offset_Internal;
                Result.Prop = Property;

                if (Property->HasTypeFlag(EClassCastFlags::BoolProperty)) {
                    UBoolProperty* BoolProperty = static_cast<UBoolProperty*>(Property);
                    if (!BoolProperty->IsNativeBool())
                        Result.ByteMask = BoolProperty->ByteMask;
                }

                return Result;
            }
        }
    } else {
        for (FField* ChildProperty = ChildProperties; ChildProperty && ChildProperty->Class;
             ChildProperty = ChildProperty->Next) {
            if (ChildProperty->HasTypeFlag(EClassCastFlags::Property) && ChildProperty->Name == Name) {
                FProperty* Property = static_cast<FProperty*>(ChildProperty);
                Result.Found = true;
                Result.Offset = Property->Offset;
                Result.FProp = Property;

                if (ChildProperty->HasTypeFlag(EClassCastFlags::BoolProperty)) {
                    FBoolProperty* BoolProperty = static_cast<FBoolProperty*>(ChildProperty);
                    if (BoolProperty->ByteMask != 0xFF)
                        Result.ByteMask = BoolProperty->ByteMask;
                }

                return Result;
            }
        }
    }

    return Result;
}
UFunction* UStruct::FindFunction(const FName& Name) const {
    for (UField* Child = Children; Child; Child = Child->Next) {
        if (!Child->HasTypeFlag(EClassCastFlags::Function))
            continue;

        UFunction* Function = reinterpret_cast<UFunction*>(Child);
        if (Function->Name == Name)
            return Function;
    }

    return nullptr;
}

bool UBoolProperty::IsNativeBool() const {
    return ByteMask == 0xFF;
}

} // namespace SDK
