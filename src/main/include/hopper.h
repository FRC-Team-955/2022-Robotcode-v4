#ifndef HOPPER
#define HOPPER

#include <ctre/Phoenix.h>

#include "settings.h"

class Hopper {
public:
  Hopper(TalonSRX *talon_hopper_top, TalonSRX *talon_hopper_bottom):
  talon_hopper_top(talon_hopper_top), talon_hopper_bottom(talon_hopper_bottom) {
    talon_hopper_top->TalonSRX::ConfigPeakCurrentLimit(40);
    talon_hopper_bottom->TalonSRX::ConfigPeakCurrentLimit(40);
    talon_hopper_top->TalonSRX::EnableCurrentLimit(true);
    talon_hopper_bottom->TalonSRX::EnableCurrentLimit(true);
  }
  void RunHopperMotor(float hopper_motor_percent_top, float hopper_motor_percent_bottom);
  void DiplayHopperInfo();
  void InitShoot();
  
  bool hopper_on = false;
private:
  TalonSRX *talon_hopper_top;
  TalonSRX *talon_hopper_bottom;
};
#endif
