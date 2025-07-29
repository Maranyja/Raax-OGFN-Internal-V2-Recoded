#include "FMemory.h"

namespace SDK::FMemory {

void Free(void* Original) {
    FMemoryRealloc(Original, 0, 0);
}
[[nodiscard]] void* Malloc(uint32_t Size, uint32_t Alignment) {
    return FMemoryRealloc(nullptr, Size, Alignment);
}
[[nodiscard]] void* Realloc(void* Original, uint32_t Size, uint32_t Alignment) {
    return FMemoryRealloc(Original, Size, Alignment);
}

} // namespace SDK::FMemory
