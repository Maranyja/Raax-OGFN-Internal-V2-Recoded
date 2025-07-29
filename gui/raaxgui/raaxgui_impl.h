#pragma once
#include "raaxgui_input.h"

/*
 * This header is designed so you can add your own implementation for core concepts like drawing, input & error
 * handling. You are expected to set the correct structs and fill in the empty functions.
 */

// Add your required includes here
#include <cheat/sdk/sdk.h>
#include <drawing/drawing.h>
#include <utils/error.h>
#include <cheat/core.h>

namespace RaaxGUI {
namespace Impl {

// --- Drawing Impl --------------------------------------------------

using Vec3 = SDK::FVector;
using Vec2 = SDK::FVector2D;
using Color = SDK::FLinearColor;

inline void DrawFilledRect(const Vec2& Pos, const Vec2& Size, const Color& Col) {
    Drawing::RectFilled(Pos, Size, Col);
}

inline void DrawText(const char* Text, const Vec2& Pos, const Color& Col, float FontSize, bool CenteredX,
                     bool CenteredY, bool Outlined) {
    Drawing::Text(Text, Pos, Col, FontSize, CenteredX, CenteredY, Outlined);
}

// --- Input Impl ----------------------------------------------------

inline Vec2 GetMousePos() {
    Vec2 Result;
    if (Core::g_LocalPlayerController)
        Core::g_LocalPlayerController->GetMousePosition(Result.X, Result.Y);
    return Result;
}

inline bool WasKeyJustReleased(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(KeyIDNames[(int)Key]);

    return Core::g_LocalPlayerController->WasInputKeyJustReleased(fKey);
}

inline bool WasKeyJustPressed(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(KeyIDNames[(int)Key]);

    return Core::g_LocalPlayerController->WasInputKeyJustPressed(fKey);
}

inline bool IsKeyDown(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(KeyIDNames[(int)Key]);

    return Core::g_LocalPlayerController->IsInputKeyDown(fKey);
}

// --- Error Handling Impl -------------------------------------------

[[noreturn]] inline void ThrowError(const std::string& Msg) {
    Error::ThrowError(Msg);
}

} // namespace Impl
} // namespace RaaxGUI
