#include "hopper.h"

void Hopper::RunHopperMotor(float hopper_motor_percent_top,float hopper_motor_percent_bottom) {
  talon_hopper_top->Set(ControlMode::PercentOutput,hopper_motor_percent_top);
  talon_hopper_bottom->Set(ControlMode::PercentOutput,hopper_motor_percent_bottom);
}
void Hopper::DiplayHopperInfo(){
  frc::SmartDashboard::PutNumber("Hopper Bottom Amp", talon_hopper_bottom->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Hopper Top Amp", talon_hopper_top->GetOutputCurrent());
}
