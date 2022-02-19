#include "intake.h"

using namespace frc;

void Intake::PistonUp() {
  double_solenoid_left->Set(frc::DoubleSolenoid::Value::kForward);
  double_solenoid_right->Set(frc::DoubleSolenoid::Value::kForward);
}
void Intake::PistonDown() {
  double_solenoid_left->Set(frc::DoubleSolenoid::Value::kReverse);
  double_solenoid_right->Set(frc::DoubleSolenoid::Value::kReverse);
}
void Intake::RunIntake(float intake_percent) {
  std::cout<<"function run"<<std::endl;
  if (intake_talon->GetOutputCurrent() > MechanismConst::kintake_reversal_amps) {
    intake_percent *= -1;
  }
  intake_talon->Set(ControlMode::PercentOutput, intake_percent);
}
void Intake::StopIntake() { intake_talon->Set(ControlMode::PercentOutput, 0); }
void Intake::DisplayIntakeInfo(){
  frc::Shuffleboard::GetTab("Telop").Add("IntakeMotorAmps", intake_talon->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Sol1", double_solenoid_left->Get()).WithWidget(frc::BuiltInWidgets::kBooleanBox);
  frc::Shuffleboard::GetTab("Telop").Add("Sol2", double_solenoid_right->Get()).WithWidget(frc::BuiltInWidgets::kBooleanBox);
}
