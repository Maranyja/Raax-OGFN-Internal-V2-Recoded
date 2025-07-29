#include "math.h"

#include <algorithm>
#include <numbers>

#include <cheat/core.h>

namespace Math {

void SinCos(float* Sine, float* Cosine, float AngleRad) {
    if (Sine)
        *Sine = sinf(AngleRad);
    if (Cosine)
        *Cosine = cosf(AngleRad);
}

float InvSqrt(float F) {
    // Performs two passes of Newton-Raphson iteration on the hardware estimate
    //    v^-0.5 = x
    // => x^2 = v^-1
    // => 1/(x^2) = v
    // => F(x) = x^-2 - v
    //    F'(x) = -2x^-3

    //    x1 = x0 - F(x0)/F'(x0)
    // => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
    // => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
    // => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
    //
    // This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
    // but retains better accuracy (namely InvSqrt(1) = 1 exactly).

    const __m128 fOneHalf = _mm_set_ss(0.5f);
    __m128       Y0, X0, X1, X2, FOver2;
    float        temp;

    Y0 = _mm_set_ss(F);
    X0 = _mm_rsqrt_ss(Y0); // 1/sqrt estimate (12 bits)
    FOver2 = _mm_mul_ss(Y0, fOneHalf);

    // 1st Newton-Raphson iteration
    X1 = _mm_mul_ss(X0, X0);
    X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
    X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

    // 2nd Newton-Raphson iteration
    X2 = _mm_mul_ss(X1, X1);
    X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
    X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

    _mm_store_ss(&temp, X2);
    return temp;
}

float DegreesToRadians(float Degrees) {
    return Degrees * (std::numbers::pi_v<float> / 180.0f);
}

float RadiansToDegrees(float Radians) {
    return Radians * (180.0f / std::numbers::pi_v<float>);
}

SDK::FQuat RotatorToQuat(const SDK::FRotator& Rotator) {
    const float DEG_TO_RAD = std::numbers::pi_v<float> / (180.f);
    const float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;
    float       SP, SY, SR;
    float       CP, CY, CR;

    const float PitchNoWinding = fmodf(Rotator.Pitch, 360.0f);
    const float YawNoWinding = fmodf(Rotator.Yaw, 360.0f);
    const float RollNoWinding = fmodf(Rotator.Roll, 360.0f);

    SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
    SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
    SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

    SDK::FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

SDK::FRotator FindLookAtRotation(const SDK::FVector& Start, const SDK::FVector& Target) {
    SDK::FVector Direction = Target - Start;

    float Yaw = atan2f(Direction.Y, Direction.X) * 180.0f / std::numbers::pi_v<float>;

    float HorizontalDistance = sqrtf(Direction.X * Direction.X + Direction.Y * Direction.Y);
    float Pitch = atan2f(Direction.Z, HorizontalDistance) * 180.0f / std::numbers::pi_v<float>;

    SDK::FVector RotationAngles = {Pitch, Yaw, 0.0f};
    return SDK::FRotator(RotationAngles.X, RotationAngles.Y, RotationAngles.Z);
}

SDK::FVector GetForwardVector(const SDK::FRotator& InRot) {
    float PitchNoWinding = fmodf(InRot.Pitch, 360.f);
    float YawNoWinding = fmodf(InRot.Yaw, 360.f);

    float CP, SP, CY, SY;
    SinCos(&SP, &CP, DegreesToRadians(PitchNoWinding));
    SinCos(&SY, &CY, DegreesToRadians(YawNoWinding));
    return SDK::FVector(CP * CY, CP * SY, SP);
}

float GetDegreeDistance(const SDK::FRotator& Rotator1, const SDK::FRotator& Rotator2) {
    SDK::FVector ForwardVector1 = GetForwardVector(Rotator1);
    SDK::FVector ForwardVector2 = GetForwardVector(Rotator2);

    ForwardVector1.Normalize();
    ForwardVector2.Normalize();

    float DotProduct = ForwardVector1.Dot(ForwardVector2);
    DotProduct = std::clamp(DotProduct, -1.0f, 1.0f);

    return RadiansToDegrees(acosf(DotProduct));
}

bool IsOnScreen(const SDK::FVector2D& Position) {
    bool OnScreenX = (Position.X >= 0.f) && (Position.X <= Core::g_ScreenSizeX);
    bool OnScreenY = (Position.Y >= 0.f) && (Position.Y <= Core::g_ScreenSizeY);
    return OnScreenX && OnScreenY;
}

} // namespace Math
