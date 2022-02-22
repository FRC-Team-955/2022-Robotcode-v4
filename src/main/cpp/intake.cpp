#include "intake.h"

using namespace frc;

void Intake::PistonUp() {
  double_solenoid->Set(frc::DoubleSolenoid::Value::kForward);
}
void Intake::PistonDown() {
  double_solenoid->Set(frc::DoubleSolenoid::Value::kReverse);
}
void Intake::RunIntake(float intake_percent) {
  // if (intake_talon->GetOutputCurrent() > MechanismConst::kintake_reversal_amps) {
  //   // intake_percent *= -1;
  // }
  intake_talon->Set(ControlMode::PercentOutput, intake_percent);
}
void Intake::StopIntake() { intake_talon->Set(ControlMode::PercentOutput, 0); }
void Intake::DisplayIntakeInfo(){
  frc::Shuffleboard::GetTab("Telop").Add("IntakeMotorAmps", intake_talon->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Sol1", double_solenoid->Get()).WithWidget(frc::BuiltInWidgets::kBooleanBox);
}
