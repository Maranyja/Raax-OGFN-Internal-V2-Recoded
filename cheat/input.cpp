#include "input.h"

namespace Input {

const char* GetKeyName(Input::KeyID Key) {
    return RaaxGUI::Impl::KeyIDNames[(int)Key];
}

#if _ENGINE
bool WasKeyJustReleased(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(GetKeyName(Key));

    return Core::g_LocalPlayerController->WasInputKeyJustReleased(fKey);
}
bool WasKeyJustPressed(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(GetKeyName(Key));

    return Core::g_LocalPlayerController->WasInputKeyJustPressed(fKey);
}
bool IsKeyDown(KeyID Key) {
    if (!Core::g_LocalPlayerController)
        return false;

    SDK::FKey fKey = {};
    fKey.KeyName = SDK::FName(GetKeyName(Key));

    return Core::g_LocalPlayerController->IsInputKeyDown(fKey);
}
#else
static ImGuiKey KeyIDToImGuiKey(KeyID Key) {
    switch (Key) {
    case KeyID::LeftMouseButton:
        return ImGuiKey_MouseLeft;
    case KeyID::RightMouseButton:
        return ImGuiKey_MouseRight;
    case KeyID::MiddleMouseButton:
        return ImGuiKey_MouseMiddle;
    case KeyID::ThumbMouseButton:
        return ImGuiKey_MouseX1;
    case KeyID::ThumbMouseButton2:
        return ImGuiKey_MouseX2;
    case KeyID::BackSpace:
        return ImGuiKey_Backspace;
    case KeyID::Tab:
        return ImGuiKey_Tab;
    case KeyID::Enter:
        return ImGuiKey_Enter;
    case KeyID::Pause:
        return ImGuiKey_Pause;
    case KeyID::CapsLock:
        return ImGuiKey_CapsLock;
    case KeyID::Escape:
        return ImGuiKey_Escape;
    case KeyID::SpaceBar:
        return ImGuiKey_Space;
    case KeyID::PageUp:
        return ImGuiKey_PageUp;
    case KeyID::PageDown:
        return ImGuiKey_PageDown;
    case KeyID::End:
        return ImGuiKey_End;
    case KeyID::Home:
        return ImGuiKey_Home;
    case KeyID::Left:
        return ImGuiKey_LeftArrow;
    case KeyID::Up:
        return ImGuiKey_UpArrow;
    case KeyID::Right:
        return ImGuiKey_RightArrow;
    case KeyID::Down:
        return ImGuiKey_DownArrow;
    case KeyID::Insert:
        return ImGuiKey_Insert;
    case KeyID::Delete:
        return ImGuiKey_Delete;
    case KeyID::Zero:
        return ImGuiKey_0;
    case KeyID::One:
        return ImGuiKey_1;
    case KeyID::Two:
        return ImGuiKey_2;
    case KeyID::Three:
        return ImGuiKey_3;
    case KeyID::Four:
        return ImGuiKey_4;
    case KeyID::Five:
        return ImGuiKey_5;
    case KeyID::Six:
        return ImGuiKey_6;
    case KeyID::Seven:
        return ImGuiKey_7;
    case KeyID::Eight:
        return ImGuiKey_8;
    case KeyID::Nine:
        return ImGuiKey_9;
    case KeyID::A:
        return ImGuiKey_A;
    case KeyID::B:
        return ImGuiKey_B;
    case KeyID::C:
        return ImGuiKey_C;
    case KeyID::D:
        return ImGuiKey_D;
    case KeyID::E:
        return ImGuiKey_E;
    case KeyID::F:
        return ImGuiKey_F;
    case KeyID::G:
        return ImGuiKey_G;
    case KeyID::H:
        return ImGuiKey_H;
    case KeyID::I:
        return ImGuiKey_I;
    case KeyID::J:
        return ImGuiKey_J;
    case KeyID::K:
        return ImGuiKey_K;
    case KeyID::L:
        return ImGuiKey_L;
    case KeyID::M:
        return ImGuiKey_M;
    case KeyID::N:
        return ImGuiKey_N;
    case KeyID::O:
        return ImGuiKey_O;
    case KeyID::P:
        return ImGuiKey_P;
    case KeyID::Q:
        return ImGuiKey_Q;
    case KeyID::R:
        return ImGuiKey_R;
    case KeyID::S:
        return ImGuiKey_S;
    case KeyID::T:
        return ImGuiKey_T;
    case KeyID::U:
        return ImGuiKey_U;
    case KeyID::V:
        return ImGuiKey_V;
    case KeyID::W:
        return ImGuiKey_W;
    case KeyID::X:
        return ImGuiKey_X;
    case KeyID::Y:
        return ImGuiKey_Y;
    case KeyID::Z:
        return ImGuiKey_Z;
    case KeyID::NumPadZero:
        return ImGuiKey_Keypad0;
    case KeyID::NumPadOne:
        return ImGuiKey_Keypad1;
    case KeyID::NumPadTwo:
        return ImGuiKey_Keypad2;
    case KeyID::NumPadThree:
        return ImGuiKey_Keypad3;
    case KeyID::NumPadFour:
        return ImGuiKey_Keypad4;
    case KeyID::NumPadFive:
        return ImGuiKey_Keypad5;
    case KeyID::NumPadSix:
        return ImGuiKey_Keypad6;
    case KeyID::NumPadSeven:
        return ImGuiKey_Keypad7;
    case KeyID::NumPadEight:
        return ImGuiKey_Keypad8;
    case KeyID::NumPadNine:
        return ImGuiKey_Keypad9;
    case KeyID::Multiply:
        return ImGuiKey_KeypadMultiply;
    case KeyID::Add:
        return ImGuiKey_KeypadAdd;
    case KeyID::Subtract:
        return ImGuiKey_KeypadSubtract;
    case KeyID::Decimal:
        return ImGuiKey_KeypadDecimal;
    case KeyID::Divide:
        return ImGuiKey_KeypadDivide;
    case KeyID::F1:
        return ImGuiKey_F1;
    case KeyID::F2:
        return ImGuiKey_F2;
    case KeyID::F3:
        return ImGuiKey_F3;
    case KeyID::F4:
        return ImGuiKey_F4;
    case KeyID::F5:
        return ImGuiKey_F5;
    case KeyID::F6:
        return ImGuiKey_F6;
    case KeyID::F7:
        return ImGuiKey_F7;
    case KeyID::F8:
        return ImGuiKey_F8;
    case KeyID::F9:
        return ImGuiKey_F9;
    case KeyID::F10:
        return ImGuiKey_F10;
    case KeyID::F11:
        return ImGuiKey_F11;
    case KeyID::F12:
        return ImGuiKey_F12;
    case KeyID::NumLock:
        return ImGuiKey_NumLock;
    case KeyID::ScrollLock:
        return ImGuiKey_ScrollLock;
    case KeyID::LeftShift:
        return ImGuiKey_LeftShift;
    case KeyID::RightShift:
        return ImGuiKey_RightShift;
    case KeyID::LeftControl:
        return ImGuiKey_LeftCtrl;
    case KeyID::RightControl:
        return ImGuiKey_RightCtrl;
    case KeyID::LeftAlt:
        return ImGuiKey_LeftAlt;
    case KeyID::RightAlt:
        return ImGuiKey_RightAlt;
    case KeyID::LeftCommand:
        return ImGuiKey_LeftSuper;
    case KeyID::RightCommand:
        return ImGuiKey_RightSuper;
    case KeyID::Semicolon:
        return ImGuiKey_Semicolon;
    case KeyID::Equals:
        return ImGuiKey_Equal;
    case KeyID::Comma:
        return ImGuiKey_Comma;
    case KeyID::Underscore:
        return ImGuiKey_Minus;
    case KeyID::Period:
        return ImGuiKey_Period;
    case KeyID::Slash:
        return ImGuiKey_Slash;
    case KeyID::Tilde:
        return ImGuiKey_GraveAccent;
    case KeyID::LeftBracket:
        return ImGuiKey_LeftBracket;
    case KeyID::Backslash:
        return ImGuiKey_Backslash;
    case KeyID::RightBracket:
        return ImGuiKey_RightBracket;
    case KeyID::Quote:
        return ImGuiKey_Apostrophe;
    case KeyID::A_AccentGrave:
        return ImGuiKey_GraveAccent;
    case KeyID::E_AccentGrave:
        return ImGuiKey_GraveAccent;
    case KeyID::E_AccentAigu:
        return ImGuiKey_Apostrophe;
    case KeyID::C_Cedille:
        return ImGuiKey_Comma;
    case KeyID::Section:
        return ImGuiKey_GraveAccent;

    // Gamepad mappings
    case KeyID::Gamepad_FaceButton_Left:
        return ImGuiKey_GamepadFaceLeft;
    case KeyID::Gamepad_FaceButton_Right:
        return ImGuiKey_GamepadFaceRight;
    case KeyID::Gamepad_FaceButton_Top:
        return ImGuiKey_GamepadFaceUp;
    case KeyID::Gamepad_FaceButton_Bottom:
        return ImGuiKey_GamepadFaceDown;
    case KeyID::Gamepad_LeftShoulder:
        return ImGuiKey_GamepadL1;
    case KeyID::Gamepad_RightShoulder:
        return ImGuiKey_GamepadR1;
    case KeyID::Gamepad_LeftTrigger:
        return ImGuiKey_GamepadL2;
    case KeyID::Gamepad_RightTrigger:
        return ImGuiKey_GamepadR2;
    case KeyID::Gamepad_LeftThumbstick:
        return ImGuiKey_GamepadL3;
    case KeyID::Gamepad_RightThumbstick:
        return ImGuiKey_GamepadR3;
    case KeyID::Gamepad_DPad_Up:
        return ImGuiKey_GamepadDpadUp;
    case KeyID::Gamepad_DPad_Down:
        return ImGuiKey_GamepadDpadDown;
    case KeyID::Gamepad_DPad_Left:
        return ImGuiKey_GamepadDpadLeft;
    case KeyID::Gamepad_DPad_Right:
        return ImGuiKey_GamepadDpadRight;
    case KeyID::Gamepad_Special_Left:
        return ImGuiKey_GamepadBack;
    case KeyID::Gamepad_Special_Right:
        return ImGuiKey_GamepadStart;
    case KeyID::Gamepad_LeftStick_Up:
        return ImGuiKey_GamepadLStickUp;
    case KeyID::Gamepad_LeftStick_Down:
        return ImGuiKey_GamepadLStickDown;
    case KeyID::Gamepad_LeftStick_Left:
        return ImGuiKey_GamepadLStickLeft;
    case KeyID::Gamepad_LeftStick_Right:
        return ImGuiKey_GamepadLStickRight;
    case KeyID::Gamepad_RightStick_Up:
        return ImGuiKey_GamepadRStickUp;
    case KeyID::Gamepad_RightStick_Down:
        return ImGuiKey_GamepadRStickDown;
    case KeyID::Gamepad_RightStick_Left:
        return ImGuiKey_GamepadRStickLeft;
    case KeyID::Gamepad_RightStick_Right:
        return ImGuiKey_GamepadRStickRight;

    default:
        return ImGuiKey_None;
    }
}

bool WasKeyJustReleased(KeyID Key) {
    return ImGui::IsKeyReleased(KeyIDToImGuiKey(Key));
}
bool WasKeyJustPressed(KeyID Key) {
    return ImGui::IsKeyPressed(KeyIDToImGuiKey(Key));
}
bool IsKeyDown(KeyID Key) {
    return ImGui::IsKeyDown(KeyIDToImGuiKey(Key));
}
#endif

} // namespace Input
