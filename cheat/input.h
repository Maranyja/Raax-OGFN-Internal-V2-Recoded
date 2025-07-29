#pragma once
#include <gui/raaxgui/raaxgui_input.h>
#include <cheat/core.h>

#ifndef _ENGINE
#include <extern/imgui/imgui.h>
#endif

namespace Input {

using KeyID = RaaxGUI::Impl::KeyID;

const char* GetKeyName(Input::KeyID Key);

bool WasKeyJustReleased(KeyID Key);
bool WasKeyJustPressed(KeyID Key);
bool IsKeyDown(KeyID Key);

} // namespace Input
