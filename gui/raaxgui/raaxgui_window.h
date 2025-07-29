#pragma once
#include "raaxgui_element.h"

#include <unordered_map>

namespace RaaxGUI {
namespace Impl {

class RaaxWindow final : public RaaxElement {
  public:
    RaaxWindow(const std::string& Name, const Vec2& Pos, const Vec2& Size);
    ~RaaxWindow() override = default;

  public:
    void Render() override;

  private:
    void RemoveUnseenElements();

  public:
    RaaxElement* AddButton(int Id, const char* Name);
    RaaxElement* AddCheckbox(int Id, const char* Name, bool* pValue);
    RaaxElement* AddSliderInt(int Id, const char* Name, int* pValue, int Min, int Max);
    RaaxElement* AddSliderFloat(int Id, const char* Name, float* pValue, float Min, float Max);

  public:
    Vec2 GetNextElementPos(const Vec2& ElementSize, bool SameLine);

  public:
    RaaxElement* GetCollidingElement(const Vec2& Pos);

  public:
    bool CollidesWith(const Vec2& Pos) override {
        return Pos.X >= m_Pos.X && Pos.Y >= m_Pos.Y && Pos.X < m_Pos.X + m_Size.X && Pos.Y < m_Pos.Y + m_Size.Y;
    }
    void BeginClick(const Vec2& Pos) override { m_ClickPos = Pos; }
    void TickClick(const Vec2& Pos) override {
        m_Pos.X += Pos.X - m_ClickPos.X;
        m_Pos.Y += Pos.Y - m_ClickPos.Y;
        m_ClickPos = Pos;
    }
    void EndClick() override {}
    Vec2 GetPos() override { return m_Pos; }

  public:
    void SetOpenState(bool Value) { m_Open = Value; }
    void SetSize(const Vec2& Size) { m_Size = Size; }

  private:
    Vec2 m_ClickPos;
    Vec2 m_CurrentElementPos;
    Vec2 m_LastElementSize;
    bool m_IsFirstElement;

    std::string m_Name;
    Vec2        m_Pos;
    Vec2        m_Size;
    bool        m_Open = false;

    std::vector<int>                                                       m_ElementOrder;
    std::unordered_map<int, std::pair<bool, std::unique_ptr<RaaxElement>>> m_Elements;

  public:
    bool SeenThisFrame = false;
};

} // namespace Impl
} // namespace RaaxGUI
