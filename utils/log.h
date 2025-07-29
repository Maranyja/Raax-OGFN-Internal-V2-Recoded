#pragma once
#include <string>

#include <extern/loguru/loguru.hpp>
#include <globals.h>

namespace Log {

// --- Logging Functions & Data --------------------------------------

#define LOG_ERROR loguru::Verbosity_ERROR
#define LOG_WARN loguru::Verbosity_WARNING
#define LOG_INFO loguru::Verbosity_INFO
#define LOG_TRACE loguru::Verbosity_1

#if CFG_USELOGGING
void SetThreadLogName(const std::string& Name);
void InitLogger();
#define LOG(LogLevel, Message, ...) VLOG_F(LogLevel, Message, ##__VA_ARGS__)
#else
#define LOG void(0)
#endif

// --- Console Functions ---------------------------------------------

#if CFG_SHOWCONSOLE
void CreateConsole();
void DestroyConsole();
#endif

} // namespace Log
