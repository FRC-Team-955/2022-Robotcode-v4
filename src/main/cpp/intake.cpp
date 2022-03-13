#include "intake.h"

using namespace frc;

void Intake::PistonUp() {
  double_solenoid->Set(frc::DoubleSolenoid::Value::kForward);
}
void Intake::PistonDown() {
  double_solenoid->Set(frc::DoubleSolenoid::Value::kReverse);
}
void Intake::RunIntake(float intake_percent) {
  intake_talon->Set(ControlMode::PercentOutput, intake_percent);
}
void Intake::StopIntake() { intake_talon->Set(ControlMode::PercentOutput, 0); }
void Intake::DisplayIntakeInfo(){
  frc::SmartDashboard::PutNumber("Intake Motor Amps", intake_talon->GetOutputCurrent());
  frc::SmartDashboard::PutBoolean("Intake Solenoid Extension", (bool)(double_solenoid->Get()-1));
}
