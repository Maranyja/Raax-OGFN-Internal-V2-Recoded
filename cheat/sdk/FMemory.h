#pragma once
#include <cstdint>

namespace SDK::FMemory {

inline void* (*FMemoryRealloc)(void* Original, uint32_t Size, uint32_t Alignment) = nullptr;

void                Free(void* Original);
[[nodiscard]] void* Malloc(uint32_t Size, uint32_t Alignment);
[[nodiscard]] void* Realloc(void* Original, uint32_t Size, uint32_t Alignment);

} // namespace SDK::FMemory
