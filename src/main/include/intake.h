#ifndef INTAKE
#define INTAKE
#include "settings.h"
#include <ctre/Phoenix.h>
#include <frc/Joystick.h>
#include <frc/Solenoid.h>
#include <iostream>


using namespace frc;

class Intake {
public:
  Intake(TalonSRX *intake_talon, Solenoid *sol1, Solenoid *sol2):intake_talon(intake_talon), sol1(sol1), sol2(sol2){
    intake_talon->TalonSRX::ConfigPeakCurrentLimit(40);
    intake_talon->TalonSRX::EnableCurrentLimit(true);
  }

  void PistonUp();
  void PistonDown();
  void RunIntake(float intake_percent);
  void StopIntake();

private:
  TalonSRX *intake_talon;
  Solenoid *sol1;
  Solenoid *sol2;
};

#endif