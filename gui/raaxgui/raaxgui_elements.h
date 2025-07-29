#pragma once
#include "raaxgui_impl.h"
#include "raaxgui_element.h"
#include "raaxgui_window.h"

namespace RaaxGUI {
namespace Impl {

class RaaxButton final : public RaaxElement {
  public:
    RaaxButton(RaaxWindow* Window, const char* Name);
    ~RaaxButton() override = default;

  public:
    void Render() override;

  public:
    bool CollidesWith(const Vec2& Pos) override;
    void BeginClick(const Vec2& Pos) override {}
    void TickClick(const Vec2& Pos) override {}
    void EndClick() override { m_JustEndedClick = true; }
    Vec2 GetPos() override { return m_Pos; }

  public:
    void SetSameLine(bool SameLine) { m_SameLine = SameLine; }
    bool JustEndedClick() { return m_JustEndedClick; }

  private:
    std::string m_Name;
    bool        m_Clicked;
    bool        m_JustEndedClick;
    bool        m_SameLine;
    Vec2        m_Pos;
    Vec2        m_Size;
};

class RaaxCheckbox final : public RaaxElement {
  public:
    RaaxCheckbox(RaaxWindow* Window, const char* Name, bool* pValue);
    ~RaaxCheckbox() override = default;

  public:
    void Render() override;

  public:
    bool CollidesWith(const Vec2& Pos) override;
    void BeginClick(const Vec2& Pos) override { m_Clicked = true; }
    void TickClick(const Vec2& Pos) override {}
    void EndClick() override {
        m_Clicked = false;
        *m_pValue = !*m_pValue;
    }
    Vec2 GetPos() override { return m_Pos; }

  public:
    void SetSameLine(bool SameLine) { m_SameLine = SameLine; }
    void Update(bool* pValue);

  private:
    std::string m_Name;
    bool*       m_pValue;
    bool        m_Clicked;
    bool        m_SameLine;
    Vec2        m_Pos;
    Vec2        m_Size;
};

template <typename T> class RaaxSlider final : public RaaxElement {
  public:
    RaaxSlider(RaaxWindow* Window, const char* Name, T* pValue, T Min, T Max);
    ~RaaxSlider() override = default;

  public:
    void Render() override;

  private:
    float GetPercentageFull();
    Vec2  GetSliderPos();
    Vec2  GetSliderGrabberPos();

  public:
    bool CollidesWith(const Vec2& Pos) override;
    void BeginClick(const Vec2& Pos) override { m_Clicked = true; }
    void TickClick(const Vec2& Pos) override;
    void EndClick() override { m_Clicked = false; }
    Vec2 GetPos() override { return m_Pos; }

  public:
    void SetSameLine(bool SameLine) { m_SameLine = SameLine; }
    void Update(T* pValue, T Min, T Max);

  private:
    std::string m_Name;
    T*          m_pValue;
    T           m_Min;
    T           m_Max;
    bool        m_Clicked;
    bool        m_SameLine;
    Vec2        m_Pos;
    Vec2        m_Size;
};

} // namespace Impl
} // namespace RaaxGUI
