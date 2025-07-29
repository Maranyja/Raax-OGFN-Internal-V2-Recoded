#include "error.h"

#include <Windows.h>

#include <utils/log.h>

namespace Error {

[[noreturn]] void ThrowError(const std::string& Msg) {
    LOG(LOG_ERROR, "Error thrown: %s", Msg.c_str());
    MessageBoxA(0, Msg.c_str(), "Error", MB_ICONERROR);

    *(uint64_t*)(0) = 5;

    // TODO: Unload the cheat instead of exiting.
    exit(1);
}

} // namespace Error
