#pragma once
#include <string>

namespace Error {

[[noreturn]] void ThrowError(const std::string& Msg);

} // namespace Error
