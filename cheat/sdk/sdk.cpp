#include "sdk.h"
#include <sstream>

#include <cheat/core.h>
#include <utils/error.h>
#include <utils/memory.h>
#include <utils/log.h>
#include <utils/math.h>

namespace SDK {

// --- Unreal-Engine general offsets ---------------------------------

static bool SetupGObjects() {
    // Two standard patterns for finding chunked and non-chunked GObjects.
    static const std::vector<std::pair<const std::string, const std::string>> Patterns = {
        {"Chunked", "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1"}, {"Fixed", "48 8B 05 ? ? ? ? 48 8D 14 C8 EB 02"}};

    for (const auto& [Type, Pattern] : Patterns) {
        uintptr_t Address = Memory::PatternScan<uint32_t>(Pattern.c_str(), 3, true);
        if (Memory::IsAddressInsideImage(Address)) {
            LOG(LOG_INFO, "Found GObjects (%s) offset: 0x%p", Type.c_str(),
                reinterpret_cast<void*>(Address - Memory::GetImageBase()));
            UObject::Objects.Initialize(reinterpret_cast<void*>(Address), Type == "Chunked");
            return true;
        }
    }

    Error::ThrowError("Failed to find GObjects!");
    return false;
}
static bool SetupFMemoryRealloc() {
    // This signature has been very reliable from what I have tested.
    // Since Realloc can be used as Alloc and Free aswell, it is easier to only get Realloc.
    /*
    48 89 5C 24 08	mov     [rsp+arg_0], rbx
    48 89 74 24 10	mov     [rsp+arg_8], rsi
    57				push    rdi
    48 83 EC 20		sub     rsp, 20h
    48 8B F1		mov     rsi, rcx
    41 8B D8		mov     ebx, r8d
    48 8B 0D 64		mov     rcx, cs:qword_6024FE0
    5A 10 04
    48 8B FA		mov     rdi, rdx
    48 85 C9		test    rcx, rcx
    */
    uintptr_t FMemoryRealloc = Memory::PatternScan(
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F1 41 8B D8 48 8B 0D ? ? ? ? 48 8B FA 48 85 C9");
    if (FMemoryRealloc) {
        FMemory::FMemoryRealloc = reinterpret_cast<decltype(FMemory::FMemoryRealloc)>(FMemoryRealloc);
        LOG(LOG_INFO, "Found FMemoryRealloc offset: 0x%p",
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FMemory::FMemoryRealloc) - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find FMemoryRealloc!");
    return false;
}
static bool SetupFNameToString() {
    // Two standard patterns for finding FNameToString across different game versions.
    static const std::vector<std::pair<const std::wstring, const std::string>> Patterns = {
        {L"TSoftObjectPtr<%s%s>", "E8 ? ? ? ? 48"},
        {L"Error uncompressing data.  Format: %f, Filename: %s", "E8 ? ? ? ? EB"}};

    for (const auto& [String, Pattern] : Patterns) {
        uintptr_t StringRef = Memory::FindStringRef(String.c_str());
        if (StringRef) {
            uintptr_t FNameToString =
                Memory::PatternScanRange<int32_t>(StringRef, 0x50, Pattern.c_str(), true, 1, true);
            if (FNameToString) {
                FName::FNameToString = reinterpret_cast<decltype(FName::FNameToString)>(FNameToString);
                LOG(LOG_INFO, "Found FNameToString offset: 0x%p",
                    reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FName::FNameToString) -
                                            Memory::GetImageBase()));
                return true;
            }
        }
    }

    Error::ThrowError("Failed to find FNameToString!");
    return false;
}
static bool SetupFNameConstructorW() {
    uintptr_t StringRef = Memory::FindStringRef(L"CanvasObject");
    if (!StringRef) {
        Error::ThrowError("Failed to find FNameConstructorW: \"CanvasObject\" string");
        return false;
    }

    uintptr_t FNameConstructorW = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", false, 1, true);
    if (!FNameConstructorW) {
        Error::ThrowError("Failed to find FNameConstructorW: Call FNameConstructorW instruction");
        return false;
    }

    FName::FNameConstructorW = reinterpret_cast<decltype(FName::FNameConstructorW)>(FNameConstructorW);
    LOG(LOG_INFO, "Found FNameConstructorW offset: 0x%p",
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FName::FNameConstructorW) - Memory::GetImageBase()));
    return true;
}
static bool SetupUKismetSystemLibraryLineTraceSingle() {
    // Two patterns for LineTraceSingle. One supports older UE4 versions, and the other supports later UE4 versions and
    // some UE5 versions.
    static const std::vector<std::pair<size_t, const std::string>> Patterns = {
        {151, "48 8B 43 20 48 8D 4D E0 0F 10 45 C0 44 0F B6 8D ? ? ? ? 48 85 C0 0F 10 4D D0 4C 8D 45 A0 40 0F 95 C7 66 "
              "0F 7F 45 ? 48 8D 55 B0 F2 0F 10 45 ? 48 03 F8 8B 45 88 F2 0F 11 45 ? F2 0F 10 45 ? 89 45 A8 8B 45 98 F2 "
              "0F 11 45 ? F3 0F 10 44 24 ? F3 0F 11 44 24 ? 48 89 4C 24 ? 48 8D 4D F0 48 89 4C 24 ? 48 8B 4C 24 ? 44 "
              "88 7C 24 ? 48 89 74 24 ? 89 45 B8 0F B6 85 ? ? ? ? 89 44 24 30 4C 89 74 24 ? 44 88 64 24 ? 48 89 7B 20 "
              "66 0F 7F 4D ? E8"},
        {88, "4C 39 6B 20 4C 8D 45 A0 F2 0F 10 45 ? 48 8D 55 B0 44 0F B6 8D ? ? ? ? 49 8B C5 48 8B 4C 24 ? 0F 95 C0 48 "
             "01 43 20 8B 45 88 44 88 74 24 ? F2 0F 11 45 ? F2 0F 10 45 ? 89 45 A8 8B 45 98 48 89 7C 24 ? 48 89 74 24 "
             "? 89 45 B8 F2 0F 11 45 ? 44 88 7C 24 ? E8"}};

    for (const auto& [Offset, Pattern] : Patterns) {
        uintptr_t Address = Memory::PatternScan<int32_t>(Pattern.c_str(), Offset, true);
        if (Memory::IsAddressInsideImage(Address)) {
            UKismetSystemLibrary::KismetSystemLibraryLineTraceSingle =
                reinterpret_cast<decltype(UKismetSystemLibrary::KismetSystemLibraryLineTraceSingle)>(Address);
            LOG(LOG_INFO, "Found UKismetSystemLibrary::LineTraceSingle offset: 0x%p",
                reinterpret_cast<void*>(Address - Memory::GetImageBase()));
            return true;
        }
    }

    Error::ThrowError("Failed to find UKismetSystemLibrary::LineTraceSingle!");
    return false;
}

static bool SetupUnrealGeneralOffsets() {
    if (!SetupGObjects())
        return false;
    if (!SetupFMemoryRealloc())
        return false;
    if (!SetupFNameToString())
        return false;
    if (!SetupFNameConstructorW())
        return false;
    if (!SetupUKismetSystemLibraryLineTraceSingle())
        return false;
    return true;
}

// --- Unreal-Engine struct/class offsets ----------------------------

static bool Setup_UProperty_Offset_Internal() {
    /*temporary*/
    static bool bUnrealVersionHigherThan23 = false;
    UProperty::Offset_Internal_Offset = bUnrealVersionHigherThan23 ? 0x4C : 0x44;
    LOG(LOG_INFO, "Using hardcoded UProperty::Offset_Internal offset: 0x%X", UProperty::Offset_Internal_Offset);
    return true;
}
static bool Setup_UBoolProperty_ByteMask() {
    UBoolProperty::ByteMask_Offset = 0x72;
    LOG(LOG_INFO, "Using hardcoded UBoolProperty::ByteMask offset: 0x%X", UBoolProperty::ByteMask_Offset);
    return true;
}
static bool Setup_UClass_ClassCastFlags() {
    std::vector<std::pair<void*, EClassCastFlags>> Pairs = {
        {UObject::FindObjectFast("Actor"), EClassCastFlags::Actor},
        {UObject::FindObjectFast("Class"), EClassCastFlags::Field | EClassCastFlags::Struct | EClassCastFlags::Class}};

    UClass::ClassCastFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UClass::ClassCastFlags_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassCastFlags offset: 0x%X", UClass::ClassCastFlags_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UClass::ClassCastFlags offset!");
    return false;
}
static bool Setup_UClass_ClassDefaultObject() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("Object"), UObject::FindObjectFast("Default__Object")},
        {UObject::FindObjectFast("Field"), UObject::FindObjectFast("Default__Field")}};

    UClass::ClassDefaultObject_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UClass::ClassDefaultObject_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassDefaultObject offset: 0x%X", UClass::ClassDefaultObject_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UClass::ClassDefaultObject offset!");
    return false;
}
static bool Setup_UStruct_SuperStruct() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("Struct"), UObject::FindObjectFast("Field")},
        {UObject::FindObjectFast("Class"), UObject::FindObjectFast("Struct")}};

    if (!Pairs[0].first)
        Pairs[0].first = Pairs[1].second = UObject::FindObjectFast("struct");

    UStruct::SuperStruct_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UStruct::SuperStruct_Offset) {
        LOG(LOG_INFO, "Found UStruct::SuperStruct offset: 0x%X", UStruct::SuperStruct_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UStruct::SuperStruct offset!");
    return false;
}
static bool Setup_UStruct_Children() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("PlayerController"),
         UObject::FindObjectFastWithOuter("WasInputKeyJustReleased", "PlayerController")},
        {UObject::FindObjectFast("Controller"), UObject::FindObjectFastWithOuter("UnPossess", "Controller")}};

    UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!UStruct::Children_Offset) {
        LOG(LOG_INFO, "Failed to find UStruct::Children with object pairs 1. Attempting again with object pairs 2.");

        Pairs = {{UObject::FindObjectFast("Vector"), UObject::FindObjectFastWithOuter("X", "Vector")},
                 {UObject::FindObjectFast("Vector4"), UObject::FindObjectFastWithOuter("X", "Vector4")},
                 {UObject::FindObjectFast("Vector2D"), UObject::FindObjectFastWithOuter("X", "Vector2D")},
                 {UObject::FindObjectFast("Guid"), UObject::FindObjectFastWithOuter("A", "Guid")}};

        UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (UStruct::Children_Offset) {
        LOG(LOG_INFO, "Found UStruct::Children offset: 0x%X", UStruct::Children_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UStruct::Children offset!");
    return false;
}
static bool Setup_UStruct_ChildProperties() {
    UStruct::ChildProperties_Offset = 0x50; // Potentially unsafe to be hardcoded.
    LOG(LOG_INFO, "Using potentially unsafe hardcoded UStruct::ChildProperties offset: 0x%X",
        UStruct::ChildProperties_Offset);
    return true;
}
static bool Setup_UStruct_StructSize() {
    std::vector<std::pair<void*, int32_t>> Pairs = {{UObject::FindObjectFast("Color"), 0x4},
                                                    {UObject::FindObjectFast("Guid"), 0x10}};

    UStruct::PropertiesSize_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UStruct::PropertiesSize_Offset) {
        LOG(LOG_INFO, "Found UStruct::PropertiesSize offset: 0x%X", UStruct::PropertiesSize_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UStruct::PropertiesSize offset!");
    return false;
}
static bool Setup_UStruct_MinAlignment() {
    UStruct::MinAlignment_Offset = UStruct::PropertiesSize_Offset + 4; // Potentially unsafe to be hardcoded.
    LOG(LOG_INFO, "Using potentially unsafe hardcoded UStruct::MinAlignment offset: 0x%X",
        UStruct::MinAlignment_Offset);
    return true;
}
static bool Setup_UField_Next() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast<UClass>("KismetArrayLibrary", EClassCastFlags::Class)->Children,
         UObject::FindObjectFast<UClass>("FilterArray", EClassCastFlags::Function)}};

    UField::Next_Offset = Memory::FindMatchingValueOffset(Pairs, 0x10, 0x40);
    if (UField::Next_Offset) {
        LOG(LOG_INFO, "Found UField::Next offset: 0x%X", UField::Next_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UField::Next offset!");
    return false;
}
static bool Setup_UFunction_FunctionFlags() {
    std::vector<std::pair<void*, EFunctionFlags>> Pairs = {
        {UObject::FindObjectFast("WasInputKeyJustPressed"),
         EFunctionFlags::Final | EFunctionFlags::Native | EFunctionFlags::Public | EFunctionFlags::BlueprintCallable |
             EFunctionFlags::BlueprintPure | EFunctionFlags::Const},
        {UObject::FindObjectFast("ToggleSpeaking"),
         EFunctionFlags::Exec | EFunctionFlags::Native | EFunctionFlags::Public},
        {UObject::FindObjectFast("SwitchLevel"),
         EFunctionFlags::Exec | EFunctionFlags::Native | EFunctionFlags::Public}};

    UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!UFunction::FunctionFlags_Offset) {
        for (auto& [_, Flags] : Pairs)
            Flags = Flags | EFunctionFlags::RequiredAPI;
        UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (UFunction::FunctionFlags_Offset) {
        LOG(LOG_INFO, "Found UFunction::FunctionFlags offset: 0x%X", UFunction::FunctionFlags_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UFunction::FunctionFlags offset!");
    return false;
}
static bool Setup_UFunction_FuncPtr() {
    PIMAGE_SECTION_HEADER TextSection = Memory::GetImageTextSection();
    uintptr_t             TextStart = TextSection->VirtualAddress + Memory::GetImageBase();
    uintptr_t             TextEnd = TextStart + TextSection->Misc.VirtualSize;

    UFunction* Func = UObject::FindObjectFast<UFunction>("WasInputKeyJustPressed");

    for (uint32_t i = UFunction::FunctionFlags_Offset; i < UFunction::FunctionFlags_Offset + 0x30; i++) {
        uintptr_t As64BitInt = *(uintptr_t*)((uintptr_t)Func + i);
        if (As64BitInt > TextStart && As64BitInt < TextEnd) {
            UFunction::FuncPtr_Offset = i;
            LOG(LOG_INFO, "Found UFunction::FuncPtr offset: 0x%X", UFunction::FuncPtr_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find UFunction::FuncPtr offset!");
    return false;
}

static bool SetupUnrealStructOffsets() {
    if (!Setup_UProperty_Offset_Internal())
        return false;
    if (!Setup_UBoolProperty_ByteMask())
        return false;
    if (!Setup_UClass_ClassCastFlags())
        return false;
    if (!Setup_UClass_ClassDefaultObject())
        return false;
    if (!Setup_UStruct_SuperStruct())
        return false;
    if (!Setup_UStruct_Children())
        return false;
    if (!Setup_UStruct_ChildProperties())
        return false;
    if (!Setup_UStruct_StructSize())
        return false;
    if (!Setup_UStruct_MinAlignment())
        return false;
    if (!Setup_UField_Next())
        return false;
    if (!Setup_UFunction_FunctionFlags())
        return false;
    if (!Setup_UFunction_FuncPtr())
        return false;
    return true;
}

// --- Unreal-Engine & Fortnite general offsets ----------------------

static bool SetupProcessEvent() {
    auto Resolve32BitRelativeJump = [](void* FunctionPtr) -> uintptr_t {
        uint8_t* Address = reinterpret_cast<uint8_t*>(FunctionPtr);
        if (*Address == 0xE9) {
            uintptr_t Ret = (uintptr_t)((Address + 5) + *reinterpret_cast<int32_t*>(Address + 1));
            if (Memory::IsAddressInsideImage(Ret))
                return Ret;
        }

        return reinterpret_cast<uintptr_t>(FunctionPtr);
    };

    void** VTable = UObject::Objects.GetByIndex(0)->VTable;
    for (int i = 0; i < 0x150; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        if (Memory::PatternScanRangeBytes<int32_t>(
                Resolve32BitRelativeJump(VTable[i]), 0x400,
                {0xF7, -0x1, (int32_t)UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x04, 0x0, 0x0}, false, -1,
                false, false) &&
            Memory::PatternScanRangeBytes<int32_t>(
                Resolve32BitRelativeJump(VTable[i]), 0x400,
                {0xF7, -0x1, (int32_t)UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0}, false, -1,
                false, false)) {
            UObject::ProcessEvent_Idx = i;
            LOG(LOG_INFO, "Found UObject::ProcessEvent VFT index: 0x%X", UObject::ProcessEvent_Idx);
            return true;
        }
    }

    Error::ThrowError("Failed to find UObject::ProcessEvent VFT index!");
    return false;
}
static bool SetupEngineVersion() {
    FString EngineVersionStr = UKismetSystemLibrary::GetEngineVersion();

    LOG(LOG_INFO, "Found EngineVersion String: %s", EngineVersionStr.ToString().c_str());

    std::istringstream stream(EngineVersionStr.ToString());
    std::string        line;
    while (std::getline(stream, line)) {
        size_t      DashPos = line.find('-');
        std::string EngineStr = line.substr(0, DashPos);

        size_t      ReleasePos = line.find("Release-");
        std::string GameStr = line.substr(ReleasePos + 8);

        size_t      PlusPos = line.find("+++");
        std::string CLStr = line.substr(DashPos + 1, PlusPos - DashPos - 1);

        g_EngineVersion = std::stof(EngineStr.substr(0, EngineStr.find_last_of('.')));
        g_CL = std::stoi(CLStr);

        // Scuffed fix for game version 1.8 and likely lower
        if (GameStr == "Live")
            g_GameVersion = 1.f; // 1.f default value
        else
            g_GameVersion = std::stof(GameStr);
    }

    LOG(LOG_INFO, "EngineVersion: %f, GameVersions: %f, CL: %d", g_EngineVersion, g_GameVersion, g_CL);
    return true;
}
static bool SetupDrawTransition() {
    void**       VTable = UObject::FindObjectFast("Default__GameViewportClient")->VTable;
    PropertyInfo GameInstanceProp = UObject::GetPropertyInfo("GameViewportClient", "GameInstance");
    int          bSuppressTransitionMessage = GameInstanceProp.Offset + sizeof(void*);

    for (int i = 0x30; i < 0x80 - 1; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        uintptr_t Address = Memory::PatternScanRangeBytes<int32_t>(
            (uintptr_t)VTable[i], 0x35, {0x80, 0xB9, bSuppressTransitionMessage, 0x00, 0x00, 0x00, 0x00}, false, -1,
            false, false);
        if (Address) {
            UGameViewportClient::DrawTransition_Idx = i;
            LOG(LOG_INFO, "Found UGameViewportClient::DrawTransition VFT index: 0x%X",
                UGameViewportClient::DrawTransition_Idx);
            return true;
        }
    }

    Error::ThrowError("Failed to find UCanvas::DrawTransition VFT index!");
    return false;
}
static bool SetupViewProjectionMatrix() {
    uintptr_t SearchStart = Memory::FindStringRef(L"/Engine/EngineResources/GradientTexture0.");
    if (SearchStart) {
        // Resolve the jmp if the function is split into multiple parts.
        uintptr_t Jmp = Memory::PatternScanRange<int32_t>(SearchStart, 0x30, "E9", false, 1, true);
        if (Jmp)
            SearchStart = Jmp;
    } else {
        SearchStart = Memory::PatternScan("89 ? 38 48 8B 05 ? ? ? ? 48 89 ? 60 48 8B 05");
    }

    if (SearchStart) {
        // The signature we are looking for is:
        /*
        0F 28 05 50 50 22 02      movaps  xmm0, cs:xmmword_474C280
        0F 11 83 C0 02 00 00      movups  xmmword ptr [rbx+2C0h], xmm0
        0F 28 0D 02 50 22 02      movaps  xmm1, cs:xmmword_474C240
        0F 11 8B 80 02 00 00      movups  xmmword ptr [rbx+280h], xmm1
        */

        // The second movups instruction is writing to the ViewProjectionMatrix, so we will get the offset of that
        // instruction.
        uintptr_t Sig = Memory::PatternScanRange<int32_t>(
            SearchStart, 0x150, "0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 48");
        if (!Sig)
            Sig = Memory::PatternScanRange<int32_t>(
                SearchStart, 0x150, "0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 4C");

        if (Sig) {
            UCanvas::ViewProjectionMatrix_Offset = *(uint32_t*)(Sig - 18);
            LOG(LOG_INFO, "Found UCanvas::ViewProjectionMatrix offset: 0x%X", UCanvas::ViewProjectionMatrix_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find UCanvas::ViewProjectionMatrix offset!");
    return false;
}
static bool SetupLevelActors() {
    UWorld* World = Core::GetNewWorld();
    if (!World) {
        Error::ThrowError("Failed to get World to find ULevel::Actors offset!");
        return false;
    }

    ULevel* Level = World->PersistentLevel;
    if (!Level) {
        Error::ThrowError(
            "Failed to get PersistentLevel to find ULevel::Actors offset! Injected DLL before game loaded?");
        return false;
    }

    PropertyInfo Info = UObject::GetPropertyInfo("Level", "OwningWorld");
    if (Info.Found) {
        for (int i = 0x70; i < Info.Offset; i += 8) {
            // We will check if the address is a valid TArray<AActor*> by checking if the Max() is greater than Num()
            // and if the pointer to the data is valid.
            TArray<AActor*>* Actors = reinterpret_cast<TArray<AActor*>*>((uintptr_t)Level + i);
            if (!Actors->IsValid())
                continue;
            if (Actors->Max() <= 0 || Actors->Num() <= 0)
                continue;
            if (Actors->Max() < Actors->Num())
                continue;

            ULevel::Actors_Offset = i;
            LOG(LOG_INFO, "Found ULevel::Actors offset: 0x%X", ULevel::Actors_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find ULevel::Actors offset!");
    return false;
}
static bool SetupComponentSpaceTransformsArray() {
    PropertyInfo Info = UObject::GetPropertyInfo("SkinnedMeshComponent", "VertexOffsetUsage", true);
    int32_t      Offset = Info.Offset + 0x10;
    if (!Info.Found) {
        Info = UObject::GetPropertyInfo("SkinnedMeshComponent", "MasterPoseComponent");
        Offset = Info.Offset + 0x8;
    }

    if (Info.Found) {
        USkinnedMeshComponent::ComponentSpaceTransformsArray_Offset = Offset;
        LOG(LOG_INFO, "Found USkinnedMeshComponent::ComponentSpaceTransformsArray offset: 0x%X",
            USkinnedMeshComponent::ComponentSpaceTransformsArray_Offset);
        return true;
    }

    Error::ThrowError("Failed to find USkinnedMeshComponent::ComponentSpaceTransformsArray offset!");
    return false;
}
#ifdef _ENGINE
static bool SetupCanvas() {
    UCanvas::Canvas_Offset = UCanvas::ViewProjectionMatrix_Offset - (sizeof(void*) * 2);
    LOG(LOG_INFO, "Using hardcoded UCanvas::Canvas offset: 0x%X", UCanvas::Canvas_Offset);
    return true;
}
static bool SetupFCanvasGetBatchedElements() {
    uintptr_t StringRef = Memory::FindStringRef(L"STAT_Canvas_GetBatchElementsTime");
    if (!StringRef) {
        Error::ThrowError("Failed to find \"STAT_Canvas_GetBatchElementsTime\" for FCanvas::GetBatchedElements!");
        return false;
    }

    uintptr_t Address = Memory::PatternScanRange<int32_t>(StringRef, 0x100, "48 8B C4", true);
    if (Memory::IsAddressInsideImage(Address)) {
        FCanvas::FCanvasGetBatchedElements = reinterpret_cast<decltype(FCanvas::FCanvasGetBatchedElements)>(Address);
        LOG(LOG_INFO, "Found FCanvas::GetBatchedElements offset: 0x%p",
            reinterpret_cast<void*>(Address - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find FCanvas::GetBatchedElements!");
    return false;
}
static bool SetupBatchedThickLines() {
    FBatchedElements::BatchedThickLines_Offset = 0x40;
    LOG(LOG_INFO, "Using hardcoded FBatchedElements::BatchedThickLines offset: 0x%X",
        FBatchedElements::BatchedThickLines_Offset);
    return true;
}
#endif
static bool SetupEditModeInputComponent0Offset(uint64_t& Output) {
    Output = Memory::FindStringRef(L"EditModeInputComponent0");
    if (Output) {
        LOG(LOG_INFO, "Found EditModeInputComponent0 offset: %p",
            reinterpret_cast<void*>(Output - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find EditModeInputComponent0 offset!");
    return false;
}
static bool SetupFireOffset(uint64_t EditModeInputComponent0) {
    uintptr_t FireString = Memory::FindStringRefRange("Fire", (uint8_t*)(EditModeInputComponent0 - 0x4500), 0x4500);
    if (!FireString) {
        Error::ThrowError("Failed to find Fire_Press \"Fire\" string");
        return false;
    }

    LOG(LOG_INFO, "Found Fire_Press \"Fire\" string ref: %p",
        reinterpret_cast<void*>(FireString - Memory::GetImageBase()));

    // Search for all lea r64,m patterns, and return the earliest one
    auto FindLeaInstruction = [](uintptr_t SearchStart, uintptr_t SearchSize) {
        constexpr const char* Patterns[] = {
            "48 8D", // Legacy registers
            "4C 8D", // Numbered registers
        };

        uintptr_t EarliestResult = UINT64_MAX;
        for (const char* Pattern : Patterns) {
            uintptr_t Result = Memory::PatternScanRange<int32_t>(SearchStart, SearchSize, Pattern, false, 3, true);

            // We would normally virtual query the result to make sure it's a function (executable) and not
            // something like a string, this isn't possible since fortnite uses rwx for all sections.
            if (Memory::IsAddressInsideImage(Result) && Result < EarliestResult)
                EarliestResult = Result;
        }

        return EarliestResult == UINT64_MAX ? 0 : EarliestResult;
    };

    uintptr_t FirePressAddress = FindLeaInstruction(FireString - 0x28, 0x50);
    if (!FirePressAddress) {
        Error::ThrowError("Failed to find Fire_Press");
        return false;
    }

    // Find the next "Fire" string, used for Fire_Release
    FireString = Memory::FindStringRefRange("Fire", reinterpret_cast<uint8_t*>(FireString + 1), 0x100);
    if (!FireString) {
        Error::ThrowError("Failed to find Fire_Release \"Fire\" string");
        return false;
    }

    LOG(LOG_INFO, "Found Fire_Release \"Fire\" string ref: %p",
        reinterpret_cast<void*>(FireString - Memory::GetImageBase()));

    uintptr_t FireReleaseAddress = FindLeaInstruction(FireString - 0x28, 0x50);
    if (!FireReleaseAddress) {
        Error::ThrowError("Failed to find Fire_Release");
        return false;
    }

    AFortPlayerController::pFire_Press =
        reinterpret_cast<decltype(AFortPlayerController::pFire_Press)>(FirePressAddress);
    LOG(LOG_INFO, "Found AFortPlayerController::Fire_Press offset: %p",
        reinterpret_cast<void*>(FirePressAddress - Memory::GetImageBase()));

    AFortPlayerController::pFire_Release =
        reinterpret_cast<decltype(AFortPlayerController::pFire_Release)>(FireReleaseAddress);
    LOG(LOG_INFO, "Found AFortPlayerController::Fire_Release offset: %p",
        reinterpret_cast<void*>(FireReleaseAddress - Memory::GetImageBase()));

    return true;
}
static void SetupGetTargetingTransform() {
    auto FortPawn = UObject::FindObjectFast<AFortPawn>("Default__FortPawn");
    if (!FortPawn) {
        LOG(LOG_WARN, "Failed to get Default__FortPawn to find GetTargetingTransform offset!");
        return;
    }

    UStruct* Super = FortPawn->Class->SuperStruct;
    int32_t  StructSize = Super->PropertiesSize;
    int32_t  MinAlignment = Super->MinAlignment;

    // long ahh code to align properly to get afortpawn class start addr
    int32_t RequiredAlign = MinAlignment - (StructSize % MinAlignment);
    int32_t FortPawnClassStart = StructSize + (RequiredAlign != MinAlignment ? RequiredAlign : 0x0);

    // AFortPawn has several interfaces:
    // AFGF_Character, IGameplayTagAssetInterface, IFortDamageableActorInterface, IFortSpottableActorInterface,
    // IFortAbilitySystemInterface, IGameplayCueInterface, IFortTargetSelectionInterface,
    // IFortAIEncounterInfoOwnerInterface, IVisualLoggerDebugSnapshotInterface, IFortHealthRegenInterface,
    // IAISightTargetInterface, IFortAutoFireTargetInterface, IFortLockOnTargetInterface,
    // IAbilitySystemReplicationProxyInterface, IFortCurieInterface, IFortReplicationGraphInterface,
    // IFortSoundIndicatorInterface
    //
    // The way this gets compiled is that AFortPawn has a pointer to each one of these interfaces at the start of the
    // class, which it can swap out at runtime if it pleases.
    //
    // The interface we want to find is the IFortTargetSelectionInterface, which is usually the 8th or 9th interface,
    // depending on the game version
    //
    // What we will do is scan these interfaces to find a VFT and check its 3rd function (VFT index doesnt change afaik)
    // for a few byte patterns I have found reliable across game versions.

    // Never seen the VFT be further than this
    int32_t MaxSearchOffset = FortPawnClassStart + 0x100;

    constexpr int32_t VFTIndex = 2;
    for (int32_t Offset = FortPawnClassStart; Offset < MaxSearchOffset; Offset += sizeof(void*)) {
        void** VFT = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(FortPawn) + Offset);
        if (!Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VFT)))
            continue;

        void* Function = VFT[VFTIndex];
        if (!Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(Function)))
            continue;

        struct PatternInfo {
            const char* Pattern;
            int32_t     SearchSize;
            int32_t     ByteCount;
        };
        std::vector<PatternInfo> Patterns = {
            {"41 54", 0x25, 2},                          // push r12
            {"41 56 41 57", 0x5, 4},                     // push r14
                                                         // push r15
            {"48 8B DA", 0x50, 3},                       // mov rbx, rdx
            {"48 85 C9 74", 0x10, 4},                    // test rcx, rcx
                                                         // jz
            {"0F C6 C1 55 0F C6 C9 AA F3 0F", 0x50, 10}, // shufps xmm0, xmm1, 0x55
                                                         // shufps xmm1, xmm1, 0xAA
                                                         // movss dword ptr
            {"F3 0F", 0x10, 2},                          // movss   dword ptr
            {"EB", 0x10, 1}                              // jmp
        };

        bool FailedToFindPattern = false;

        int32_t SearchOffset = 0;
        for (const auto& Pattern : Patterns) {
            // SearchOffset will be the last search offset
            uintptr_t SearchAddr = reinterpret_cast<uintptr_t>(Function) + SearchOffset;
            uintptr_t PatternAddr = Memory::PatternScanRange(SearchAddr, Pattern.SearchSize, Pattern.Pattern);
            if (!PatternAddr) {
                FailedToFindPattern = true;
                break;
            }

            SearchOffset = Pattern.ByteCount + (PatternAddr - reinterpret_cast<uintptr_t>(Function));
        }

        if (!FailedToFindPattern) {
            AFortPawn::pGetTargetingTransform = reinterpret_cast<decltype(AFortPawn::pGetTargetingTransform)>(Function);
            LOG(LOG_INFO, "Found AFortPawn::IFortAbilityTargetSelection->GetTargetingTransform offset: %p",
                reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(Function) - Memory::GetImageBase()));
            return;
        }
    }

    LOG(LOG_WARN, "Failed to find AFortPawn::IFortAbilityTargetSelection->GetTargetingTransform!");
}
static void FindGetWeaponStats() {
    void** VTable = UObject::FindObjectFast("Default__FortWeapon")->VTable;

    for (int i = 0x30; i < 0x100; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        if (Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x83, 0xEC, 0x58, 0x48, 0x8B, -0x01, -0x01, -0x01, -0x01, -0x01, 0x48, 0x85}) ||
            Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x8B, 0x89, -0x01, -0x01, 0x00, 0x00, 0x48, -0x01, -0x01, 0x74, 0x13}) ||
            Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x8B, 0x60, -0x01, -0x01, 0x00, 0x00, 0x48, -0x01, -0x01, 0x8B, 0xFA})) {
            AFortWeapon::GetWeaponStats_Idx = i;
            LOG(LOG_INFO, "Found AFortWeapon::GetWeaponStats VFT index: 0x%X", AFortWeapon::GetWeaponStats_Idx);
            return;
        }
    }

    LOG(LOG_WARN,
        "Failed to find AFortWeapon::GetWeaponStats VFT index! WeaponStats based exploits will no longer work.");
}
static void FindComponentToWorldOffset() {
    void* execK2_GetComponentToWorld = UObject::GetFunction("SceneComponent", "K2_GetComponentToWorld")->FuncPtr;
    if (execK2_GetComponentToWorld) {
        uintptr_t K2_GetComponentToWorld =
            Memory::PatternScanRange<int32_t>((uintptr_t)execK2_GetComponentToWorld, 0x50, "E8", false, 1, true);
        if (!K2_GetComponentToWorld)
            K2_GetComponentToWorld = reinterpret_cast<uintptr_t>(execK2_GetComponentToWorld);

        uint8_t* Data = (uint8_t*)Memory::PatternScanRange<int32_t>(K2_GetComponentToWorld, 0x30, "0F 10");
        if (Data && Data[0] == 0x0F && Data[1] == 0x10 && Data[7] == 0x0F && Data[8] == 0x10 && Data[14] == 0x0F &&
            Data[15] == 0x10) {
            USceneComponent::ComponentToWorld_Offset = *(uint32_t*)(Data + 3);
            LOG(LOG_INFO, "Found USceneComponent::ComponentToWorld offset: 0x%X",
                USceneComponent::ComponentToWorld_Offset);
            return;
        }
    }

    LOG(LOG_WARN, "Failed to find USceneComponent::ComponentToWorld offset! Will fallback to calling UFunction.");
}

static bool SetupUnrealFortniteOffsets() {
    if (!SetupProcessEvent())
        return false;
    if (!SetupEngineVersion())
        return false;
    if (!SetupDrawTransition())
        return false;
    if (!SetupViewProjectionMatrix())
        return false;
    if (!SetupLevelActors())
        return false;
    if (!SetupComponentSpaceTransformsArray())
        return false;
#ifdef _ENGINE
    if (!SetupCanvas())
        return false;
    if (!SetupFCanvasGetBatchedElements())
        return false;
    if (!SetupBatchedThickLines())
        return false;
#endif

    SetupGetTargetingTransform();
    FindGetWeaponStats();
    FindComponentToWorldOffset();

    uint64_t EditModeInputComponent0 = 0;
    if (SetupEditModeInputComponent0Offset(EditModeInputComponent0)) {
        if (!SetupFireOffset(EditModeInputComponent0))
            return false;
    }

    return true;
}

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up SDK...");

    if (!SetupUnrealGeneralOffsets())
        return false;
    if (!SetupUnrealStructOffsets())
        return false;
    if (!SetupUnrealFortniteOffsets())
        return false;

    return true;
}

// --- Global Variables ----------------------------------------------

float g_EngineVersion = 0.f;
float g_GameVersion = 0.f;
int   g_CL = 0;

} // namespace SDK
