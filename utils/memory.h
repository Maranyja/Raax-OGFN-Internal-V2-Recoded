#pragma once
#include <Windows.h>
#include <vector>

namespace Memory {

/*
 * @brief Checks if an address is accessible and writable.
 */
bool IsValidAndWritable(void* Address);

/*
 * @brief Returns valid DosHeader from ModuleBase.
 * Logs a warning on failure.
 */
PIMAGE_DOS_HEADER GetDosHeader(uintptr_t ModuleBase);
/*
 * @brief Returns valid NtHeaders from ModuleBase.
 * Logs a warning on failure.
 */
PIMAGE_NT_HEADERS64 GetNtHeaders(uintptr_t ModuleBase);
/*
 * @brief Returns first section matching name (Name chopped to IMAGE_SIZEOF_SHORT_NAME).
 * Logs a warning on failure.
 */
PIMAGE_SECTION_HEADER GetSectionFromName(const char* Name, uintptr_t ModuleBase);

/* @brief Returns a modules size from it's NT headers. */
uint64_t GetModuleSize(uintptr_t ModuleBase);
/* @brief Wrapper for basic range check. */
bool IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase);

/* @brief Returns the main executable image base address. */
uintptr_t GetImageBase();
/* @brief Returns the main executable image size. */
uint64_t GetImageSize();
/* @Brief Returns first ".text" section in the main executable image. */
PIMAGE_SECTION_HEADER GetImageTextSection();
/* @brief Wrapper for basic range check in the main executable image. */
bool IsAddressInsideImage(uintptr_t Address);

/**
 * @brief Scans memory range for a byte pattern.
 *
 * @tparam OperandSize - The size of the operand used for relative address calculations.
 * @param StartAddress - The starting address for the scan.
 * @param ScanSize - The size of the memory region to scan.
 * @param Bytes - The raw bytes to scan for (use -1 for wildcard).
 * @param Backwards - Whether to scan backwards or forwards. This means it will move ScanSize backwards from
 * StartAddress instead of forwards.
 * @param Offset - The offset to apply when computing the final address.
 * @param RelativeAddress - If true, computes a relative address based on OperandSize.
 * @param WarnIfNotFound - Log a warning message if the pattern is not found, only functional if CFG_DEBUGMEMORYOUTPUT
 * is true.
 * @return The address of the first match found, or 0 if no match is found.
 */
template <typename OperandSize = int32_t>
uintptr_t PatternScanRangeBytes(uintptr_t StartAddress, uint64_t ScanSize, const std::vector<int>& Bytes,
                                bool Backwards = false, size_t Offset = -1, bool RelativeAddress = false,
                                bool WarnIfNotFound = true);

/**
 * @brief Scans memory range for a byte pattern using an IDA style signature.
 *
 * @tparam OperandSize - The size of the operand used for relative address calculations.
 * @param StartAddress - The starting address for the scan.
 * @param ScanSize - The size of the memory region to scan.
 * @param Pattern - The pattern to search for (supports wildcard '?').
 * @param Backwards - Whether to scan backwards or forwards. This means it will move ScanSize backwards from
 * StartAddress instead of forwards.
 * @param Offset - The offset to apply when computing the final address.
 * @param RelativeAddress - If true, computes a relative address based on OperandSize.
 * @return The address of the first match found, or 0 if no match is found.
 */
template <typename OperandSize = int32_t>
uintptr_t PatternScanRange(uintptr_t StartAddress, uint64_t ScanSize, const char* Pattern, bool Backwards = false,
                           size_t Offset = -1, bool RelativeAddress = false);

/**
 * @brief Scans the main executable image for a byte pattern.
 * This function is a convenience wrapper for PatternScan, automatically scanning the main executable image.
 */
template <typename OperandSize = int32_t>
uintptr_t PatternScan(const char* Pattern, size_t Offset = -1, bool RelativeAddress = false);

/**
 * @brief Finds first instruction that references a string using a LEA instruction in the main executable image.
 * @tparam StringType - Must be `const char*` or `const wchar_t*`.
 * @param RefString - Target string to search for.
 * @param StartAddress - The starting address for the scan.
 * @param ScanSize - The size of the memory region to scan.
 * @return The address of the found instruction.
 */
template <typename StringType>
static uintptr_t FindStringRefRange(StringType RefString, uint8_t* StartAddress, size_t ScanSize);

/**
 * @brief Scans the main images text section for a string reference.
 * This function is a convenience wrapper for FindStringRefRange, automatically scanning the main images text section.
 */
template <typename StringType = const char*> uintptr_t FindStringRef(StringType RefString);

/**
 * @brief Finds an offset where all objects and their value pairs match.
 * @tparam T - Value type for pairs.
 * @tparam Increment - Amount of bytes to increment from MinOffset until MaxOffset.
 * @param ObjectValuePairs - Pairs of objects to their expected values.
 * @param MinOffset - Offset to start searching from.
 * @param MaxOffset - Offset to stop search from. Scanning memory read bounds will end at MaxOffset + sizeof(T).
 * @return The offset at which all objects match their expected value, or 0 on failure.
 */
template <typename T, int Increment = 4>
uint32_t FindMatchingValueOffset(const std::vector<std::pair<void*, T>>& ObjectValuePairs, uint32_t MinOffset = 0x28,
                                 uint32_t MaxOffset = 0x1A0);

} // namespace Memory

#include "memory.inl"
