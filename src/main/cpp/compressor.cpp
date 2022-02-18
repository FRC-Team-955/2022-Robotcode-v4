#include "compressor.h"

void RobotCompressor::TurnOnCompressor() {
  compressor.EnableDigital();
}

void RobotCompressor::TurnOffCompressor() {
  compressor.Disable();
}

bool RobotCompressor::DetectPressure() {
  if (compressor.GetPressure() <
      (units::pounds_per_square_inch_t)
          MechanismConst::kcompressor_warning_pressure) {
    return true;
  } else {
    return false;
  }
}