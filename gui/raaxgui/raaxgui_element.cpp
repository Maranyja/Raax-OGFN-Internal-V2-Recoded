#include "raaxgui_element.h"
#include "raaxgui_globals.h"

namespace RaaxGUI {
namespace Impl {

RaaxElement* TracePos(const Vec2& Pos) {
    for (const auto& [_, Window] : g_Windows) {
        if (RaaxElement* Elem = Window->GetCollidingElement(Pos)) {
            return Elem;
        }
    }
    return nullptr;
}

} // namespace Impl
} // namespace RaaxGUI
