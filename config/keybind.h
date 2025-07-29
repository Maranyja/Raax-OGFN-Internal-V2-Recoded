#pragma once
#include "config_field_view.h"

#include <string>
#include <vector>

#include <extern/imgui/imgui.h>
#include <cheat/input.h>

namespace Keybind {

// --- Keybind Structures --------------------------------------------

struct KeybindData {
    ConfigReflection::ConfigFieldView ReflectedBool = {};
    Input::KeyID                      Keybind = Input::KeyID::NONE;

    std::string Serialize() const;
    bool        Deserialize(const std::string& Data);
};

// --- Public Functions ----------------------------------------------

std::string SerializeKeybinds(std::vector<KeybindData>& Input);
bool        DeserializeKeybinds(const std::string& Data, std::vector<KeybindData>& Output);

void Tick();

} // namespace Keybind
