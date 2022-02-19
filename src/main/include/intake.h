#ifndef INTAKE
#define INTAKE

#include <ctre/Phoenix.h>
#include <frc/Joystick.h>
#include <frc/Solenoid.h>
#include <iostream>

#include "settings.h"

using namespace frc;

class Intake {
public:
  Intake(TalonSRX *intake_talon):intake_talon(intake_talon){
    intake_talon->TalonSRX::ConfigPeakCurrentLimit(40);
    intake_talon->TalonSRX::EnableCurrentLimit(true);
    //, sol1(sol1), sol2(sol2) , Solenoid *sol1, Solenoid *sol2
  }

  void PistonUp();
  void PistonDown();
  void RunIntake(float intake_percent);
  void StopIntake();
  void DisplayIntakeInfo();

private:
  Joystick *joy1;
  TalonSRX *intake_talon;
  Solenoid *sol1;
  Solenoid *sol2;
};

#endif