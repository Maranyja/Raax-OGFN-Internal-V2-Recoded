#include "gui.h"
#include "dx11/dx11.h"

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include <cheat/hooks.h>
#include <extern/imgui/imgui.h>
#include <extern/imgui/imgui_impl_win32.h>
#include <extern/imgui/imgui_impl_dx11.h>
#include <utils/log.h>
#include <gui/mainwindow.h>

#ifndef _ENGINE
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace GUI {

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up GUI...");
#ifndef _ENGINE
    return DX11::Init();
#else
    return true;
#endif
}

void Destroy() {
    LOG(LOG_TRACE, "Destroying GUI...");
#ifndef _ENGINE
    DX11::Destroy();
#endif
}

// --- ImGui Utility Functions ---------------------------------------

static Input::KeyID GetLastReleasedKey() {
    for (int i = (int)Input::KeyID::START; i < (int)Input::KeyID::COUNT; i++) {
        if (Input::WasKeyJustReleased((Input::KeyID)i))
            return (Input::KeyID)i;
    }

    return Input::KeyID::NONE;
}

void Keybind(const char* Str, bool& WaitingForKeybind, Input::KeyID& OutKeybind) {
#ifndef _ENGINE
    std::string KeybindStr = Str;
    KeybindStr += ": ";
    KeybindStr += Input::GetKeyName(OutKeybind);

    ImGui::BeginDisabled(WaitingForKeybind);
    bool Result = ImGui::Button(KeybindStr.c_str());
    ImGui::EndDisabled();

    if (WaitingForKeybind) {
        Input::KeyID ReleasedKey = GUI::GetLastReleasedKey();
        if (ReleasedKey != Input::KeyID::NONE) {
            WaitingForKeybind = false;
            OutKeybind = ReleasedKey;
        }
    }
    WaitingForKeybind |= Result;
#endif
}

#ifndef _ENGINE
// --- Hook Functions ------------------------------------------------

LRESULT __stdcall h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_SetupImGui) {
#ifndef _ENGINE
        // Still prevents any clashing issues with ImGui render thread and game thread collisions whilst also fixing
        // dead lock issues on some older seasons (tested on 7.40)
        {
            std::lock_guard<std::recursive_mutex> lock(g_WndProcMutex);
            if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
                return true;
        }
#endif

        if (MainWindow::g_WindowOpen) {
            // Just copied straight from ImGui. Replace later, cba now.
            switch (uMsg) {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONDBLCLK:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONDBLCLK:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
            case WM_SETCURSOR:
            case WM_INPUT:
                return 0;
            default:
                break;
            }
        }

        return o_WndProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// --- Global Variables ----------------------------------------------

std::recursive_mutex g_WndProcMutex;
bool                 g_SetupImGui = false;
#endif

} // namespace GUI
