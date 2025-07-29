#include "raaxgui_element.h"
#include "raaxgui_globals.h"

#include <algorithm>
#include <string>
#include <format>

namespace RaaxGUI {
namespace Impl {

RaaxButton::RaaxButton(RaaxWindow* Window, const char* Name)
    : RaaxElement(Window), m_Name(Name), m_Clicked(false), m_JustEndedClick(false), m_SameLine(false),
      m_Size(g_Style.ButtonSize) {}

void RaaxButton::Render() {
    m_JustEndedClick = false;

    RaaxWindow* ParentWindow = static_cast<RaaxWindow*>(m_Parent);
    m_Pos = ParentWindow->GetNextElementPos(m_Size, m_SameLine);

    DrawFilledRect(m_Pos, m_Size, g_Style.ButtonCol);

    Vec2 TextPos = Vec2(m_Pos.X + (m_Size.X / 2.f), m_Pos.Y + (m_Size.Y / 2.f));
    DrawText(m_Name.c_str(), TextPos, g_Style.ButtonTextCol, g_Style.ButtonTextFontSize, true, true, true);
}

RaaxCheckbox::RaaxCheckbox(RaaxWindow* Window, const char* Name, bool* pValue)
    : RaaxElement(Window), m_Name(Name), m_pValue(pValue), m_Clicked(false), m_SameLine(false),
      m_Size(Vec2(g_Style.CheckboxSize, g_Style.CheckboxSize)) {}

void RaaxCheckbox::Render() {
    RaaxWindow* ParentWindow = static_cast<RaaxWindow*>(m_Parent);
    m_Pos = ParentWindow->GetNextElementPos(m_Size, m_SameLine);

    DrawFilledRect(m_Pos, m_Size, g_Style.CheckboxCol);

    Vec2 BoxPos = Vec2(m_Pos.X + g_Style.CheckboxBoxSize / 2.f, m_Pos.Y + g_Style.CheckboxBoxSize / 2.f);
    Vec2 BoxSize = Vec2(g_Style.CheckboxBoxSize, g_Style.CheckboxBoxSize);
    if (*m_pValue) {
        DrawFilledRect(BoxPos, BoxSize, g_Style.CheckboxBoxCol);
    }

    Vec2 TextPos = Vec2(BoxPos.X + m_Size.X, BoxPos.Y);
    DrawText(m_Name.c_str(), TextPos, g_Style.CheckboxTextCol, g_Style.CheckboxTextFontSize, false, false, true);
}

bool RaaxButton::CollidesWith(const Vec2& Pos) {
    Vec2 TopLeft = m_Pos;
    Vec2 BottomRight = m_Pos + m_Size;
    return Pos.X >= TopLeft.X && Pos.Y >= TopLeft.Y && Pos.X < BottomRight.X && Pos.Y < BottomRight.Y;
}

bool RaaxCheckbox::CollidesWith(const Vec2& Pos) {
    Vec2 TopLeft = m_Pos;
    Vec2 BottomRight = m_Pos + m_Size;
    return Pos.X >= TopLeft.X && Pos.Y >= TopLeft.Y && Pos.X < BottomRight.X && Pos.Y < BottomRight.Y;
}

void RaaxCheckbox::Update(bool* pValue) {
    m_pValue = pValue;
}

template <typename T>
RaaxSlider<T>::RaaxSlider(RaaxWindow* Window, const char* Name, T* pValue, T Min, T Max)
    : RaaxElement(Window), m_Name(Name), m_pValue(pValue), m_Clicked(false), m_SameLine(false),
      m_Size(Vec2(std::max(g_Style.SliderSize.X, g_Style.SliderGrabberSize.X),
                  std::max(g_Style.SliderSize.Y, g_Style.SliderGrabberSize.Y) + g_Style.SliderTextFontSize)),
      m_Min(Min), m_Max(Max) {}

template <typename T> void RaaxSlider<T>::Render() {
    RaaxWindow* ParentWindow = static_cast<RaaxWindow*>(m_Parent);
    m_Pos = ParentWindow->GetNextElementPos(m_Size, m_SameLine);

    float PercentageFull = GetPercentageFull();
    Vec2  SliderPos = GetSliderPos();
    Vec2  SliderGrabberPos = GetSliderGrabberPos();
    Vec2  ValueTextPos = Vec2(SliderGrabberPos.X, SliderGrabberPos.Y + g_Style.SliderGrabberSize.Y);
    Vec2  TextPos = Vec2(SliderPos.X + g_Style.SliderSize.X, SliderPos.Y);
    Vec2  FilledSize = Vec2(g_Style.SliderSize.X * PercentageFull, g_Style.SliderSize.Y);

    std::string ValueText;
    if constexpr (std::is_floating_point_v<T>) {
        ValueText = std::format("{:.1f}", *m_pValue);
    } else {
        ValueText = std::format("{}", *m_pValue);
    }

    DrawFilledRect(SliderPos, g_Style.SliderSize, g_Style.SliderCol);
    DrawFilledRect(SliderPos, FilledSize, g_Style.SliderFillCol);
    DrawFilledRect(SliderGrabberPos, g_Style.SliderGrabberSize, g_Style.SliderGrabbCol);
    DrawText(ValueText.c_str(), ValueTextPos, g_Style.SliderValueTextCol, g_Style.SliderValueTextFontSize, true, false,
             true);
    DrawText(m_Name.c_str(), TextPos, g_Style.SliderTextCol, g_Style.SliderTextFontSize, false, false, true);
}

template <typename T> float RaaxSlider<T>::GetPercentageFull() {
    return std::clamp((static_cast<float>(*m_pValue - m_Min) / static_cast<float>(m_Max - m_Min)), 0.f, 1.f);
}
template <typename T> Vec2 RaaxSlider<T>::GetSliderPos() {
    return Vec2(m_Pos.X, m_Pos.Y + g_Style.SliderGrabberSize.Y / 3.f);
}
template <typename T> Vec2 RaaxSlider<T>::GetSliderGrabberPos() {
    float PercentageFull = GetPercentageFull();
    float HalfGrabber = g_Style.SliderGrabberSize.X / 2.f;
    float SliderStart = m_Pos.X;
    float SliderEnd = m_Pos.X + g_Style.SliderSize.X;

    float CenterX = SliderStart + (g_Style.SliderSize.X * PercentageFull);
    CenterX = std::clamp(CenterX, SliderStart + HalfGrabber, SliderEnd - HalfGrabber);

    return Vec2(CenterX - HalfGrabber, m_Pos.Y);
}

template <typename T> bool RaaxSlider<T>::CollidesWith(const Vec2& Pos) {
    Vec2 SliderPos = GetSliderPos();
    Vec2 SliderGrabberPos = GetSliderGrabberPos();

    Vec2 TopLeft = Vec2(std::min(SliderPos.X, SliderGrabberPos.X), std::min(SliderPos.Y, SliderGrabberPos.Y));
    Vec2 BottomRight =
        Vec2(std::max(SliderPos.X + g_Style.SliderSize.X, SliderGrabberPos.X + g_Style.SliderGrabberSize.X),
             std::max(SliderPos.Y + g_Style.SliderSize.Y, SliderGrabberPos.Y + g_Style.SliderGrabberSize.Y));

    return Pos.X >= TopLeft.X && Pos.Y >= TopLeft.Y && Pos.X < BottomRight.X && Pos.Y < BottomRight.Y;
}
template <typename T> void RaaxSlider<T>::TickClick(const Vec2& Pos) {
    Vec2  SliderPos = GetSliderPos();
    float ClickOffsetX = Pos.X - SliderPos.X;

    float PercentComplete = std::clamp(ClickOffsetX / g_Style.SliderSize.X, 0.f, 1.f);
    *m_pValue = static_cast<T>(m_Min + PercentComplete * (m_Max - m_Min));
}

template <typename T> void RaaxSlider<T>::Update(T* pValue, T Min, T Max) {
    m_pValue = pValue;
    m_Min = Min;
    m_Max = Max;
}

template class RaaxSlider<int>;
template class RaaxSlider<float>;

} // namespace Impl
} // namespace RaaxGUI
