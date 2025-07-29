#pragma once
#include <string>

#include <cheat/sdk/Basic.h>

namespace ConfigReflection {

// --- Reflection Structs --------------------------------------------

template <typename T> struct TypeDescriptor {
    static constexpr std::string_view Name = "unknown";
};

template <> struct TypeDescriptor<bool> {
    static constexpr std::string_view Name = "bool";
};

template <> struct TypeDescriptor<float> {
    static constexpr std::string_view Name = "float";
};

template <> struct TypeDescriptor<int> {
    static constexpr std::string_view Name = "int";
};

template <> struct TypeDescriptor<SDK::FLinearColor> {
    static constexpr std::string_view Name = "FLinearColor";
};

template <typename Class, typename T> struct MemberDescriptor {
    std::string_view Name;
    T Class::*        Ptr;
    TypeDescriptor<T> Type;
};

struct ConfigFieldView {
    void*            Ptr;
    std::string_view Name;
    std::string_view Type;
    std::string      FullPath;

    template <typename T> T&   As() const { return *reinterpret_cast<T*>(Ptr); }
    template <typename T> bool Is() const { return Type == TypeDescriptor<T>::Name; }
};

} // namespace ConfigReflection
