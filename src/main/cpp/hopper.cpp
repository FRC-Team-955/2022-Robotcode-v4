#include "hopper.h"

void Hopper::RunHopperMotor(float hopper_motor_percent_top,float hopper_motor_percent_bottom) {
  talon_hopper_top->Set(ControlMode::PercentOutput,std::min(hopper_motor_percent_top, max));
  talon_hopper_bottom->Set(ControlMode::PercentOutput,std::min(hopper_motor_percent_bottom, max));
}
void Hopper::DiplayHopperInfo(){
  frc::Shuffleboard::GetTab("Telop").Add("Hopper Bottom Amp",talon_hopper_bottom->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Hopper Top Amp",talon_hopper_top->GetOutputCurrent());
}