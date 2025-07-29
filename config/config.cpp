#include "config.h"
#include "config_reflection.h"
#include "keybind.h"

#include <sstream>
#include <charconv>

#include <extern/inifile-cpp/inicpp.h>
#include <extern/cpp-base64/base64.h>
#include <utils/error.h>
#include <utils/log.h>

namespace Config {

// --- Serialization Utility Functions -------------------------------

static bool TryParseInt(const std::string& Str, int& Out) {
    auto Result = std::from_chars(Str.data(), Str.data() + Str.size(), Out);
    return Result.ec == std::errc();
}

static bool TryParseFloat(const std::string& Str, float& Out) {
    auto Result = std::from_chars(Str.data(), Str.data() + Str.size(), Out);
    return Result.ec == std::errc();
}

static bool TryParseBool(const std::string& Str, bool& Out) {
    if (Str == "true") {
        Out = true;
        return true;
    } else if (Str == "false") {
        Out = false;
        return true;
    }
    return false;
}

static bool TryParseColor(const std::string& Str, SDK::FLinearColor& OutColor) {
    std::stringstream ss(Str);
    std::string       Item;
    int               Colors[4];
    int               i = 0;

    while (std::getline(ss, Item, '/') && i < 4) {
        if (!TryParseInt(Item, Colors[i]))
            return false;
        i++;
    }

    if (i != 4)
        return false;

    OutColor = SDK::FLinearColor{Colors[0] / 255.f, Colors[1] / 255.f, Colors[2] / 255.f, Colors[3] / 255.f};
    return true;
}

template <typename T> static bool TryParseEnum(const std::string& Str, T& Out) {
    int IntValue;
    if (TryParseInt(Str, IntValue)) {
        Out = static_cast<T>(IntValue);
        return true;
    }
    return false;
}

// --- Config Serialization Functions --------------------------------

static void SerializeKeybinds(ConfigData& Config) {
    Config.Keybinds.KeybindData =
        Config.Keybinds.Keybinds.empty() ? "" : Keybind::SerializeKeybinds(Config.Keybinds.Keybinds);
}

template <typename T> static bool ShouldSerializeValue(const T& Value, const T& MergeValue) {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        const auto Members = ConfigReflection::DescribeMembers<T>();
        bool       AnyDifference = false;
        std::apply(
            [&](const auto&... Member) {
                auto CompareMember = [&](const auto& m) {
                    if (ShouldSerializeValue(Value.*(m.Ptr), MergeValue.*(m.Ptr))) {
                        AnyDifference = true;
                    }
                };
                (CompareMember(Member), ...);
            },
            Members);
        return AnyDifference;
    } else {
        return Value != MergeValue;
    }
}

template <typename T>
static void StructureToIni(ini::IniFile& Ini, const T& Value, const T& MergeValue, bool IgnoreMergeConfig,
                           const std::string& Section = "", const std::string& ValueName = "") {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        if (!IgnoreMergeConfig && !ShouldSerializeValue(Value, MergeValue)) {
            return;
        }

        const auto Members = ConfigReflection::DescribeMembers<T>();
        std::apply(
            [&](const auto&... member) {
                auto process = [&](const auto& m) {
                    std::string NewSection;
                    std::string NewValueName;

                    if (Section.empty()) {
                        NewSection = std::string(m.Name);
                    } else {
                        NewSection = Section + "." + std::string(m.Name);
                    }
                    NewValueName = "";

                    StructureToIni(Ini, Value.*(m.Ptr), MergeValue.*(m.Ptr), IgnoreMergeConfig, NewSection,
                                   NewValueName);
                };
                (process(member), ...);
            },
            Members);
    } else {
        std::string ValStr;
        if (IgnoreMergeConfig || Value != MergeValue) {
            if constexpr (std::is_same_v<T, bool>) {
                ValStr = Value ? "true" : "false";
            } else if constexpr (std::is_enum_v<T>) {
                ValStr = std::to_string(static_cast<int>(Value));
            } else if constexpr (std::is_arithmetic_v<T>) {
                ValStr = std::to_string(Value);
            } else if constexpr (std::is_same_v<T, SDK::FLinearColor>) {
                ValStr = std::format("{}/{}/{}/{}", (int)(Value.R * 255.f), (int)(Value.G * 255.f),
                                     (int)(Value.B * 255.f), (int)(Value.A * 255.f));
            } else if constexpr (std::is_same_v<T, std::string>) {
                ValStr = Value;
            }
        }

        if (!Section.empty() && !ValStr.empty()) {
            size_t      LastDot = Section.find_last_of('.');
            std::string ActualSection;
            std::string ActualValueName;

            if (LastDot != std::string::npos) {
                ActualSection = Section.substr(0, LastDot);
                ActualValueName = Section.substr(LastDot + 1);
            } else {
                ActualSection = Section;
                ActualValueName = ValueName.empty() ? "Value" : ValueName;
            }

            Ini[ActualSection][ActualValueName] = ValStr;
        }
    }
}

template <typename T> static bool IniToStructure(ini::IniFile& Ini, const std::string& Section, T& OutValue) {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        const auto Members = ConfigReflection::DescribeMembers<T>();
        bool       AllSuccess = true;

        std::apply(
            [&](const auto&... member) {
                auto process = [&](const auto& m) {
                    std::string NewSection;
                    if (Section.empty()) {
                        NewSection = std::string(m.Name);
                    } else {
                        NewSection = Section + "." + std::string(m.Name);
                    }
                    if (!IniToStructure(Ini, NewSection, OutValue.*(m.Ptr))) {
                        AllSuccess |= false;
                    }
                };
                (process(member), ...);
            },
            Members);

        return AllSuccess;
    } else {
        if (Section.empty())
            return false;

        size_t      LastDot = Section.find_last_of('.');
        std::string ActualSection;
        std::string ActualValueName;

        if (LastDot != std::string::npos) {
            ActualSection = Section.substr(0, LastDot);
            ActualValueName = Section.substr(LastDot + 1);
        } else {
            ActualSection = Section;
            ActualValueName = "Value";
        }

        if (!Ini.contains(ActualSection) || !Ini[ActualSection].contains(ActualValueName)) {
            return false;
        }

        const std::string& ValStr = Ini[ActualSection][ActualValueName].as<std::string>();

        if constexpr (std::is_same_v<T, bool>) {
            return TryParseBool(ValStr, OutValue);
        } else if constexpr (std::is_enum_v<T>) {
            return TryParseEnum(ValStr, OutValue);
        } else if constexpr (std::is_arithmetic_v<T>) {
            if constexpr (std::is_integral_v<T>) {
                int temp;
                if (TryParseInt(ValStr, temp)) {
                    OutValue = static_cast<T>(temp);
                    return true;
                }
                return false;
            } else {
                float temp;
                if (TryParseFloat(ValStr, temp)) {
                    OutValue = static_cast<T>(temp);
                    return true;
                }
                return false;
            }
        } else if constexpr (std::is_same_v<T, SDK::FLinearColor>) {
            return TryParseColor(ValStr, OutValue);
        } else if constexpr (std::is_same_v<T, std::string>) {
            OutValue = ValStr;
            return true;
        }

        return false;
    }
}

static bool DeserializeKeybinds(ConfigData& Config) {
    if (Config.Keybinds.KeybindData.empty()) {
        return true;
    }

    if (!Keybind::DeserializeKeybinds(Config.Keybinds.KeybindData, Config.Keybinds.Keybinds)) {
        return false;
    }

    for (auto& Keybind : Config.Keybinds.Keybinds) {
        if (!ConfigReflection::FindFieldByPath(Keybind.ReflectedBool.FullPath, Keybind.ReflectedBool)) {
            return false;
        }
    }

    return true;
}

std::string ConfigData::SerializeConfig(bool FullConfig) {
    SerializeKeybinds(*this);

    ini::IniFile Ini;
    if (FullConfig) {
        StructureToIni(Ini, *this, *this, true);
    } else {
        ConfigData Default = {};
        StructureToIni(Ini, *this, Default, false);
    }

    std::ostringstream ss;
    Ini.encode(ss);
    return base64_encode(ss.str());
}

bool ConfigData::DeserializeConfig(const std::string& Data) {
    try {
        ini::IniFile Ini;
        Ini.decode(base64_decode(Data));

        ConfigData NewConfig = {};
        if (IniToStructure(Ini, "", NewConfig) && DeserializeKeybinds(NewConfig)) {
            LOG(LOG_INFO, "%s", NewConfig.Keybinds.KeybindData.c_str());
            *this = NewConfig;
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

} // namespace Config
