#pragma once
#include "FMemory.h"
#include "CoreUObject.h"
#include "Engine.h"
#include "FortniteGame.h"

namespace SDK {

// --- Initialization ------------------------------------------------

bool Init();

// --- Global Variables ----------------------------------------------

extern float g_EngineVersion;
extern float g_GameVersion;
extern int   g_CL;

} // namespace SDK
