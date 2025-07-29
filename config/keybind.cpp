#include "keybind.h"
#include "config_reflection.h"

#include <sstream>
#include <charconv>

#include <gui/mainwindow.h>

namespace Keybind {

// --- KeybindData Functions -----------------------------------------

std::string KeybindData::Serialize() const {
    std::stringstream ss;
    ss << "{\"path\":\"" << ReflectedBool.FullPath << "\",\"key\":" << static_cast<int>(Keybind) << "}";
    return ss.str();
}

bool KeybindData::Deserialize(const std::string& Data) {
    size_t PathStart = Data.find("\"path\":\"");
    if (PathStart == std::string::npos)
        return false;

    size_t PathEnd = Data.find("\"", PathStart + 8);
    if (PathEnd == std::string::npos)
        return false;

    size_t KeyStart = Data.find("\"key\":", PathEnd);
    if (KeyStart == std::string::npos)
        return false;

    int  KeyValue;
    auto Result = std::from_chars(Data.data() + KeyStart + 6, Data.data() + Data.size(), KeyValue);
    if (Result.ec != std::errc())
        return false;

    ReflectedBool.FullPath = Data.substr(PathStart + 8, PathEnd - (PathStart + 8));
    Keybind = static_cast<Input::KeyID>(KeyValue);
    return true;
}

// --- Public Functions ----------------------------------------------

std::string SerializeKeybinds(std::vector<KeybindData>& Input) {
    std::string Result = "[";
    bool        First = true;

    for (const auto& Keybind : Input) {
        if (!First) {
            Result += ",";
        }
        First = false;
        Result += Keybind.Serialize();
    }

    Result += "]";
    return Result;
}

bool DeserializeKeybinds(const std::string& Data, std::vector<KeybindData>& Output) {
    size_t ArrayStart = Data.find("[{");
    size_t ArrayEnd = Data.rfind("}]");

    if (ArrayStart == std::string::npos || ArrayEnd == std::string::npos || ArrayEnd <= ArrayStart)
        return false;

    std::string Content = Data.substr(ArrayStart, ArrayEnd - ArrayStart + 2);
    if (Content.empty() || Content.front() != '[' || Content.back() != ']')
        return false;

    std::vector<KeybindData> NewKeybinds;
    size_t                   Start = 0;

    while (Start < Content.size()) {
        size_t End = Content.find('}', Start);
        if (End == std::string::npos)
            break;

        std::string Entry = Content.substr(Start, End - Start + 1);
        KeybindData Keybind;
        if (Keybind.Deserialize(Entry)) {
            NewKeybinds.push_back(Keybind);
        }

        Start = Content.find('{', End);
        if (Start == std::string::npos)
            break;
    }

    Output = std::move(NewKeybinds);
    return true;
}

void Tick() {
    if (GUI::MainWindow::g_WindowOpen) {
        return;
    }

    for (const auto& Keybind : Config::g_Config.Keybinds.Keybinds) {
        if (Input::WasKeyJustPressed(Keybind.Keybind) && Keybind.ReflectedBool.Is<bool>()) {
            Keybind.ReflectedBool.As<bool>() = !Keybind.ReflectedBool.As<bool>();
        }
    }
}

} // namespace Keybind
