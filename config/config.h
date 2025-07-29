#pragma once
#include "keybind.h"

#include <string>
#include <format>

#include <cheat/sdk/Basic.h>
#include <cheat/input.h>

namespace Config {

// --- Config Structs & Data -----------------------------------------

struct ConfigData {
    enum class BoxType : int { MIN = 0, Full = 0, Cornered = 1, Full3D = 2, MAX = 2 };
    enum class TracerPos : int { MIN = 0, Top = 0, Middle = 1, Bottom = 2, MAX = 2 };
    enum class Tier : int { MIN = 0, Common = 0, Uncommon = 1, Rare = 2, Epic = 3, Legendary = 4, Mythic = 5, MAX = 5 };
    enum class TargetSelection : int { MIN = 0, Distance = 0, Degrees = 1, Combined = 2, MAX = 2 };
    enum class TargetBone : int { MIN = 0, Head = 0, Neck = 1, Chest = 2, Pelvis = 3, Random = 4, MAX = 4 };

    struct VisualsConfig {
        struct PlayerConfig {
            float MaxDistance = 300.f;
            float FontSize = 16.f;

            bool    Box = false;
            BoxType BoxType = BoxType::Cornered;
            float   BoxThickness = 2.f;

            bool              FilledBox = false;
            SDK::FLinearColor FilledBoxColor = SDK::FLinearColor(0.f, 0.f, 0.f, 0.25f);

            bool  Skeleton = false;
            float SkeletonThickness = 1.f;

            bool      Tracer = false;
            float     TracerThickness = 1.f;
            TracerPos TracerStart = TracerPos::Bottom;
            TracerPos TracerEnd = TracerPos::Bottom;

            bool Platform = false;
            bool Name = false;
            bool CurrentWeapon = false;
            bool Distance = false;

            bool  OSI = false;
            bool  OSIMatchFOV = false;
            float OSIFOV = 10.f;
            float OSISize = 8.f;

            bool              Chams = false;
            bool              ChamsOnSelf = false;
            bool              ChamsWireframe = false;
            bool              ChamsThroughWalls = false;
            float             ChamsEmissionIntensity = 1.f;
            SDK::FLinearColor ChamsColor = SDK::FLinearColor(1.f, 0.f, 0.75f, 1.f);
        } Player;

        struct RadarConfig {
            bool              Radar = false;
            bool              RotateWithCamera = false;
            bool              ShowCameraFOV = false;
            bool              ShowGuidelines = false;
            float             MaxDistance = 150.f;
            float             PosX = 1.f;
            float             PosY = 1.f;
            float             Size = 10.f;
            SDK::FLinearColor BackgroundColor = SDK::FLinearColor(0.f, 0.f, 0.f, 1.f);
            SDK::FLinearColor ColorVisible = SDK::FLinearColor(1.f, 0.f, 0.f, 1.f);
            SDK::FLinearColor ColorHidden = SDK::FLinearColor(0.43f, 0.f, 0.f, 1.f);
        } Radar;

        struct LootConfig {
            bool  LootText = false;
            bool  LootFadeOff = false;
            float LootMaxDistance = 300.f;
            Tier  LootMinTier = Tier::Epic;

            bool  ChestText = false;
            bool  ChestFadeOff = false;
            float ChestMaxDistance = 300.f;

            bool  AmmoBoxText = false;
            bool  AmmoBoxFadeOff = false;
            float AmmoBoxMaxDistance = 300.f;

            bool  SupplyDropText = false;
            bool  SupplyDropFadeOff = false;
            float SupplyDropMaxDistance = 2500.f;

            bool  LlamaText = false;
            bool  LlamaFadeOff = false;
            float LlamaMaxDistance = 2500.f;
        } Loot;

    } Visuals;

    struct AimbotConfig {
        struct AimbotAmmoConfig {
            bool  Enabled = false;
            bool  UseInTriggerBot = false;
            float Smoothness = 5.f;
            float MaxDistance = 300.f;

            bool VisibleCheck = false;
            bool StickyTarget = false;

            bool  ShowFOV = false;
            float FOV = 10.f;

            bool  UseDeadzone = false;
            bool  ShowDeadzoneFOV = false;
            float DeadzoneFOV = 2.f;

            TargetSelection Selection = TargetSelection::Combined;
            TargetBone      Bone = TargetBone::Head;

            float RandomBoneRefreshRate = 1.f;
        };

        AimbotAmmoConfig ShellsAimbot = {.Enabled = false,
                                         .Smoothness = 1.5f,
                                         .MaxDistance = 25.f,

                                         .VisibleCheck = false,
                                         .StickyTarget = false,

                                         .ShowFOV = false,
                                         .FOV = 7.5f,

                                         .UseDeadzone = false,
                                         .ShowDeadzoneFOV = false,
                                         .DeadzoneFOV = 0.5f,

                                         .Selection = TargetSelection::Combined,
                                         .Bone = TargetBone::Head,

                                         .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig LightAimbot = {.Enabled = false,
                                        .Smoothness = 3.f,
                                        .MaxDistance = 75.f,

                                        .VisibleCheck = false,
                                        .StickyTarget = false,

                                        .ShowFOV = false,
                                        .FOV = 8.5f,

                                        .UseDeadzone = false,
                                        .ShowDeadzoneFOV = false,
                                        .DeadzoneFOV = 0.6f,

                                        .Selection = TargetSelection::Combined,
                                        .Bone = TargetBone::Neck,

                                        .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig MediumAimbot = {.Enabled = false,
                                         .Smoothness = 7.5f,
                                         .MaxDistance = 300.f,

                                         .VisibleCheck = false,
                                         .StickyTarget = false,

                                         .ShowFOV = false,
                                         .FOV = 10.f,

                                         .UseDeadzone = false,
                                         .ShowDeadzoneFOV = false,
                                         .DeadzoneFOV = 1.f,

                                         .Selection = TargetSelection::Combined,
                                         .Bone = TargetBone::Random,

                                         .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig HeavyAimbot = {.Enabled = false,
                                        .Smoothness = 3.f,
                                        .MaxDistance = 300.f,

                                        .VisibleCheck = false,
                                        .StickyTarget = false,

                                        .ShowFOV = false,
                                        .FOV = 10.f,

                                        .UseDeadzone = false,
                                        .ShowDeadzoneFOV = false,
                                        .DeadzoneFOV = 2.f,

                                        .Selection = TargetSelection::Combined,
                                        .Bone = TargetBone::Head,

                                        .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig OtherAimbot = {};

        AimbotAmmoConfig AllAimbot = {};

        bool         SplitAimbotByAmmo = false;
        bool         BulletPrediction = false;
        bool         ShowTargetLine = false;

        bool         BulletTP = false;
        bool         SilentAim = false;

        Input::KeyID AimbotKeybind = Input::KeyID::RightMouseButton;
    } Aimbot;

    struct TriggerBotConfig {
        bool         Enabled = false;
        bool         UseKeybind = false;
        Input::KeyID Keybind = Input::KeyID::G;
        bool         ShowFOV = false;
        float        FOV = 4.f;
        float        MaxDistance = 25.f;
        float        FireDelayS = 0.25f;
    } TriggerBot;

    struct ExploitConfig {
        bool  NoSpread = false;
        float NoSpreadAmount = 0.5f;
        bool  NoRecoil = false;
        float NoRecoilAmount = 0.5f;
        bool  NoReload = false;
        float NoReloadAmount = 0.5f;
        bool  NoCharge = false;
        float NoChargeAmount = 0.5f;

        bool  RapidFire = false;
        float RapidFireAmount = 1.f;

        bool  DamageMultiplier = false;
        float DamageMultiplierAmount = 1.f;

        bool  FastPickaxe = false;
        float FastPickaxeSpeed = 1.f;

        bool AutomaticWeapons = false;
        bool DoublePump = false;
        bool ADSInAir = false;

        bool ZiplineFly = false;

        bool AutoReviveSelf = false;
        bool InstantRevive = false;
    } Exploit;

    struct KeybindConfig {
        std::vector<Keybind::KeybindData> Keybinds;
        std::string                       KeybindData;
    } Keybinds;

    struct ColorConfig {
        SDK::FLinearColor PrimaryColorVisible = SDK::FLinearColor(1.f, 0.f, 0.f, 1.f);
        SDK::FLinearColor PrimaryColorHidden = SDK::FLinearColor(0.43f, 0.f, 0.f, 1.f);
        SDK::FLinearColor SecondaryColorVisible = SDK::FLinearColor(0.f, 0.47f, 1.f, 1.f);
        SDK::FLinearColor SecondaryColorHidden = SDK::FLinearColor(0.f, 0.17f, 0.34f, 1.f);
    } Color;

    Input::KeyID MenuKeybind = Input::KeyID::Insert;

    std::string SerializeConfig(bool FullConfig);
    bool        DeserializeConfig(const std::string& Data);
};

inline ConfigData g_Config;

} // namespace Config

static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::BoxType>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TracerPos>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::Tier>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TargetSelection>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TargetBone>, int>);
