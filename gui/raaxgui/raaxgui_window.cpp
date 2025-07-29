#include "raaxgui_window.h"
#include "raaxgui_globals.h"
#include "raaxgui_elements.h"

namespace RaaxGUI {
namespace Impl {

RaaxWindow::RaaxWindow(const std::string& Name, const Vec2& Pos, const Vec2& Size)
    : RaaxElement(), m_IsFirstElement(false), m_Name(Name), m_Pos(Pos), m_Size(Size) {}

void RaaxWindow::Render() {
    if (!m_Open)
        return;

    DrawFilledRect(m_Pos, m_Size, g_Style.WindowCol);

    RemoveUnseenElements();

    m_IsFirstElement = true;
    for (int Id : m_ElementOrder) {
        if (m_Elements[Id].first) {
            m_Elements[Id].second->Render();
        }
    }

    m_ElementOrder.clear();
}

void RaaxWindow::RemoveUnseenElements() {
    std::erase_if(m_Elements, [](const auto& Cache) { return !Cache.second.first; });
}

RaaxElement* RaaxWindow::AddButton(int Id, const char* Name) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxButton>(this, Name));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxButton*>(It.second.get());
    It.first = true;
    if (Elem)
        m_ElementOrder.push_back(Id);

    return Elem;
}
RaaxElement* RaaxWindow::AddCheckbox(int Id, const char* Name, bool* pValue) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxCheckbox>(this, Name, pValue));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxCheckbox*>(It.second.get());
    It.first = true;
    Elem->Update(pValue);
    if (Elem)
        m_ElementOrder.push_back(Id);

    return Elem;
}
RaaxElement* RaaxWindow::AddSliderInt(int Id, const char* Name, int* pValue, int Min, int Max) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxSlider<int>>(this, Name, pValue, Min, Max));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxSlider<int>*>(It.second.get());
    It.first = true;
    Elem->Update(pValue, Min, Max);
    if (Elem)
        m_ElementOrder.push_back(Id);

    return Elem;
}
RaaxElement* RaaxWindow::AddSliderFloat(int Id, const char* Name, float* pValue, float Min, float Max) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxSlider<float>>(this, Name, pValue, Min, Max));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxSlider<float>*>(It.second.get());
    It.first = true;
    Elem->Update(pValue, Min, Max);
    if (Elem)
        m_ElementOrder.push_back(Id);

    return Elem;
}

Vec2 RaaxWindow::GetNextElementPos(const Vec2& ElementSize, bool SameLine) {
    if (m_IsFirstElement) {
        m_CurrentElementPos = g_Style.ElementSpacing;

        m_IsFirstElement = false;
        m_LastElementSize = ElementSize;
        return m_CurrentElementPos + m_Pos;
    }

    if (SameLine) {
        m_CurrentElementPos.X += g_Style.ElementSpacing.X + ElementSize.X;
    } else {
        m_CurrentElementPos.X = g_Style.ElementSpacing.X;
        m_CurrentElementPos.Y += g_Style.ElementSpacing.Y + m_LastElementSize.Y;
    }

    m_LastElementSize = ElementSize;
    return m_CurrentElementPos + m_Pos;
}

RaaxElement* RaaxWindow::GetCollidingElement(const Vec2& Pos) {
    for (const auto& [_, Elem] : m_Elements) {
        if (Elem.second->CollidesWith(Pos)) {
            return Elem.second.get();
        }
    }
    return CollidesWith(Pos) ? this : nullptr;
}

} // namespace Impl
} // namespace RaaxGUI
