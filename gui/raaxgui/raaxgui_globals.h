#pragma once
#include "raaxgui_impl.h"
#include "raaxgui_elements.h"
#include "raaxgui_window.h"

#include <unordered_map>

namespace RaaxGUI {
namespace Impl {

struct InputData {
    Impl::Vec2 MousePos;
    bool       Clicking = false;

    void Update() { MousePos = GetMousePos(); }
} inline g_Input;

struct RaaxStyle {
    Vec2 ElementSpacing = Vec2(4.f, 4.f);
    Color WindowCol = Color(0.1f, 0.1f, 0.1f, 1.f);

    Color ButtonCol = Color(0.2f, 0.2f, 0.2f, 1.f);
    Color ButtonTextCol = Color(1.f, 1.f, 1.f, 1.f);
    Vec2  ButtonSize = Vec2(70.f, 50.f);
    float ButtonTextFontSize = 10.f;

    Color CheckboxCol = Color(0.2f, 0.2f, 0.2f, 1.f);
    Color CheckboxBoxCol = Color(0.2f, 0.8f, 0.2f, 1.f);
    Color CheckboxTextCol = Color(1.f, 1.f, 1.f, 1.f);
    float CheckboxSize = 20.f;
    float CheckboxBoxSize = 10.f;
    float CheckboxTextFontSize = 10.f;

    Color SliderCol = Color(0.2f, 0.2f, 0.2f, 1.f);
    Color SliderFillCol = Color(0.7f, 0.7f, 0.7f, 1.f);
    Color SliderGrabbCol = Color(0.4f, 0.4f, 0.4f, 1.f);
    Color SliderValueTextCol = Color(1.f, 1.f, 1.f, 1.f);
    Color SliderTextCol = Color(1.f, 1.f, 1.f, 1.f);
    Vec2  SliderSize = Vec2(150.f, 4.f);
    Vec2  SliderGrabberSize = Vec2(8.f, 12.f);
    float SliderValueTextFontSize = 8.f;
    float SliderTextFontSize = 8.f;
} inline g_Style;

inline bool g_PendingNextWindowSize = false;
inline Vec2 g_NextWindowSize = Vec2();
inline bool g_SameLine = false;

inline bool                                                 g_InFrame = false;
inline std::unordered_map<int, std::unique_ptr<RaaxWindow>> g_Windows;
inline RaaxElement*                                         g_LastClickedElem = nullptr;
inline RaaxElement*                                         g_ClickedElem = nullptr;

inline RaaxWindow* g_CurrentWindow = nullptr;

} // namespace Impl
} // namespace RaaxGUI
