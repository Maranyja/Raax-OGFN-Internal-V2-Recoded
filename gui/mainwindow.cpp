#include "mainwindow.h"

#include <extern/imgui/imgui.h>
#define NOMINMAX
#include <config/config.h>
#include <config/keybind.h>
#include <config/config_reflection.h>
#include <gui/gui.h>
#include <cheat/sdk/Engine.h>

#ifdef _ENGINE
#include <gui/raaxgui/raaxgui.h>
#endif
#include <extern/imgui/imgui_internal.h>

#ifdef _ENGINE
#define SameLine RaaxGUI::SameLine
#define Button RaaxGUI::Button
#define Checkbox RaaxGUI::Checkbox
#define SliderFloat RaaxGUI::SliderFloat
#define SlderInt RaaxGUI::SlderInt

#define Text void(0)
#define Combo void(0)
#define ColorEdit4 void(0)

#define SetItemTooltip void(0)
#define SetClipboardText void(0)
#define EndTabItem void(0)
#define EndTabBar void(0)
#define EndDisabled void(0)
#define EndChild void(0)
#define BeginTabBar void(0)
#define BeginDisabled void(0)

#define ImVec2 SDK::FVector2D

using ImGuiChildFlags = int;
using ImGuiWindowFlags = int;
using ImGuiTabItemFlags = int;
using ImGuiInputTextFlags = int;

static bool BeginChild(const char* str_id, const ImVec2& size_arg, ImGuiChildFlags child_flags = 0,
                       ImGuiWindowFlags window_flags = 0) {
    return true;
}
static bool BeginTabItem(const char* label, bool* p_open = 0, ImGuiTabItemFlags flags = 0) {
    return true;
}
static bool InputText(const char* label, char* buf, size_t buf_size = 0) {
    return false;
}
#else
#define SameLine ImGui::SameLine
#define Button ImGui::Button
#define Checkbox ImGui::Checkbox
#define SliderFloat ImGui::SliderFloat
#define SlderInt ImGui::SlderInt
#define Text ImGui::Text
#define EndChild ImGui::EndChild
#define SetItemTooltip ImGui::SetItemTooltip
#define InputText ImGui::InputText
#define EndTabItem ImGui::EndTabItem
#define ColorEdit4 ImGui::ColorEdit4
#define Combo ImGui::Combo
#define BeginDisabled ImGui::BeginDisabled
#define EndDisabled ImGui::EndDisabled
#define SetClipboardText ImGui::SetClipboardText
#define EndTabBar ImGui::EndTabBar
#define BeginTabBar ImGui::BeginTabBar
#define BeginTabItem ImGui::BeginTabItem
#define EndTabItem ImGui::EndTabItem
#define BeginChild ImGui::BeginChild
#define EndChild ImGui::EndChild

#endif

namespace GUI {
namespace MainWindow {

#ifndef _ENGINE
// --- Keybind UI Functions ------------------------------------------

template <typename T>
static void DisplayMembersInTree(T* Instance, std::string Prefix, ConfigReflection::ConfigFieldView& OutField) {
    auto Members = ConfigReflection::DescribeMembers<T>();
    std::apply(
        [&](auto&&... Member) {
            (
                [&Member, Instance, &Prefix, &OutField] {
                    using MemberType = std::remove_cvref_t<decltype(Instance->*(Member.Ptr))>;
                    auto&       FieldValue = Instance->*(Member.Ptr);
                    std::string CurrentPath =
                        Prefix.empty() ? std::string(Member.Name) : Prefix + "." + Member.Name.data();

                    if constexpr (requires { ConfigReflection::DescribeMembers<MemberType>(); }) {
                        if (ImGui::TreeNode(Member.Name.data())) {
                            DisplayMembersInTree<MemberType>(&FieldValue, CurrentPath, OutField);
                            ImGui::TreePop();
                        }
                    } else if constexpr (std::is_same_v<MemberType, bool>) {
                        if (ImGui::Selectable(Member.Name.data(), OutField.Ptr == &FieldValue)) {
                            OutField = ConfigReflection::ConfigFieldView{
                                .Ptr = &FieldValue,
                                .Name = Member.Name,
                                .Type = ConfigReflection::TypeDescriptor<MemberType>::Name,
                                .FullPath = CurrentPath};
                        }
                    }
                }(),
                ...);
        },
        Members);
}

static void DisplayKeybindPopup(Keybind::KeybindData& CurrentKeybind, const char* PopupName, bool AllowCancelling) {
    ImGui::SetNextWindowSize(ImVec2(350.f, 350.f), ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal(PopupName, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        ImVec2 WindowSize = ImGui::GetWindowSize();
        ImVec2 ScreenSize = ImGui::GetIO().DisplaySize;
        ImVec2 Centered = ImVec2(ScreenSize.x / 2.f - WindowSize.x / 2.f, ScreenSize.y / 2.f - WindowSize.y / 2.f);
        ImGui::SetWindowPos(Centered);

        Text("Configure Keybind");
        ImGui::Separator();

        Text("Select a target");
        if (BeginChild("TargetSelection", ImVec2(0, 200), ImGuiChildFlags_Borders)) {
            DisplayMembersInTree<Config::ConfigData>(&Config::g_Config, "", CurrentKeybind.ReflectedBool);
        }
        EndChild();

        ImGui::Spacing();

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeight() * 2 -
                             ImGui::GetStyle().ItemSpacing.y * 2 - 5);
        static bool WaitingForKeybind = false;
        GUI::Keybind("New Keybind", WaitingForKeybind, CurrentKeybind.Keybind);

        bool ReadyToConfirm = CurrentKeybind.Keybind != Input::KeyID::NONE && CurrentKeybind.ReflectedBool.Ptr;

        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - 5);
        BeginDisabled(!ReadyToConfirm);
        if (Button("Confirm", ImVec2(90, 0))) {
            Config::g_Config.Keybinds.Keybinds.push_back(CurrentKeybind);
            ImGui::CloseCurrentPopup();
        }
        EndDisabled();

        SameLine();
        BeginDisabled(!AllowCancelling);
        if (Button("Cancel", ImVec2(90, 0))) {
            ImGui::CloseCurrentPopup();
        }
        EndDisabled();

        ImGui::EndPopup();
    } else {
        CurrentKeybind = {};
    }
}

static void AddKeybindButton() {
    if (Button("Add Keybind")) {
        ImGui::OpenPopup("Keybind");
    }

    static Keybind::KeybindData CurrentKeybind = {};
    DisplayKeybindPopup(CurrentKeybind, "Keybind", true);
}

static void ListKeybinds() {
    static Keybind::KeybindData CurrentKeybind = {};

    if (Button("Delete All Keybinds")) {
        Config::g_Config.Keybinds.Keybinds.clear();
    }

    static size_t EditingIdx = -1;
    static bool   bEditing = false;

    if (BeginChild("KeybindList", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        size_t PendingDeleteIdx = -1;

        if (Config::g_Config.Keybinds.Keybinds.empty()) {
            Text("No keybinds!");
        } else {
            for (size_t i = 0; i < Config::g_Config.Keybinds.Keybinds.size(); i++) {
                auto& Keybind = Config::g_Config.Keybinds.Keybinds[i];

                std::string KeybindInfoStr =
                    std::format("{} - {}", Input::GetKeyName(Keybind.Keybind), Keybind.ReflectedBool.Name.data());
                ImGui::PushID(&Keybind);
                BeginDisabled(bEditing && i != EditingIdx);

                if (Button("Delete")) {
                    PendingDeleteIdx = i;
                }
                SameLine();
                GUI::Keybind("Key", bEditing, Keybind.Keybind);
                if (bEditing && EditingIdx == -1) {
                    EditingIdx = i;
                }
                if (!bEditing && EditingIdx != -1) {
                    EditingIdx = -1;
                }
                SameLine();
                Text(Keybind.ReflectedBool.Name.data());

                EndDisabled();
                ImGui::PopID();
            }
        }

        if (PendingDeleteIdx != -1) {
            Config::g_Config.Keybinds.Keybinds.erase(Config::g_Config.Keybinds.Keybinds.begin() + PendingDeleteIdx);
        }
    }

    DisplayKeybindPopup(CurrentKeybind, "Keybind (Edit)", false);
    EndChild();
}
#endif

// --- Window Tab Functions & Data -----------------------------------

static void AimbotTab() {
    auto DisplayAimbotConfig = [](Config::ConfigData::AimbotConfig::AimbotAmmoConfig& Config) {
        Checkbox("Enabled", &Config.Enabled);
        Checkbox("Use In Trigger Bot", &Config.UseInTriggerBot);

        SliderFloat("Smoothness", &Config.Smoothness, 1.f, 20.f);
        SliderFloat("Max Distance", &Config.MaxDistance, 1.f, 500.f);

        Checkbox("Visible Check", &Config.VisibleCheck);
        Checkbox("Sticky Target", &Config.StickyTarget);

        Checkbox("Show FOV", &Config.ShowFOV);
        SliderFloat("FOV", &Config.FOV, 0.25f, 180.f);

        if (Config.DeadzoneFOV > Config.FOV)
            Config.DeadzoneFOV = Config.FOV;

        Checkbox("Use Deadzone", &Config.UseDeadzone);
        if (Config.UseDeadzone) {
            Checkbox("Show Deadzone FOV", &Config.ShowDeadzoneFOV);
            SliderFloat("Deadzone FOV", &Config.DeadzoneFOV, 0.25f, Config.FOV);
        }

        Combo("Target Selection", (int*)&Config.Selection, "Distance\000Degrees\000Combined");
        Combo("Target Bone", (int*)&Config.Bone, "Head\000Neck\000Chest\000Pelvis\000Random");

        if (Config.Bone == Config::ConfigData::TargetBone::Random)
            SliderFloat("Random Bone Refresh Rate", &Config.RandomBoneRefreshRate, 0.1f, 3.f);
    };

    auto& AimbotConfig = Config::g_Config.Aimbot;

    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        Checkbox("Split By Ammo Type", &AimbotConfig.SplitAimbotByAmmo);
        SameLine();
        Checkbox("Bullet Prediction", &AimbotConfig.BulletPrediction);
        SameLine();
        Checkbox("Show Target Line", &AimbotConfig.ShowTargetLine);

        Checkbox("Bullet TP", &AimbotConfig.BulletTP);
        SameLine();
        BeginDisabled();
        Checkbox("Silent Aim", &AimbotConfig.SilentAim);
        SetItemTooltip("Coming soon...");
        EndDisabled();

        static bool WaitingForKeybind = false;
        GUI::Keybind("Aim Keybind", WaitingForKeybind, AimbotConfig.AimbotKeybind);

        if (BeginChild("AimbotConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
            BeginTabBar("Aimbot Tab");

            // Scuffed attempt to fix flickering when toggling SplitAimbotByAmmo
            static bool AllWeaponsSelected = false;

            BeginDisabled(!AllWeaponsSelected && AimbotConfig.SplitAimbotByAmmo);
            if (BeginTabItem("All Weapons", nullptr,
                             AimbotConfig.SplitAimbotByAmmo ? ImGuiTabItemFlags_None : ImGuiTabItemFlags_SetSelected)) {
                AllWeaponsSelected = true;
                DisplayAimbotConfig(AimbotConfig.AllAimbot);
                EndTabItem();
            } else {
                AllWeaponsSelected = false;
            }
            EndDisabled();

            BeginDisabled(!AimbotConfig.SplitAimbotByAmmo);
            if (BeginTabItem("Shells", nullptr,
                             (AllWeaponsSelected && AimbotConfig.SplitAimbotByAmmo) ? ImGuiTabItemFlags_SetSelected
                                                                                    : ImGuiTabItemFlags_None)) {
                DisplayAimbotConfig(AimbotConfig.ShellsAimbot);
                EndTabItem();
            }
            if (BeginTabItem("Light")) {
                DisplayAimbotConfig(AimbotConfig.LightAimbot);
                EndTabItem();
            }
            if (BeginTabItem("Medium")) {
                DisplayAimbotConfig(AimbotConfig.MediumAimbot);
                EndTabItem();
            }
            if (BeginTabItem("Heavy")) {
                DisplayAimbotConfig(AimbotConfig.HeavyAimbot);
                EndTabItem();
            }
            if (BeginTabItem("Other")) {
                DisplayAimbotConfig(AimbotConfig.OtherAimbot);
                EndTabItem();
            }
            EndDisabled();

            EndTabBar();
        }
        EndChild();
    }
    EndChild();
}

static void TriggerBotTab() {
    auto& Config = Config::g_Config.TriggerBot;

    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        Checkbox("Enabled", &Config.Enabled);

        Checkbox("Use Keybind", &Config.UseKeybind);
        SameLine();
        static bool WaitingForKeybind = false;
        GUI::Keybind("Keybind", WaitingForKeybind, Config.Keybind);

        Checkbox("Show FOV", &Config.ShowFOV);
        SliderFloat("FOV", &Config.FOV, 0.25f, 180.f);

        SliderFloat("Max Distance", &Config.MaxDistance, 0.f, 500.f);
        SliderFloat("Fire Delay (s)", &Config.FireDelayS, 0.f, 2.f);
    }
    EndChild();
}

static void VisualsTab() {
    auto& Config = Config::g_Config.Visuals;

    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        BeginTabBar("Visuals Tab");
        if (BeginTabItem("Player")) {
            SliderFloat("Max Distance", &Config.Player.MaxDistance, 0.f, 500.f);
            SliderFloat("Font Size", &Config.Player.FontSize, 4.f, 32.f);

            Checkbox("Box", &Config.Player.Box);
            if (Config.Player.Box) {
                Combo("Box Type", (int*)&Config.Player.BoxType, "Full\000Cornered\000Full 3D");
                SliderFloat("Box Thickness", &Config.Player.BoxThickness, 1.f, 20.f);
                Checkbox("Filled Box", &Config.Player.FilledBox);
                if (Config.Player.FilledBox)
                    ColorEdit4("Filled Box Color", (float*)&Config.Player.FilledBoxColor);
            }

            Checkbox("Skeleton", &Config.Player.Skeleton);
            if (Config.Player.Skeleton)
                SliderFloat("Skeleton Thickness", &Config.Player.SkeletonThickness, 1.f, 20.f);

            Checkbox("Tracer", &Config.Player.Tracer);
            if (Config.Player.Tracer) {
                SliderFloat("Tracer Thickness", &Config.Player.TracerThickness, 1.f, 20.f);
                Combo("Tracer Start", (int*)&Config.Player.TracerStart, "Top\000Middle\000Bottom");
                Combo("Tracer End", (int*)&Config.Player.TracerEnd, "Top\000Middle\000Bottom");
            }

            Checkbox("Platform", &Config.Player.Platform);
            Checkbox("Name", &Config.Player.Name);
            Checkbox("Current Weapon", &Config.Player.CurrentWeapon);
            Checkbox("Distance", &Config.Player.Distance);

            Checkbox("OSI", &Config.Player.OSI);
            if (Config.Player.OSI) {
                Checkbox("OSI Match FOV", &Config.Player.OSIMatchFOV);
                SliderFloat("OSI FOV", &Config.Player.OSIFOV, 0.25f, 180.f);
                SliderFloat("OSI Size", &Config.Player.OSISize, 1.f, 20.f);
            }

            Checkbox("Chams", &Config.Player.Chams);
            if (Config.Player.Chams) {
                Checkbox("Chams On Self", &Config.Player.ChamsOnSelf);
                Checkbox("Chams Wireframe", &Config.Player.ChamsWireframe);
                Checkbox("Chams Through Walls", &Config.Player.ChamsThroughWalls);
                SliderFloat("Chams Emission Intensity", &Config.Player.ChamsEmissionIntensity, 0.f, 100.f);
                ColorEdit4("Chams Color", (float*)&Config.Player.ChamsColor);
            }

            EndTabItem();
        }
        if (BeginTabItem("Radar")) {
            float MaxPosition = 100.f - Config.Radar.Size;
            Config.Radar.PosX = std::min(Config.Radar.PosX, MaxPosition);
            Config.Radar.PosY = std::min(Config.Radar.PosY, MaxPosition);

            Checkbox("Enable Radar", &Config.Radar.Radar);
            Checkbox("Rotate With Camera", &Config.Radar.RotateWithCamera);
            Checkbox("Show Camera FOV", &Config.Radar.ShowCameraFOV);
            Checkbox("Show Guidelines", &Config.Radar.ShowGuidelines);
            SliderFloat("Max Distance", &Config.Radar.MaxDistance, 1.f, 500.f);
            SliderFloat("Position X", &Config.Radar.PosX, 0.f, MaxPosition);
            SliderFloat("Position Y", &Config.Radar.PosY, 0.f, MaxPosition);
            SliderFloat("Size", &Config.Radar.Size, 3.f, 100.f);
            ColorEdit4("Background Color", (float*)&Config.Radar.BackgroundColor);
            ColorEdit4("Color Visible", (float*)&Config.Radar.ColorVisible);
            ColorEdit4("Color Hidden", (float*)&Config.Radar.ColorHidden);
            EndTabItem();
        }
        if (BeginTabItem("Loot")) {
            Checkbox("Loot Text", &Config.Loot.LootText);
            if (Config.Loot.LootText) {
                Combo("Loot Minimum Tier", (int*)&Config.Loot.LootMinTier,
                      "Common\000Uncommon\000Rare\000Epic\000Legendary\000Mythic");
                Checkbox("Loot Fade Off", &Config.Loot.LootFadeOff);
                SameLine();
                SliderFloat("Loot Max Distance", &Config.Loot.LootMaxDistance, 1.f, 1000.f);
            }

            Checkbox("Chest Text", &Config.Loot.ChestText);
            if (Config.Loot.ChestText) {
                Checkbox("Chest Fade Off", &Config.Loot.ChestFadeOff);
                SameLine();
                SliderFloat("Chest Max Distance", &Config.Loot.ChestMaxDistance, 1.f, 1000.f);
            }

            Checkbox("Ammo Box Text", &Config.Loot.AmmoBoxText);
            if (Config.Loot.AmmoBoxText) {
                Checkbox("Ammo Box Fade Off", &Config.Loot.AmmoBoxFadeOff);
                SameLine();
                SliderFloat("Ammo Box Max Distance", &Config.Loot.AmmoBoxMaxDistance, 1.f, 1000.f);
            }

            Checkbox("Supply Drop Text", &Config.Loot.SupplyDropText);
            if (Config.Loot.SupplyDropText) {
                Checkbox("Supply Drop Fade Off", &Config.Loot.SupplyDropFadeOff);
                SameLine();
                SliderFloat("Supply Drop Max Distance", &Config.Loot.SupplyDropMaxDistance, 1.f, 2500.f);
            }

            Checkbox("Llama Text", &Config.Loot.LlamaText);
            if (Config.Loot.LlamaText) {
                Checkbox("Llama Fade Off", &Config.Loot.LlamaFadeOff);
                SameLine();
                SliderFloat("Llama Max Distance", &Config.Loot.LlamaMaxDistance, 1.f, 2500.f);
            }
            EndTabItem();
        }
        if (BeginTabItem("Colors")) {
            auto& ColorConfig = Config::g_Config.Color;
            ColorEdit4("Primary Color Visible", (float*)&ColorConfig.PrimaryColorVisible);
            ColorEdit4("Primary Color Hidden", (float*)&ColorConfig.PrimaryColorHidden);
            ColorEdit4("Secondary Color Visible", (float*)&ColorConfig.SecondaryColorVisible);
            ColorEdit4("Secondary Color Hidden", (float*)&ColorConfig.SecondaryColorHidden);
            EndTabItem();
        }
        EndTabBar();
    }
    EndChild();
}

static void ExploitsTab() {
    auto& Config = Config::g_Config.Exploit;

    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        BeginTabBar("Exploits Tab");
        if (BeginTabItem("Weapon")) {
            Checkbox("No Spread", &Config.NoSpread);
            SameLine();
            SliderFloat("Multiplier##0", &Config.NoSpreadAmount, 0.f, 1.f);
            Checkbox("No Recoil", &Config.NoRecoil);
            SameLine();
            SliderFloat("Multiplier##1", &Config.NoRecoilAmount, 0.f, 1.f);
            Checkbox("No Reload", &Config.NoReload);
            SameLine();
            SliderFloat("Time##2", &Config.NoReloadAmount, 0.f, 1.f);
            Checkbox("No Charge", &Config.NoCharge);
            SameLine();
            SliderFloat("Time##3", &Config.NoChargeAmount, 0.f, 1.f);

            Checkbox("Rapid Fire", &Config.RapidFire);
            static float RapidFireAmount = Config.RapidFireAmount *= 50.f;
            SameLine();
            SliderFloat("Speed##3", &RapidFireAmount, 1.f, 50.f);
            Config.RapidFireAmount = RapidFireAmount / 50.f;

            Checkbox("Damage Multiplier", &Config.DamageMultiplier);
            static float DamageMultiplierAmount = Config.DamageMultiplierAmount *= 50.f;
            SameLine();
            SliderFloat("Multiplier##4", &DamageMultiplierAmount, 1.f, 50.f);
            Config.DamageMultiplierAmount = DamageMultiplierAmount / 50.f;

            Checkbox("Fast Pickaxe", &Config.FastPickaxe);
            static float FastPickaxeSpeed = Config.FastPickaxeSpeed *= 50.f;
            SameLine();
            SliderFloat("Speed##5", &FastPickaxeSpeed, 1.f, 50.f);
            Config.FastPickaxeSpeed = FastPickaxeSpeed / 50.f;

            Checkbox("Automatic Weapons", &Config.AutomaticWeapons);
            Checkbox("Double Pump", &Config.DoublePump);
            Checkbox("ADS In Air", &Config.ADSInAir);
            EndTabItem();
        }
        if (BeginTabItem("Player")) {
            Checkbox("Zipline Fly", &Config.ZiplineFly);
            Checkbox("Auto Revive Self", &Config.AutoReviveSelf);
            Checkbox("Instant Revive", &Config.InstantRevive);

            static char InputName[1024];
            InputText("New Name", InputName, sizeof(InputName));
            if (Button("Server Change Name")) {
                std::string             InputNameStr = InputName;
                std::wstring            InputNameWStr = std::wstring(InputNameStr.begin(), InputNameStr.end());
                SDK::APlayerController* Controller = SDK::GetLocalController();
                if (Controller)
                    Controller->ServerChangeName(SDK::FString(InputNameWStr.c_str()));
            }
            EndTabItem();
        }
        EndTabBar();
    }
    EndChild();
}

#ifndef _ENGINE
static void KeybindsTab() {
    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        AddKeybindButton();
        ListKeybinds();
    }
    EndChild();
}
#endif

static void ConfigTab() {
    static char InputConfigData[8196];
    static bool InputConfigIsValid = false;
    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        if (InputText("Config Data", InputConfigData, sizeof(InputConfigData))) {
            Config::ConfigData TestConfig = {};
            InputConfigIsValid = TestConfig.DeserializeConfig(InputConfigData);
        }
        if (InputConfigData[0] == '\0') // ImGui seems to not always return true when deleting the final character
            InputConfigIsValid = false;

        if (Button("Copy Config")) {
            SetClipboardText(Config::g_Config.SerializeConfig(false).c_str());
        }
        SameLine();
        if (Button("Copy Full Config")) {
            SetClipboardText(Config::g_Config.SerializeConfig(true).c_str());
        }

        BeginDisabled(!InputConfigIsValid);
        if (Button("Load Config")) {
            Config::g_Config.DeserializeConfig(InputConfigData);
            InputConfigIsValid = false;
            InputConfigData[0] = '\00';
        }
        EndDisabled();
        SameLine();
        if (Button("Load Default Config")) {
            Config::g_Config = Config::ConfigData();
        }
    }
    EndChild();
}

static void MiscTab() {
    auto& ColorConfig = Config::g_Config.Color;

    if (BeginChild("ConfigRegion", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
        static bool WaitingForKeybind = false;
        GUI::Keybind("Menu Keybind", WaitingForKeybind, Config::g_Config.MenuKeybind);

        Text("Thank you for using my cheat! Join my Discord and star the repository!");
        Text("Made by Raax with help from Toxy, Parkie & NotTacs");
        Text("2ly.link/26uAo - github.com/raax7");
        Text("Remake Of Menu Was Made By Maslo2116");
        
    }
    EndChild();
}

// --- Public Tick Functions -----------------------------------------
static int selectedTab = 0;
void Tick() {
    if (Input::WasKeyJustReleased(Config::g_Config.MenuKeybind))
        GUI::MainWindow::g_WindowOpen = !GUI::MainWindow::g_WindowOpen;

    if (GUI::MainWindow::g_WindowOpen) {
        ImGuiStyle& style = ImGui::GetStyle();

        // Dark theme with more contrast
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);

        // Button styling
        style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

        style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f); // Red slider grab
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.20f, 0.20f, 1.00f);

        // Checkbox styling (smaller padding)
        style.FramePadding = ImVec2(4, 2); // Reduced from 6,4
        style.ItemSpacing = ImVec2(4, 2);  // Reduced spacing between items
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

        // Tab styling
        style.TabRounding = 0.0f;
        style.TabBorderSize = 0.0f;
        style.Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

        // Window settings
        style.WindowRounding = 0.0f;
        style.WindowPadding = ImVec2(0, 0);
        //Raax-OGFN-V2 | Recode
        ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
        if (ImGui::Begin("RAAX OGFN | Recode", nullptr,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize)) {

            ImGui::Columns(2, "MainColumns", false);
            ImGui::SetColumnWidth(0, 120);

            BeginChild("Tabs", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
            {
                const char* tabNames[] = { "Aimbot", "Trigger Bot", "Visuals", "Exploits", "Keybinds", "Config", "Misc" };

                for (int i = 0; i < IM_ARRAYSIZE(tabNames); i++) {
                    bool isSelected = (selectedTab == i);

                    // Custom tab button
                    ImGui::PushID(i);
                    if (Button(tabNames[i], ImVec2(-1, 35))) {
                        selectedTab = i;
                    }

                    if (isSelected) {
                        ImVec2 p_min = ImGui::GetItemRectMin();
                        ImVec2 p_max = ImGui::GetItemRectMax();
                        ImDrawList* draw_list = ImGui::GetWindowDrawList();

                       
                        ImColor left_col(180, 0, 0, 255);
                      
                        ImColor right_col(0, 0, 0, 255);

                        // Full button gradient background
                        draw_list->AddRectFilledMultiColor(
                            p_min, p_max,
                            left_col, right_col, right_col, left_col
                        );

                       
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.00f));
                        ImGui::RenderTextClipped(p_min, p_max, tabNames[i], NULL, NULL, ImVec2(0.5f, 0.5f));
                        ImGui::PopStyleColor();

                        // red underile
                        /*ImColor line_col(180, 40, 40, 255);
                        draw_list->AddLine(
                            ImVec2(p_min.x, p_max.y - 1), 
                            ImVec2(p_max.x, p_max.y - 1),  
                            line_col, 1.0f                 
                        );*/
                    }
                    else {
                        // Default rendering for unselected tabs
                        ImGui::RenderTextClipped(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
                            tabNames[i], NULL, NULL, ImVec2(0.5f, 0.5f));
                    }
                    ImGui::PopID();
                }
            }
            EndChild();

            ImGui::NextColumn();

            // Tab Content 
            BeginChild("Content", ImVec2(0, 0), false);
            {
                switch (selectedTab) {
                case 0: AimbotTab(); break;
                case 1: TriggerBotTab(); break;
                case 2: VisualsTab(); break;
                case 3: ExploitsTab(); break;
                case 4: KeybindsTab(); break;
                case 5: ConfigTab(); break;
                case 6: MiscTab(); break;
                default: break;
                }
            }
            EndChild();

            ImGui::Columns(1);
            ImGui::End();
        }

        // Cursor 
#ifndef _ENGINE
        ImGuiIO& io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 4.f, ImColor(0.f, 0.f, 0.f));
        ImGui::GetForegroundDrawList()->AddCircle(io.MousePos, 3.f, ImColor(1.f, 1.f, 1.f));
#else
        SDK::FVector2D MousePos = RaaxGUI::Impl::GetMousePos();
        Drawing::Circle(MousePos, 4.f, 12, SDK::FLinearColor::White, 3.f);
#endif
    }
}

bool g_WindowOpen = true;

} // namespace MainWindow
} // namespace GUI
