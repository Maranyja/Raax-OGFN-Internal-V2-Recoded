#pragma once
#include <cheat/sdk/Basic.h>

namespace Drawing {

// --- Initialization & Tick Functions -------------------------------

void Init();
void Tick();

// --- Drawing Utility Functions -------------------------------------

void BeginBatchedLines(size_t ReserveCount = 0);
void EndBatchedLines();

// --- Drawing Functions ---------------------------------------------

void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
          const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f, bool Outlined = true,
          float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition,
          const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float FontSize = 12.f, bool CenteredX = true,
          bool CenteredY = true, bool Outlined = true, float OutlineThickness = 1.f,
          const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition,
          const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float FontSize = 12.f, bool CenteredX = true,
          bool CenteredY = true, bool Outlined = true, float OutlineThickness = 1.f,
          const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

void RectFilled(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                const SDK::FLinearColor& RenderColor);
void Rect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
          const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f, bool Outlined = true,
          float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
void CorneredRect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                  const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f,
                  bool Outlined = true, float OutlineThickness = 1.f,
                  const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
void Rect3D(const SDK::FVector2D (&BoxCorners)[8], const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
            float Thickness = 1.f, bool Outlined = true, float OutlineThickness = 1.f,
            const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black, bool Filled = false,
            const SDK::FLinearColor& FilledColor = SDK::FLinearColor::Black);

void Circle(SDK::FVector2D ScreenPosition, float Radius, int32_t Segments,
            const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f,
            bool Outlined = true, float OutlineThickness = 1.f,
            const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

void Triangle(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
              const SDK::FVector2D& ScreenPositionC, bool Filled,
              const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f,
              bool Outlined = true, float OutlineThickness = 1.f,
              const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

} // namespace Drawing
