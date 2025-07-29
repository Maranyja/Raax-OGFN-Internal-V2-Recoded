#pragma once
#include "raaxgui_impl.h"

namespace RaaxGUI {
namespace Impl {

class RaaxElement {
  public:
    RaaxElement(RaaxElement* parent = nullptr) : m_Parent(parent) {}
    virtual ~RaaxElement() = default;

  protected:
    RaaxElement* m_Parent;

  public:
    virtual void Render() = 0;
    virtual bool CollidesWith(const Vec2& Pos) = 0;
    virtual void BeginClick(const Vec2& Pos) = 0;
    virtual void TickClick(const Vec2& Pos) = 0;
    virtual void EndClick() = 0;
    virtual Vec2 GetPos() = 0;
};

RaaxElement* TracePos(const Vec2& Pos);

} // namespace Impl
} // namespace RaaxGUI
