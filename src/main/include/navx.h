#ifndef NAVX
#define NAVX

#include <AHRS.h>

class Navx {
public:
  Navx(){navx = new AHRS(frc::SPI::Port::kMXP);};
  ~Navx() {delete navx;}

  double GetYawRadians();
  double GetAngleRadians();
  double GetRateRadians();
  void Reset(double offset);
  frc::Rotation2d GetRotation2d();

private:
  AHRS *navx;
};

#endif