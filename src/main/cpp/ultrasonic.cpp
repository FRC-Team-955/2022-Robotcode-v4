#include <ultrasonic.h>
using namespace frc;

double UltraSonic::SonicDistance(std::string passin = "in") {
  raw_value = ultrasonic.GetValue();

  if (passin == "in") {
    return raw_value * 0.0492;
  }
  else if (passin == "cm") {
    return raw_value * 0.125;
  }
  return 69;
}