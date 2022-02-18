#include "intake.h"

using namespace frc;

void Intake::PistonUp() {
  sol1->Set(1);
  sol2->Set(1);
}
void Intake::PistonDown() {
  sol1->Set(0);
  sol2->Set(0);
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
  frc::Shuffleboard::GetTab("Telop").Add("Sol1", sol1->Get());
  frc::Shuffleboard::GetTab("Telop").Add("Sol2", sol2->Get());
}
