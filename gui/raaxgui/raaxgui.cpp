#include "raaxgui.h"
#include "raaxgui_impl.h"
#include "raaxgui_element.h"
#include "raaxgui_globals.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace RaaxGUI {
namespace Impl {

static void HandleInput() {
    if (WasKeyJustPressed(KeyID::LeftMouseButton)) {
        g_Input.Clicking = true;

        RaaxElement* Elem = TracePos(g_Input.MousePos);
        if (Elem) {
            g_ClickedElem = Elem;
            g_ClickedElem->BeginClick(g_Input.MousePos);
        }
    } else if (g_Input.Clicking) {
        if (WasKeyJustReleased(KeyID::LeftMouseButton)) {
            if (g_ClickedElem) {
                g_ClickedElem->EndClick();
            }

            g_Input.Clicking = false;
            g_LastClickedElem = g_ClickedElem;
            g_ClickedElem = nullptr;
        } else if (g_ClickedElem) {
            g_ClickedElem->TickClick(g_Input.MousePos);
        }
    }
}

// --- Static Data ---------------------------------------------------

constexpr Vec2 g_DefaultWindowPos = Vec2(100.f, 100.f);
constexpr Vec2 g_DefaultWindowSize = Vec2(250.f, 250.f);

// --- Hashing Functions ---------------------------------------------

static int HashString(const wchar_t* String) {
    int Hash = 0;

    for (int i = 0; i < wcslen(String); i++) {
        Hash += String[i];
    }

    return Hash;
}
static int HashString(const char* String) {
    int Hash = 0;

    for (int i = 0; i < strlen(String); i++) {
        Hash += String[i];
    }

    return Hash;
}

// --- Private GUI Functions -----------------------------------------

static void ResetWindowSeenFlags() {
    for (const auto& [_, Window] : g_Windows) {
        Window->SeenThisFrame = false;
    }
}

static void RemoveUnseenWindows() {
    for (auto It = g_Windows.begin(); It != g_Windows.end();) {
        RaaxWindow* Window = It->second.get();
        if (!Window->SeenThisFrame) {
            It = g_Windows.erase(It);
        } else {
            ++It;
        }
    }
}

static RaaxWindow* FindOrCreateWindow(int Id, const char* Name) {
    if (!g_Windows.contains(Id)) {
        g_Windows[Id] = std::make_unique<RaaxWindow>(Name, g_DefaultWindowPos, g_DefaultWindowSize);
    }
    return g_Windows.at(Id).get();
}

} // namespace Impl

// --- Public GUI Functions ------------------------------------------

void SetNextWindowSize(const Impl::Vec2& Size) {
    Impl::g_PendingNextWindowSize = true;
    Impl::g_NextWindowSize = Size;
}

bool Begin(const char* Name, bool* pOpen) {
    int               Id = Impl::HashString(Name);
    Impl::RaaxWindow* Window = Impl::FindOrCreateWindow(Id, Name);

    bool Open = pOpen ? *pOpen : true;
    Window->SetOpenState(Open);
    Window->SeenThisFrame = true;

    if (Impl::g_PendingNextWindowSize) {
        Window->SetSize(Impl::g_NextWindowSize);
        Impl::g_PendingNextWindowSize = false;
    }

    Impl::g_CurrentWindow = Window;
    return Open;
}

void End() {
    Impl::g_CurrentWindow = nullptr;
}

void SameLine() {
    Impl::g_SameLine = true;
}

bool Button(const char* Name) {
    if (!Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to create a button whilst not in a window!");

    bool SameLine = Impl::g_SameLine;
    Impl::g_SameLine = false;

    int  Id = Impl::HashString(Name);
    auto Elem = static_cast<Impl::RaaxButton*>(Impl::g_CurrentWindow->AddButton(Id, Name));
    if (Elem) {
        Elem->SetSameLine(SameLine);
        return Elem->JustEndedClick();
    }
    return false;
}

void Checkbox(const char* Name, bool* pValue) {
    if (!Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to create a checkbox whilst not in a window!");

    bool SameLine = Impl::g_SameLine;
    Impl::g_SameLine = false;

    int  Id = Impl::HashString(Name);
    auto Elem = static_cast<Impl::RaaxCheckbox*>(Impl::g_CurrentWindow->AddCheckbox(Id, Name, pValue));
    if (Elem)
        Elem->SetSameLine(SameLine);
}

void SliderInt(const char* Name, int* pValue, int Min, int Max) {
    if (!Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to create an int slider whilst not in a window!");

    bool SameLine = Impl::g_SameLine;
    Impl::g_SameLine = false;

    int  Id = Impl::HashString(Name);
    auto Elem = static_cast<Impl::RaaxSlider<int>*>(Impl::g_CurrentWindow->AddSliderInt(Id, Name, pValue, Min, Max));
    if (Elem)
        Elem->SetSameLine(SameLine);
}

void SliderFloat(const char* Name, float* pValue, float Min, float Max) {
    if (!Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to create an float slider whilst not in a window!");

    bool SameLine = Impl::g_SameLine;
    Impl::g_SameLine = false;

    int  Id = Impl::HashString(Name);
    auto Elem =
        static_cast<Impl::RaaxSlider<float>*>(Impl::g_CurrentWindow->AddSliderFloat(Id, Name, pValue, Min, Max));
    if (Elem)
        Elem->SetSameLine(SameLine);
}

// --- Public Tick Functions -----------------------------------------

void NewFrame() {
    if (Impl::g_InFrame)
        Impl::ThrowError("Attempted to create new frame during a frame! Missing EndFrame()?");
    Impl::g_InFrame = true;

    Impl::ResetWindowSeenFlags();
}
void EndFrame() {
    if (!Impl::g_InFrame)
        Impl::ThrowError("Attempted to end a frame whilst not in a frame! Missing NewFrame()?");
    Impl::g_InFrame = false;

    if (Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to end a frame whilst in a window! Missing End()?");

    Impl::RemoveUnseenWindows();

    for (const auto& [_, Window] : Impl::g_Windows) {
        Window->Render();
    }

    Impl::g_Input.Update();
    Impl::HandleInput();
}

} // namespace RaaxGUI
