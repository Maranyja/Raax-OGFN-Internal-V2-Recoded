#pragma once
#include "raaxgui_impl.h"

namespace RaaxGUI {

// --- Public GUI Functions ------------------------------------------

void SetNextWindowSize(const Impl::Vec2& Size);
bool Begin(const char* Name, bool* pOpen = nullptr);
void End();

void SameLine();

bool Button(const char* Name);
void Checkbox(const char* Name, bool* pValue);
void SliderInt(const char* Name, int* pValue, int Min, int Max);
void SliderFloat(const char* Name, float* pValue, float Min, float Max);

// --- Public Tick Functions -----------------------------------------

void NewFrame();
void EndFrame();

} // namespace RaaxGUI
