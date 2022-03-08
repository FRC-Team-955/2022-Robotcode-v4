#include <navx.h>

/**
 * @return NavX yaw counter-clockwise in radians, from -pi to pi.
 */
double Navx::GetYawRadians() { return -navx->GetYaw() * (3.141592 / 180.0); }

/**
 * @return NavX angle counter-clockwise in radians. Continouslly accumlates past
 * pi/-pi
 */
double Navx::GetAngleRadians() {
  return -navx->GetAngle() * (3.141592 / 180.0);
}

double Navx::GetRateRadians() { return -navx->GetRate() * (3.141592 / 180.0); }

/**
 * @return Rotation2d counter-clockwise in radians, from -pi to pi.
 */
frc::Rotation2d Navx::GetRotation2d() {
  return frc::Rotation2d(units::radian_t(GetYawRadians()));
}