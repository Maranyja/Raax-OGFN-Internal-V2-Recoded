#include "drawing.h"
#include "fontdata.h"

#include <vector>
#include <cmath>

#include <cheat/sdk/sdk.h>
#include <utils/error.h>
#include <utils/math.h>
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

static SDK::UCanvas*            g_Canvas = nullptr;
static SDK::UFont*              g_Font = nullptr;
static bool                     g_BatchedLines = false;
static std::vector<BatchedLine> g_BatchedLinesList;

// --- Initialization & Tick Functions -------------------------------

void Init() {
    g_Canvas = SDK::GetCanvas();
    g_Font = SDK::UObject::FindObjectFast<SDK::UFont>("Roboto");
    if (!g_Font)
        Error::ThrowError("Failed to find engine font!");
}
void Tick() {
    static bool Init = false;
    if (!Init) {
        Drawing::Init();
        Init = true;
    }

    g_Canvas = SDK::GetCanvas();
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
            g_Canvas->K2_DrawLine(Line.A, Line.B, Line.OutlineThickness + Line.Thickness, Line.OutlineColor);
        }
    }
    for (const auto& Line : g_BatchedLinesList) {
        g_Canvas->K2_DrawLine(Line.A, Line.B, Line.Thickness, Line.Color);
    }

    g_BatchedLinesList.clear();
    g_BatchedLines = false;
}

// --- Drawing Functions ---------------------------------------------

void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
          const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined, float OutlineThickness,
          const SDK::FLinearColor& OutlineColor) {
    if (ScreenPositionA.X != -1.f && ScreenPositionA.Y != -1.f && ScreenPositionB.X != -1.f &&
        ScreenPositionB.Y != -1.f) {
        SDK::FLinearColor AdjustedOutlineColor = OutlineColor;
        AdjustedOutlineColor.A = RenderColor.A;

        if (g_BatchedLines)
            g_BatchedLinesList.push_back({ScreenPositionA, ScreenPositionB, RenderColor, Thickness, Outlined,
                                          OutlineThickness, AdjustedOutlineColor});
        else {
            if (Outlined)
                g_Canvas->K2_DrawLine(ScreenPositionA, ScreenPositionB, OutlineThickness + Thickness,
                                      AdjustedOutlineColor);

            g_Canvas->K2_DrawLine(ScreenPositionA, ScreenPositionB, Thickness, RenderColor);
        }
    }
}

void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          float FontSize, bool CenteredX, bool CenteredY, bool Outlined, float OutlineThickness,
          const SDK::FLinearColor& OutlineColor) {
    Text(String::NarrowToWide(RenderText).c_str(), ScreenPosition, RenderColor, FontSize, CenteredX, CenteredY,
         Outlined, OutlineThickness, OutlineColor);
}
void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          float FontSize, bool CenteredX, bool CenteredY, bool Outlined, float OutlineThickness,
          const SDK::FLinearColor& OutlineColor) {
    // unreal engine engine font size is weird idk
    float AdjustedFontSize = FontSize - (FontSize * 0.25f);

    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        int32_t OriginalFontSize = g_Font->LegacyFontSize;
        g_Font->LegacyFontSize = static_cast<int32_t>(AdjustedFontSize);
        g_Canvas->K2_DrawText(g_Font, RenderText, ScreenPosition, {1.f, 1.f}, RenderColor, false,
                              SDK::FLinearColor(0.f, 0.f, 0.f, 0.f), SDK::FVector2D(0.f, 0.f), CenteredX, CenteredY,
                              Outlined, OutlineColor);
        g_Font->LegacyFontSize = OriginalFontSize;
    }
}

void RectFilled(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                const SDK::FLinearColor& RenderColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        if (ScreenSize.X < ScreenSize.Y) {
            for (int i = 0; i < ScreenSize.X; i++) {
                SDK::FVector2D Start = SDK::FVector2D(ScreenPosition.X + i, ScreenPosition.Y);
                SDK::FVector2D End = SDK::FVector2D(Start.X, ScreenPosition.Y + ScreenSize.Y);
                Line(Start, End, RenderColor, 1.f, false);
            }
        } else {
            for (int i = 0; i < ScreenSize.Y; i++) {
                SDK::FVector2D Start = SDK::FVector2D(ScreenPosition.X, ScreenPosition.Y + i);
                SDK::FVector2D End = SDK::FVector2D(ScreenPosition.X + ScreenSize.X, Start.Y);
                Line(Start, End, RenderColor, 1.f, false);
            }
        }
    }
}

void Rect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize, const SDK::FLinearColor& RenderColor,
          float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        SDK::FVector2D TopLeft = ScreenPosition;
        SDK::FVector2D TopRight = SDK::FVector2D(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y);
        SDK::FVector2D BottomLeft = SDK::FVector2D(ScreenPosition.X, ScreenPosition.Y + ScreenSize.Y);
        SDK::FVector2D BottomRight = SDK::FVector2D(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y);

        if (Outlined) {
            float Offset = OutlineThickness / 2.f;

            SDK::FVector2D oTopLeft = SDK::FVector2D(TopLeft.X - Offset, TopLeft.Y - Offset);
            SDK::FVector2D oTopRight = SDK::FVector2D(TopRight.X + Offset, TopRight.Y - Offset);
            SDK::FVector2D oBottomLeft = SDK::FVector2D(BottomLeft.X - Offset, BottomLeft.Y + Offset);
            SDK::FVector2D oBottomRight = SDK::FVector2D(BottomRight.X + Offset, BottomRight.Y + Offset);

            Line(oTopLeft, oTopRight, OutlineColor, OutlineThickness, false);
            Line(oTopRight, oBottomRight, OutlineColor, OutlineThickness, false);
            Line(oBottomRight, oBottomLeft, OutlineColor, OutlineThickness, false);
            Line(oBottomLeft, oTopLeft, OutlineColor, OutlineThickness, false);
        }

        Line(TopLeft, TopRight, RenderColor, Thickness, false);
        Line(TopRight, BottomRight, RenderColor, Thickness, false);
        Line(BottomRight, BottomLeft, RenderColor, Thickness, false);
        Line(BottomLeft, TopLeft, RenderColor, Thickness, false);
    }
}

void CorneredRect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                  const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined, float OutlineThickness,
                  const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        const float LineW = ScreenSize.X / 4.f;
        const float LineH = ScreenSize.Y / 4.f;

        auto DrawCornerLines = [&](const SDK::FVector2D& Pos, bool Top, bool Left, const SDK::FLinearColor& Color,
                                   float Thickness) {
            if (Top && Left) {
                Line(Pos, SDK::FVector2D(Pos.X + LineW, Pos.Y), Color, Thickness, false); // top
                Line(Pos, SDK::FVector2D(Pos.X, Pos.Y + LineH), Color, Thickness, false); // left
            } else if (Top && !Left) {
                Line(Pos, SDK::FVector2D(Pos.X - LineW, Pos.Y), Color, Thickness, false); // top
                Line(Pos, SDK::FVector2D(Pos.X, Pos.Y + LineH), Color, Thickness, false); // right
            } else if (!Top && !Left) {
                Line(Pos, SDK::FVector2D(Pos.X - LineW, Pos.Y), Color, Thickness, false); // bottom
                Line(Pos, SDK::FVector2D(Pos.X, Pos.Y - LineH), Color, Thickness, false); // right
            } else if (!Top && Left) {
                Line(Pos, SDK::FVector2D(Pos.X + LineW, Pos.Y), Color, Thickness, false); // bottom
                Line(Pos, SDK::FVector2D(Pos.X, Pos.Y - LineH), Color, Thickness, false); // left
            }
        };

        SDK::FVector2D TopLeft = ScreenPosition;
        SDK::FVector2D TopRight = SDK::FVector2D(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y);
        SDK::FVector2D BottomRight = SDK::FVector2D(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y);
        SDK::FVector2D BottomLeft = SDK::FVector2D(ScreenPosition.X, ScreenPosition.Y + ScreenSize.Y);

        if (Outlined) {
            float OutlineOffset = OutlineThickness / 2.f;

            DrawCornerLines(SDK::FVector2D(TopLeft.X - OutlineOffset, TopLeft.Y - OutlineOffset), true, true,
                            OutlineColor, Thickness + OutlineThickness);
            DrawCornerLines(SDK::FVector2D(TopRight.X + OutlineOffset, TopRight.Y - OutlineOffset), true, false,
                            OutlineColor, Thickness + OutlineThickness);
            DrawCornerLines(SDK::FVector2D(BottomRight.X + OutlineOffset, BottomRight.Y + OutlineOffset), false, false,
                            OutlineColor, Thickness + OutlineThickness);
            DrawCornerLines(SDK::FVector2D(BottomLeft.X - OutlineOffset, BottomLeft.Y + OutlineOffset), false, true,
                            OutlineColor, Thickness + OutlineThickness);
        }

        DrawCornerLines(TopLeft, true, true, RenderColor, Thickness);
        DrawCornerLines(TopRight, true, false, RenderColor, Thickness);
        DrawCornerLines(BottomRight, false, false, RenderColor, Thickness);
        DrawCornerLines(BottomLeft, false, true, RenderColor, Thickness);
    }
}
void Rect3D(const SDK::FVector2D (&BoxCorners)[8], const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined,
            float OutlineThickness, const SDK::FLinearColor& OutlineColor, bool Filled,
            const SDK::FLinearColor& FilledColor) {
    if (BoxCorners[0].X == -1.f && BoxCorners[0].Y == -1.f)
        return;

    BeginBatchedLines(Outlined ? 24 : 12);

    // Bottom
    Line(BoxCorners[0], BoxCorners[1], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[1], BoxCorners[3], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[3], BoxCorners[2], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[2], BoxCorners[0], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    // Top
    Line(BoxCorners[4], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[5], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[7], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[6], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    // Sides
    Line(BoxCorners[0], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[1], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[2], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[3], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    EndBatchedLines();
}

void Circle(SDK::FVector2D ScreenPosition, float Radius, int32_t Segments, const SDK::FLinearColor& RenderColor,
            float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (Segments < 3) {
        Segments = 3;
    }

    float          AngleStep = (2.f * M_PI) / static_cast<float>(Segments);
    SDK::FVector2D PreviousPoint =
        SDK::FVector2D(Radius * cosf(0) + ScreenPosition.X, Radius * sinf(0) + ScreenPosition.Y);

    for (int SegmentCount = 1; SegmentCount <= Segments; SegmentCount++) {
        SDK::FVector2D CurrentPoint = SDK::FVector2D(Radius * cosf(AngleStep * SegmentCount) + ScreenPosition.X,
                                                     Radius * sinf(AngleStep * SegmentCount) + ScreenPosition.Y);

        Line(PreviousPoint, CurrentPoint, RenderColor, 1.0f, Outlined);
        PreviousPoint = CurrentPoint;
    }
}

void Triangle(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
              const SDK::FVector2D& ScreenPositionC, bool Filled, const SDK::FLinearColor& RenderColor, float Thickness,
              bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    auto SortVertices = [](SDK::FVector2D& a, SDK::FVector2D& b, SDK::FVector2D& c) {
        if (a.Y > b.Y)
            std::swap(a, b);
        if (a.Y > c.Y)
            std::swap(a, c);
        if (b.Y > c.Y)
            std::swap(b, c);
    };

    SDK::FVector2D v1 = ScreenPositionA;
    SDK::FVector2D v2 = ScreenPositionB;
    SDK::FVector2D v3 = ScreenPositionC;

    SortVertices(v1, v2, v3);

    float Invslope1 = (v2.X - v1.X) / (v2.Y - v1.Y);
    float Invslope2 = (v3.X - v1.X) / (v3.Y - v1.Y);

    float Curx1 = v1.X;
    float Curx2 = v1.X;

    int StartY = static_cast<int>(std::ceil(v1.Y));
    int MidY = static_cast<int>(std::ceil(v2.Y));
    int EndY = static_cast<int>(std::ceil(v3.Y));

    for (int y = StartY; y < MidY; y++) {
        Line(SDK::FVector2D(Curx1, static_cast<float>(y)), SDK::FVector2D(Curx2, static_cast<float>(y)), RenderColor,
             1.0f, false);
        Curx1 += Invslope1;
        Curx2 += Invslope2;
    }

    if (v2.Y != v3.Y) {
        float Invslope3 = (v3.X - v2.X) / (v3.Y - v2.Y);
        Curx1 = v2.X;

        for (int y = MidY; y < EndY; y++) {
            Line(SDK::FVector2D(Curx1, static_cast<float>(y)), SDK::FVector2D(Curx2, static_cast<float>(y)),
                 RenderColor, 1.0f, false);
            Curx1 += Invslope3;
            Curx2 += Invslope2;
        }
    }
}

} // namespace Drawing
