#include "hooks.h"

#include <extern/minhook/include/MinHook.h>
#include <utils/log.h>

namespace Hooks {

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up Hooks...");

    MH_STATUS Result = MH_Initialize();
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_Initialize failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

void Destroy() {
    LOG(LOG_TRACE, "Destroying Hooks...");
    MH_Uninitialize();
}

// --- Hook Management -----------------------------------------------

bool CreateHook(void* Target, void* Detour, void** Original) {
    MH_STATUS Result = MH_CreateHook(Target, Detour, Original);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_CreateHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool EnableHook(void* Target) {
    MH_STATUS Result = MH_EnableHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_EnableHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool RemoveHook(void* Target) {
    MH_STATUS Result = MH_RemoveHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_RemoveHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool DisableHook(void* Target) {
    MH_STATUS Result = MH_DisableHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_DisableHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

} // namespace Hooks
