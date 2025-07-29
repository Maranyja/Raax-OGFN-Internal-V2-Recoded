#include "drawing.h"
#include "fontdata.h"

#include <vector>

#include <extern/imgui/imgui.h>
#include <utils/string.h>

namespace Drawing {

// --- Drawing Data --------------------------------------------------

struct BatchedLine {
    SDK::FVector2D    A, B;
    SDK::FLinearColor Color;
    float             Thickness;
    bool              Outlined;
    float             OutlineThickness;
    SDK::FLinearColor OutlineColor;
};

static ImDrawList*              g_DrawList = nullptr;
static ImFont*                  g_MainFontSmall = nullptr;
static ImFont*                  g_MainFont = nullptr;
static bool                     g_BatchedLines = false;
static std::vector<BatchedLine> g_BatchedLinesList;

// --- Initialization & Tick Functions -------------------------------

void Init() {
    ImFontConfig Config;
    Config.FontDataOwnedByAtlas = false; // Ensure ImGui doesn't try to free our font data.
    g_MainFontSmall = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&g_RawFontData, sizeof(g_RawFontData), 12.0f, &Config);
    g_MainFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&g_RawFontData, sizeof(g_RawFontData), 48.0f, &Config);
    g_DrawList = ImGui::GetBackgroundDrawList();
}
void Tick() {
    g_DrawList = ImGui::GetBackgroundDrawList();
}

// --- Drawing Utility Functions -------------------------------------

void BeginBatchedLines(size_t ReserveCount) {
    g_BatchedLines = true;
    g_BatchedLinesList.reserve(ReserveCount);
}
void EndBatchedLines() {
    if (g_BatchedLinesList.empty()) {
        g_BatchedLines = false;
        return;
    }

    for (const auto& Line : g_BatchedLinesList) {
        if (Line.Outlined) {
            g_DrawList->AddLine(
                ImVec2(Line.A.X, Line.A.Y), ImVec2(Line.B.X, Line.B.Y),
                ImColor(Line.OutlineColor.R, Line.OutlineColor.G, Line.OutlineColor.B, Line.OutlineColor.A),
                Line.OutlineThickness + Line.Thickness);
        }
    }
    for (const auto& Line : g_BatchedLinesList) {
        g_DrawList->AddLine(ImVec2(Line.A.X, Line.A.Y), ImVec2(Line.B.X, Line.B.Y),
                            ImColor(Line.Color.R, Line.Color.G, Line.Color.B, Line.Color.A), Line.Thickness);
    }

    g_BatchedLinesList.clear();
    g_BatchedLines = false;
}

// --- Drawing Functions ---------------------------------------------

void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
          const SDK::FLinearColor& RenderColor, const float Thickness, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPositionA.X != -1.f && ScreenPositionA.Y != -1.f && ScreenPositionB.X != -1.f &&
        ScreenPositionB.Y != -1.f) {
        SDK::FLinearColor AdjustedOutlineColor = OutlineColor;
        AdjustedOutlineColor.A = RenderColor.A;

        if (g_BatchedLines)
            g_BatchedLinesList.push_back({ScreenPositionA, ScreenPositionB, RenderColor, Thickness, Outlined,
                                          OutlineThickness, AdjustedOutlineColor});
        else {
            if (Outlined)
                g_DrawList->AddLine(ImVec2(ScreenPositionA.X, ScreenPositionA.Y),
                                    ImVec2(ScreenPositionB.X, ScreenPositionB.Y),
                                    ImColor(AdjustedOutlineColor.R, AdjustedOutlineColor.G, AdjustedOutlineColor.B,
                                            AdjustedOutlineColor.A),
                                    OutlineThickness + Thickness);

            g_DrawList->AddLine(ImVec2(ScreenPositionA.X, ScreenPositionA.Y),
                                ImVec2(ScreenPositionB.X, ScreenPositionB.Y),
                                ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
        }
    }
}

void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          const float FontSize, const bool CenteredX, const bool CenteredY, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        ImVec2 TextPosition = ImVec2(ScreenPosition.X, ScreenPosition.Y);
        if (CenteredX || CenteredY) {
            ImVec2 TextSize = g_MainFont->CalcTextSizeA(FontSize, FLT_MAX, FLT_MAX, RenderText);

            if (CenteredX)
                TextPosition.x -= TextSize.x / 2.f;
            if (CenteredY)
                TextPosition.y -= TextSize.y / 2.f;
        }

        if (Outlined) {
            ImColor ImOutlineColor = ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, RenderColor.A);

            constexpr ImVec2 Offsets[] = {
                ImVec2(-1.f, 0),
                ImVec2(1.f, 0),
                ImVec2(0, -1.f),
                ImVec2(0, 1.f),
            };

            for (const ImVec2& Offset : Offsets) {
                ImVec2 OutlinePos = ImVec2(TextPosition.x + Offset.x, TextPosition.y + Offset.y);
                g_DrawList->AddText(g_MainFont, FontSize, OutlinePos, ImOutlineColor, RenderText);
            }
        }

        g_DrawList->AddText(g_MainFont, FontSize, TextPosition,
                            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), RenderText);
    }
}
void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          const float FontSize, const bool CenteredX, const bool CenteredY, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    Text(String::WideToNarrow(RenderText).c_str(), ScreenPosition, RenderColor, FontSize, CenteredX, CenteredY,
         Outlined, OutlineThickness, OutlineColor);
}

void RectFilled(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                const SDK::FLinearColor& RenderColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(ScreenPosition.X, ScreenPosition.Y),
            ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y),
            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), 0, 0);
    }
}

void Rect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize, const SDK::FLinearColor& RenderColor,
          float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        if (Outlined) {
            ImColor ImOutlineColor = ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, RenderColor.A);
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(ScreenPosition.X, ScreenPosition.Y),
                ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y), ImOutlineColor, 0, 0,
                Thickness + 1.f);
        }
        ImGui::GetBackgroundDrawList()->AddRect(
            ImVec2(ScreenPosition.X, ScreenPosition.Y),
            ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y),
            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), 0, 0, Thickness);
    }
}

void CorneredRect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                  const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined, float OutlineThickness,
                  const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        ImVec2 a(ScreenPosition.X, ScreenPosition.Y);                               // Top-left
        ImVec2 b(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y);                // Top-right
        ImVec2 c(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y); // Bottom-right
        ImVec2 d(ScreenPosition.X, ScreenPosition.Y + ScreenSize.Y);                // Bottom-left

        const float LineW = ScreenSize.X / 4.f;
        const float LineH = ScreenSize.Y / 4.f;

        auto DrawCorner = [&](const ImVec2& Corner, const ImVec2& Start, const ImVec2& End, ImU32 ColorU32,
                              float LineThickness) {
            g_DrawList->PathLineTo(Corner);
            g_DrawList->PathLineTo(Start);
            g_DrawList->PathLineTo(End);
            g_DrawList->PathStroke(ColorU32, 0, LineThickness);
        };

        auto DrawCorneredBox = [&](ImU32 ColorU32, float LineThickness) {
            DrawCorner(ImVec2(a.x + LineW, a.y), a, ImVec2(a.x, a.y + LineH), ColorU32, LineThickness);
            DrawCorner(ImVec2(b.x - LineW, b.y), b, ImVec2(b.x, b.y + LineH), ColorU32, LineThickness);
            DrawCorner(ImVec2(c.x, c.y - LineH), c, ImVec2(c.x - LineW, c.y), ColorU32, LineThickness);
            DrawCorner(ImVec2(d.x + LineW, d.y), d, ImVec2(d.x, d.y - LineH), ColorU32, LineThickness);
        };

        if (Outlined) {
            ImColor ImOutlineColor = ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, RenderColor.A);
            DrawCorneredBox(ImOutlineColor, Thickness + OutlineThickness);
        }

        DrawCorneredBox(ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
    }
}
void Rect3D(const SDK::FVector2D (&BoxCorners)[8], const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined,
            float OutlineThickness, const SDK::FLinearColor& OutlineColor, bool Filled,
            const SDK::FLinearColor& FilledColor) {
    if (BoxCorners[0].X == -1.f && BoxCorners[0].Y == -1.f)
        return;

    if (Filled) {
        auto DrawQuad = [&](int a, int b, int c, int d) {
            ImVec2 Points[4] = {ImVec2(BoxCorners[a].X, BoxCorners[a].Y), ImVec2(BoxCorners[b].X, BoxCorners[b].Y),
                                ImVec2(BoxCorners[c].X, BoxCorners[c].Y), ImVec2(BoxCorners[d].X, BoxCorners[d].Y)};
            if (Points[0].x != -1.f && Points[1].x != -1.f && Points[2].x != -1.f && Points[3].x != -1.f)
                g_DrawList->AddConvexPolyFilled(Points, 4,
                                                ImColor(FilledColor.R, FilledColor.G, FilledColor.B, FilledColor.A));
        };

        DrawQuad(0, 1, 3, 2); // Bottom
        DrawQuad(4, 5, 7, 6); // Top
        DrawQuad(0, 1, 5, 4); // Side
        DrawQuad(1, 3, 7, 5); // Side
        DrawQuad(3, 2, 6, 7); // Side
        DrawQuad(2, 0, 4, 6); // Side
    }

    BeginBatchedLines(Outlined ? 24 : 12);

    SDK::FLinearColor AdjustedOutlineColor = OutlineColor;
    AdjustedOutlineColor.A = RenderColor.A;

    // Bottom
    Line(BoxCorners[0], BoxCorners[1], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[1], BoxCorners[3], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[3], BoxCorners[2], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[2], BoxCorners[0], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);

    // Top
    Line(BoxCorners[4], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[5], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[7], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[6], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);

    // Sides
    Line(BoxCorners[0], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[1], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[2], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);
    Line(BoxCorners[3], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, AdjustedOutlineColor);

    EndBatchedLines();
}

void Circle(SDK::FVector2D ScreenPosition, float Radius, int32_t Segments, const SDK::FLinearColor& RenderColor,
            float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (Outlined)
        g_DrawList->AddCircle(ImVec2(ScreenPosition.X, ScreenPosition.Y), Radius,
                              ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, RenderColor.A), Segments,
                              Thickness + OutlineThickness);
    g_DrawList->AddCircle(ImVec2(ScreenPosition.X, ScreenPosition.Y), Radius,
                          ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Segments, Thickness);
}

void Triangle(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
              const SDK::FVector2D& ScreenPositionC, bool Filled, const SDK::FLinearColor& RenderColor, float Thickness,
              bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    ImVec2 Points[3] = {ImVec2(ScreenPositionA.X, ScreenPositionA.Y), ImVec2(ScreenPositionB.X, ScreenPositionB.Y),
                        ImVec2(ScreenPositionC.X, ScreenPositionC.Y)};

    if (Outlined) {
        ImGui::GetBackgroundDrawList()->AddTriangle(
            Points[0], Points[1], Points[2], ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, RenderColor.A),
            Thickness + OutlineThickness);
    }

    if (Filled) {
        g_DrawList->AddTriangleFilled(Points[0], Points[1], Points[2],
                                      ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A));
    } else {
        g_DrawList->AddTriangle(Points[0], Points[1], Points[2],
                                ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
    }
}

} // namespace Drawing
