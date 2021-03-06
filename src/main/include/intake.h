#ifndef INTAKE
#define INTAKE

#include <ctre/Phoenix.h>
#include <frc/DoubleSolenoid.h>

#include "settings.h"

using namespace frc;

class Intake {
public:
  Intake(TalonSRX *intake_talon, DoubleSolenoid *double_solenoid):
  intake_talon(intake_talon), double_solenoid(double_solenoid){
    // intake_talon->TalonSRX::ConfigPeakCurrentLimit(50);
    // intake_talon->TalonSRX::EnableCurrentLimit(true);
    intake_talon->SetInverted(true);
  }
  void PistonUp();
  void PistonDown();
  void RunIntake(float intake_percent);
  void StopIntake();
  void DisplayIntakeInfo();

private:
  TalonSRX *intake_talon;
  DoubleSolenoid *double_solenoid;
};

#endif