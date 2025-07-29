#pragma once
#include <tuple>
#include <string>
#include <vector>

#include "config.h"
#include "config_field_view.h"

namespace ConfigReflection {

// --- Member Describers ---------------------------------------------

template <typename T> constexpr auto DescribeMembers();

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig::PlayerConfig>() {
    using T = Config::ConfigData::VisualsConfig::PlayerConfig;
    return std::make_tuple(
        MemberDescriptor<T, float>{"MaxDistance", &T::MaxDistance, {}}, MemberDescriptor<T, bool>{"Box", &T::Box, {}},
        MemberDescriptor<T, float>{"FontSize", &T::FontSize, {}},
        MemberDescriptor<T, Config::ConfigData::BoxType>{"BoxType", &T::BoxType, {}},
        MemberDescriptor<T, float>{"BoxThickness", &T::BoxThickness, {}},
        MemberDescriptor<T, bool>{"FilledBox", &T::FilledBox, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"FilledBoxColor", &T::FilledBoxColor, {}},
        MemberDescriptor<T, bool>{"Skeleton", &T::Skeleton, {}},
        MemberDescriptor<T, float>{"SkeletonThickness", &T::SkeletonThickness, {}},
        MemberDescriptor<T, bool>{"Tracer", &T::Tracer, {}},
        MemberDescriptor<T, float>{"TracerThickness", &T::TracerThickness, {}},
        MemberDescriptor<T, Config::ConfigData::TracerPos>{"TracerStart", &T::TracerStart, {}},
        MemberDescriptor<T, Config::ConfigData::TracerPos>{"TracerEnd", &T::TracerEnd, {}},
        MemberDescriptor<T, bool>{"Platform", &T::Platform, {}}, MemberDescriptor<T, bool>{"Name", &T::Name, {}},
        MemberDescriptor<T, bool>{"CurrentWeapon", &T::CurrentWeapon, {}},
        MemberDescriptor<T, bool>{"Distance", &T::Distance, {}}, MemberDescriptor<T, bool>{"OSI", &T::OSI, {}},
        MemberDescriptor<T, bool>{"OSIMatchFOV", &T::OSIMatchFOV, {}},
        MemberDescriptor<T, float>{"OSIFOV", &T::OSIFOV, {}}, MemberDescriptor<T, float>{"OSISize", &T::OSISize, {}},
        MemberDescriptor<T, bool>{"Chams", &T::Chams, {}},
        MemberDescriptor<T, bool>{"ChamsOnSelf", &T::ChamsOnSelf, {}},
        MemberDescriptor<T, bool>{"ChamsWireframe", &T::ChamsWireframe, {}},
        MemberDescriptor<T, bool>{"ChamsThroughWalls", &T::ChamsThroughWalls, {}},
        MemberDescriptor<T, float>{"ChamsEmissionIntensity", &T::ChamsEmissionIntensity, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"ChamsColor", &T::ChamsColor, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig::RadarConfig>() {
    using T = Config::ConfigData::VisualsConfig::RadarConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"Radar", &T::Radar, {}},
                           MemberDescriptor<T, bool>{"RotateWithCamera", &T::RotateWithCamera, {}},
                           MemberDescriptor<T, bool>{"ShowCameraFOV", &T::ShowCameraFOV, {}},
                           MemberDescriptor<T, bool>{"ShowGuidelines", &T::ShowGuidelines, {}},
                           MemberDescriptor<T, float>{"MaxDistance", &T::MaxDistance, {}},
                           MemberDescriptor<T, float>{"PosX", &T::PosX, {}},
                           MemberDescriptor<T, float>{"PosY", &T::PosY, {}},
                           MemberDescriptor<T, float>{"Size", &T::Size, {}},
                           MemberDescriptor<T, SDK::FLinearColor>{"BackgroundColor", &T::BackgroundColor, {}},
                           MemberDescriptor<T, SDK::FLinearColor>{"ColorVisible", &T::ColorVisible, {}},
                           MemberDescriptor<T, SDK::FLinearColor>{"ColorHidden", &T::ColorHidden, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig::LootConfig>() {
    using T = Config::ConfigData::VisualsConfig::LootConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"LootText", &T::LootText, {}},
                           MemberDescriptor<T, bool>{"LootFadeOff", &T::LootFadeOff, {}},
                           MemberDescriptor<T, float>{"LootMaxDistance", &T::LootMaxDistance, {}},
                           MemberDescriptor<T, Config::ConfigData::Tier>{"LootMinTier", &T::LootMinTier, {}},
                           MemberDescriptor<T, bool>{"ChestText", &T::ChestText, {}},
                           MemberDescriptor<T, bool>{"ChestFadeOff", &T::ChestFadeOff, {}},
                           MemberDescriptor<T, float>{"ChestMaxDistance", &T::ChestMaxDistance, {}},
                           MemberDescriptor<T, bool>{"AmmoBoxText", &T::AmmoBoxText, {}},
                           MemberDescriptor<T, bool>{"AmmoBoxFadeOff", &T::AmmoBoxFadeOff, {}},
                           MemberDescriptor<T, float>{"AmmoBoxMaxDistance", &T::AmmoBoxMaxDistance, {}},
                           MemberDescriptor<T, bool>{"SupplyDropText", &T::SupplyDropText, {}},
                           MemberDescriptor<T, bool>{"SupplyDropFadeOff", &T::SupplyDropFadeOff, {}},
                           MemberDescriptor<T, float>{"SupplyDropMaxDistance", &T::SupplyDropMaxDistance, {}},
                           MemberDescriptor<T, bool>{"LlamaText", &T::LlamaText, {}},
                           MemberDescriptor<T, bool>{"LlamaFadeOff", &T::LlamaFadeOff, {}},
                           MemberDescriptor<T, float>{"LlamaMaxDistance", &T::LlamaMaxDistance, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig>() {
    using T = Config::ConfigData::VisualsConfig;
    return std::make_tuple(
        MemberDescriptor<T, Config::ConfigData::VisualsConfig::PlayerConfig>{"Player", &T::Player, {}},
        MemberDescriptor<T, Config::ConfigData::VisualsConfig::RadarConfig>{"Radar", &T::Radar, {}},
        MemberDescriptor<T, Config::ConfigData::VisualsConfig::LootConfig>{"Loot", &T::Loot, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::AimbotConfig::AimbotAmmoConfig>() {
    using T = Config::ConfigData::AimbotConfig::AimbotAmmoConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"Enabled", &T::Enabled, {}},
                           MemberDescriptor<T, bool>{"UseInTriggerBot", &T::UseInTriggerBot, {}},
                           MemberDescriptor<T, float>{"Smoothness", &T::Smoothness, {}},
                           MemberDescriptor<T, float>{"MaxDistance", &T::MaxDistance, {}},
                           MemberDescriptor<T, bool>{"VisibleCheck", &T::VisibleCheck, {}},
                           MemberDescriptor<T, bool>{"StickyTarget", &T::StickyTarget, {}},
                           MemberDescriptor<T, bool>{"ShowFOV", &T::ShowFOV, {}},
                           MemberDescriptor<T, float>{"FOV", &T::FOV, {}},
                           MemberDescriptor<T, bool>{"UseDeadzone", &T::UseDeadzone, {}},
                           MemberDescriptor<T, bool>{"ShowDeadzoneFOV", &T::ShowDeadzoneFOV, {}},
                           MemberDescriptor<T, float>{"DeadzoneFOV", &T::DeadzoneFOV, {}},
                           MemberDescriptor<T, Config::ConfigData::TargetSelection>{"Selection", &T::Selection, {}},
                           MemberDescriptor<T, Config::ConfigData::TargetBone>{"Bone", &T::Bone, {}},
                           MemberDescriptor<T, float>{"RandomBoneRefreshRate", &T::RandomBoneRefreshRate, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::AimbotConfig>() {
    using T = Config::ConfigData::AimbotConfig;
    return std::make_tuple(MemberDescriptor<T, T::AimbotAmmoConfig>{"ShellsAimbot", &T::ShellsAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"LightAimbot", &T::LightAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"MediumAimbot", &T::MediumAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"HeavyAimbot", &T::HeavyAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"OtherAimbot", &T::OtherAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"AllAimbot", &T::AllAimbot, {}},
                           MemberDescriptor<T, bool>{"SplitAimbotByAmmo", &T::SplitAimbotByAmmo, {}},
                           MemberDescriptor<T, bool>{"BulletPrediction", &T::BulletPrediction, {}},
                           MemberDescriptor<T, bool>{"ShowTargetLine", &T::ShowTargetLine, {}},
                           MemberDescriptor<T, bool>{"BulletTP", &T::BulletTP, {}},
                           MemberDescriptor<T, bool>{"SilentAim", &T::SilentAim, {}},
                           MemberDescriptor<T, Input::KeyID>{"AimbotKeybind", &T::AimbotKeybind, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::TriggerBotConfig>() {
    using T = Config::ConfigData::TriggerBotConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"Enabled", &T::Enabled, {}},
                           MemberDescriptor<T, bool>{"UseKeybind", &T::UseKeybind, {}},
                           MemberDescriptor<T, Input::KeyID>{"Keybind", &T::Keybind, {}},
                           MemberDescriptor<T, bool>{"ShowFOV", &T::ShowFOV, {}},
                           MemberDescriptor<T, float>{"FOV", &T::FOV, {}},
                           MemberDescriptor<T, float>{"MaxDistance", &T::MaxDistance, {}},
                           MemberDescriptor<T, float>{"FireDelayS", &T::FireDelayS, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::ExploitConfig>() {
    using T = Config::ConfigData::ExploitConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"NoSpread", &T::NoSpread, {}},
                           MemberDescriptor<T, float>{"NoSpreadAmount", &T::NoSpreadAmount, {}},
                           MemberDescriptor<T, bool>{"NoRecoil", &T::NoRecoil, {}},
                           MemberDescriptor<T, float>{"NoRecoilAmount", &T::NoRecoilAmount, {}},
                           MemberDescriptor<T, bool>{"NoReload", &T::NoReload, {}},
                           MemberDescriptor<T, float>{"NoReloadAmount", &T::NoReloadAmount, {}},
                           MemberDescriptor<T, bool>{"NoCharge", &T::NoCharge, {}},
                           MemberDescriptor<T, float>{"NoChargeAmount", &T::NoChargeAmount, {}},
                           MemberDescriptor<T, bool>{"RapidFire", &T::RapidFire, {}},
                           MemberDescriptor<T, float>{"RapidFireAmount", &T::RapidFireAmount, {}},
                           MemberDescriptor<T, bool>{"DamageMultiplier", &T::DamageMultiplier, {}},
                           MemberDescriptor<T, float>{"DamageMultiplierAmount", &T::DamageMultiplierAmount, {}},
                           MemberDescriptor<T, bool>{"AutomaticWeapons", &T::AutomaticWeapons, {}},
                           MemberDescriptor<T, bool>{"DoublePump", &T::DoublePump, {}},
                           MemberDescriptor<T, bool>{"ADSInAir", &T::ADSInAir, {}},
                           MemberDescriptor<T, bool>{"FastPickaxe", &T::FastPickaxe, {}},
                           MemberDescriptor<T, float>{"FastPickaxeSpeed", &T::FastPickaxeSpeed, {}},
                           MemberDescriptor<T, bool>{"ZiplineFly", &T::ZiplineFly, {}},
                           MemberDescriptor<T, bool>{"AutoReviveSelf", &T::AutoReviveSelf, {}},
                           MemberDescriptor<T, bool>{"InstantRevive", &T::InstantRevive, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::KeybindConfig>() {
    using T = Config::ConfigData::KeybindConfig;
    return std::make_tuple(MemberDescriptor<T, std::string>{"KeybindData", &T::KeybindData, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::ColorConfig>() {
    using T = Config::ConfigData::ColorConfig;
    return std::make_tuple(
        MemberDescriptor<T, SDK::FLinearColor>{"PrimaryColorVisible", &T::PrimaryColorVisible, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"PrimaryColorHidden", &T::PrimaryColorHidden, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"SecondaryColorVisible", &T::SecondaryColorVisible, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"SecondaryColorHidden", &T::SecondaryColorHidden, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData>() {
    using T = Config::ConfigData;
    return std::make_tuple(MemberDescriptor<T, Config::ConfigData::VisualsConfig>{"Visuals", &T::Visuals, {}},
                           MemberDescriptor<T, Config::ConfigData::AimbotConfig>{"Aimbot", &T::Aimbot, {}},
                           MemberDescriptor<T, Config::ConfigData::TriggerBotConfig>{"TriggerBot", &T::TriggerBot, {}},
                           MemberDescriptor<T, Config::ConfigData::ExploitConfig>{"Exploit", &T::Exploit, {}},
                           MemberDescriptor<T, Config::ConfigData::KeybindConfig>{"Keybinds", &T::Keybinds, {}},
                           MemberDescriptor<T, Config::ConfigData::ColorConfig>{"Color", &T::Color, {}},
                           MemberDescriptor<T, Input::KeyID>{"MenuKeybind", &T::MenuKeybind, {}});
}

// --- Field Util Functions & Structs --------------------------------

namespace detail {
template <typename T>
inline bool FindFieldRecursive(void* ParentPtr, const std::vector<std::string>& Parts, size_t Index,
                               ConfigFieldView& OutView) {
    const auto         Members = DescribeMembers<T>();
    const std::string& TargetPart = Parts[Index];
    bool               Found = false;

    std::apply(
        [&](auto&&... m) {
            auto CheckMember = [&](const auto& Member) {
                if (Found || Member.Name != TargetPart)
                    return;

                using MemberType = std::decay_t<decltype(((T*)ParentPtr)->*(Member.Ptr))>;
                void* MemberPtr = &(static_cast<T*>(ParentPtr)->*(Member.Ptr));

                if (Index == Parts.size() - 1) {
                    OutView.Ptr = MemberPtr;
                    OutView.Name = Member.Name;
                    OutView.Type = TypeDescriptor<MemberType>::Name;
                    OutView.FullPath = "";
                    for (size_t i = 0; i <= Index; i++) {
                        OutView.FullPath += Parts[i];
                        if (i != Index)
                            OutView.FullPath += ".";
                    }
                    Found = true;
                } else {
                    if constexpr (requires { DescribeMembers<MemberType>(); }) {
                        Found = detail::FindFieldRecursive<MemberType>(MemberPtr, Parts, Index + 1, OutView);
                    }
                }
            };
            (CheckMember(m), ...);
        },
        Members);

    return Found;
}
} // namespace detail

inline bool FindFieldByPath(const std::string& Path, ConfigFieldView& OutView) {
    // Split path into parts
    std::vector<std::string> parts;
    size_t                   start = 0;
    size_t                   end = Path.find('.');

    while (end != std::string::npos) {
        parts.push_back(Path.substr(start, end - start));
        start = end + 1;
        end = Path.find('.', start);
    }
    parts.push_back(Path.substr(start));

    if (parts.empty())
        return false;

    // Start recursion with root ConfigData type
    return detail::FindFieldRecursive<Config::ConfigData>(&Config::g_Config, parts, 0, OutView);
}

} // namespace ConfigReflection
