#include "playertick.h"

#include <array>

#include <cheat/features/weaponutils.h>
#include <cheat/cache/playercache.h>
#include <cheat/tick/pickuptick.h>
#include <cheat/sdk/sdk.h>
#include <drawing/drawing.h>
#include <config/config.h>
#include <cheat/core.h>
#include <utils/math.h>

namespace Tick {
namespace Player {

// --- Player Utility Data -------------------------------------------

constexpr std::array<std::pair<Cache::Player::BoneIdx, Cache::Player::BoneIdx>, 100> BoneConnections = {
    // Spine & Head
    std::pair(Cache::Player::BoneIdx::Head, Cache::Player::BoneIdx::Neck),
    std::pair(Cache::Player::BoneIdx::Neck, Cache::Player::BoneIdx::Chest),
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::Pelvis),

    // Left Arm
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::L_Shoulder),
    std::pair(Cache::Player::BoneIdx::L_Shoulder, Cache::Player::BoneIdx::L_Elbow),
    std::pair(Cache::Player::BoneIdx::L_Elbow, Cache::Player::BoneIdx::L_Hand),

    // Right Arm
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::R_Shoulder),
    std::pair(Cache::Player::BoneIdx::R_Shoulder, Cache::Player::BoneIdx::R_Elbow),
    std::pair(Cache::Player::BoneIdx::R_Elbow, Cache::Player::BoneIdx::R_Hand),

    // Left Leg
    std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::L_Thigh),
    std::pair(Cache::Player::BoneIdx::L_Thigh, Cache::Player::BoneIdx::L_Knee),
    std::pair(Cache::Player::BoneIdx::L_Knee, Cache::Player::BoneIdx::L_Foot),

    // Right Leg
    std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::R_Thigh),
    std::pair(Cache::Player::BoneIdx::R_Thigh, Cache::Player::BoneIdx::R_Knee),
    std::pair(Cache::Player::BoneIdx::R_Knee, Cache::Player::BoneIdx::R_Foot)};

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    Cache::Player::UpdateCache();
}

void TickRenderThread() {
    const auto& PlayerConfig = Config::g_Config.Visuals.Player;
    const auto& ColorConfig = Config::g_Config.Color;

    float FontSize = PlayerConfig.FontSize;

    for (const auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance ||
            Info.TeamIndex == Core::g_LocalTeamIndex || Info.IsDead)
            continue;

        SDK::FLinearColor PrimaryColor =
            Info.HeadVisible ? ColorConfig.PrimaryColorVisible : ColorConfig.PrimaryColorHidden;
        SDK::FLinearColor SecondaryColor =
            Info.HeadVisible ? ColorConfig.SecondaryColorVisible : ColorConfig.SecondaryColorHidden;

        if (PlayerConfig.Box) {
            if (PlayerConfig.FilledBox && (PlayerConfig.BoxType == Config::ConfigData::BoxType::Full ||
                                           PlayerConfig.BoxType == Config::ConfigData::BoxType::Cornered)) {
                Drawing::RectFilled(Info.BoundTopLeft, (Info.BoundBottomRight - Info.BoundTopLeft),
                                    PlayerConfig.FilledBoxColor);
            }

            switch (PlayerConfig.BoxType) {
            case Config::ConfigData::BoxType::Full:
                Drawing::Rect(Info.BoundTopLeft, (Info.BoundBottomRight - Info.BoundTopLeft), PrimaryColor,
                              PlayerConfig.BoxThickness);
                break;
            case Config::ConfigData::BoxType::Cornered:
                Drawing::CorneredRect(Info.BoundTopLeft, (Info.BoundBottomRight - Info.BoundTopLeft), PrimaryColor,
                                      PlayerConfig.BoxThickness);
                break;
            case Config::ConfigData::BoxType::Full3D:
                Drawing::Rect3D(Info.BoundCorners2D, PrimaryColor, PlayerConfig.BoxThickness, true, 1.f,
                                SDK::FLinearColor::Black, PlayerConfig.FilledBox, PlayerConfig.FilledBoxColor);
                break;
            }
        }

        if (PlayerConfig.Skeleton) {
            Drawing::BeginBatchedLines(BoneConnections.size());
            for (const auto& Pair : BoneConnections) {
                Drawing::Line(Info.BoneScreenPos[static_cast<int>(Pair.first)],
                              Info.BoneScreenPos[static_cast<int>(Pair.second)], SecondaryColor,
                              PlayerConfig.SkeletonThickness);
            }
            Drawing::EndBatchedLines();
        }

        if (PlayerConfig.Tracer) {
            SDK::FVector2D TracerStart, TracerEnd;

            switch (PlayerConfig.TracerStart) {
            case Config::ConfigData::TracerPos::Bottom:
                TracerStart =
                    SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX), static_cast<float>(Core::g_ScreenSizeY));
                break;
            case Config::ConfigData::TracerPos::Middle:
                TracerStart = SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX),
                                             static_cast<float>(Core::g_ScreenCenterY));
                break;
            case Config::ConfigData::TracerPos::Top:
                TracerStart = SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX), 0.f);
                break;
            }

            switch (PlayerConfig.TracerEnd) {
            case Config::ConfigData::TracerPos::Bottom:
                TracerEnd = Info.BoxBottom;
                break;
            case Config::ConfigData::TracerPos::Middle:
                TracerEnd = Info.BoxMiddle;
                break;
            case Config::ConfigData::TracerPos::Top:
                TracerEnd = Info.BoxTop;
                break;
            }

            Drawing::Line(TracerStart, TracerEnd, PrimaryColor, PlayerConfig.TracerThickness);
        }

        SDK::FVector2D TopTextPos = {Info.BoxTop.X, Info.BoxTop.Y - FontSize - 2.f};
        if (PlayerConfig.Name && !Info.PlayerName.empty()) {
            Drawing::Text(Info.PlayerName.c_str(), TopTextPos, PrimaryColor, FontSize, true, false);
            TopTextPos.Y -= 2.f + FontSize;
        }
        if (PlayerConfig.Platform && !Info.Platform.empty()) {
            Drawing::Text(Info.Platform.c_str(), TopTextPos, PrimaryColor, FontSize, true, false);
            TopTextPos.Y -= 2.f + FontSize;
        }

        SDK::FVector2D BottomTextPos = {Info.BoxBottom.X, Info.BoxBottom.Y + 2.f};
        if (PlayerConfig.CurrentWeapon && !Info.WeaponName.empty()) {
            char Buffer[64];
            if (Info.BulletsPerClip) {
                snprintf(Buffer, sizeof(Buffer), "%s [%d/%d]", Info.WeaponName.c_str(), Info.AmmoCount,
                         Info.BulletsPerClip);
            } else {
                snprintf(Buffer, sizeof(Buffer), "%s", Info.WeaponName.c_str());
            }

            Drawing::Text(Buffer, BottomTextPos, Tick::Pickup::GetTierColor(Info.WeaponTier), FontSize, true, false);
            BottomTextPos.Y += 2.f + FontSize;
        }
        if (PlayerConfig.Distance) {
            char Buffer[64];
            snprintf(Buffer, sizeof(Buffer), "%d m", static_cast<int>(Info.DistanceM));

            Drawing::Text(Buffer, BottomTextPos, PrimaryColor, FontSize, true, false);
            BottomTextPos.Y += 2.f + FontSize;
        }

        if (PlayerConfig.OSI && !Info.IsOnScreen) {
            SDK::FVector2D TipPoint, BasePoint1, BasePoint2;
            SDK::FVector   HeadPosition = Info.RootWorldLocation;
            SDK::FVector   HeadPosition2D = SDK::Project3D(HeadPosition);
            if (HeadPosition2D.Z <= 0.f) {
                HeadPosition2D.X *= -1.f;
                HeadPosition2D.Y *= -1.f;
                HeadPosition2D.X += Core::g_ScreenSizeX;
                HeadPosition2D.Y += Core::g_ScreenSizeY;
            }

            float CurrentFOV = 0.f;
            if (PlayerConfig.OSIMatchFOV) {
                CurrentFOV = Features::WeaponUtils::GetState().Config.FOV;
            } else {
                CurrentFOV = PlayerConfig.OSIFOV;
            }

            if (!CurrentFOV) {
                CurrentFOV = PlayerConfig.OSIFOV;
            }

            float          Radius = CurrentFOV * Core::g_PixelsPerDegree;
            SDK::FVector2D DirectionToPlayer =
                SDK::FVector2D(HeadPosition2D.X - Core::g_ScreenCenterX, HeadPosition2D.Y - Core::g_ScreenCenterY);
            float Magnitude =
                std::sqrt(DirectionToPlayer.X * DirectionToPlayer.X + DirectionToPlayer.Y * DirectionToPlayer.Y);
            DirectionToPlayer = DirectionToPlayer / Magnitude;

            float          Angle = std::atan2(DirectionToPlayer.Y, DirectionToPlayer.X);
            SDK::FVector2D IndicatorPosition = {Core::g_ScreenCenterX + std::cos(Angle) * Radius,
                                                Core::g_ScreenCenterY + std::sin(Angle) * Radius};

            TipPoint = IndicatorPosition + DirectionToPlayer * PlayerConfig.OSISize;
            BasePoint1 =
                IndicatorPosition + SDK::FVector2D(-DirectionToPlayer.Y, DirectionToPlayer.X) * PlayerConfig.OSISize;
            BasePoint2 =
                IndicatorPosition - SDK::FVector2D(-DirectionToPlayer.Y, DirectionToPlayer.X) * PlayerConfig.OSISize;

            Drawing::Triangle(BasePoint1, BasePoint2, TipPoint, true, PrimaryColor);
        }
    }
}

} // namespace Player
} // namespace Tick
