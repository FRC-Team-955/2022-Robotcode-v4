#include "elevator.h"

#include "iostream"

using namespace frc;

void Elevator::ElevatorMove(double joystick_position) {
  joystick_position = -joystick_position;
  if (limit_switch_bottom->Get() == 0) {
    ResetPosition();
  }
  if (joystick_position > 0 && elevator_motor->GetOutputCurrent() > 40) {
    hit_top_limit = true;
  }
  if (joystick_position < 0){
    hit_top_limit = false;
  }
  if (elevator_solenoid_lock->Get() == 1){
        elevator_motor->Set(ControlMode::PercentOutput, 0);
  }else{
   //if solenoid not out
    if (joystick_position < 0 && (elevator_motor->GetSelectedSensorPosition() <= 5000)) {
      //if trying to move downwards and position is less than 5000 
      elevator_motor->Set(ControlMode::PercentOutput, 0);
    }else if(joystick_position > 0 && (limit_switch_top->Get() == 1 || hit_top_limit)){
      //if trying to move upwards and position is greater than 300,000
      elevator_motor->SetSelectedSensorPosition(300000);
      elevator_motor->Set(ControlMode::PercentOutput, 0);
    }else{
      //if not at bottom and moving downwards
      elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
    }
  }
}
void Elevator::ResetPosition(){
  elevator_motor->SetSelectedSensorPosition(0);
}
void Elevator::LockElevator() {
  elevator_solenoid_lock->Set(frc::DoubleSolenoid::Value::kForward);
  elevator_motor->Set(ControlMode::PercentOutput, 0);
}

void Elevator::UnlockElevator() {
  elevator_solenoid_lock->Set(frc::DoubleSolenoid::Value::kReverse);
  elevator_motor->Set(ControlMode::PercentOutput, 0);
}

void Elevator::DisplayElevatorInfo(){
  frc::SmartDashboard::PutBoolean("Elevator Amp Limit Hit", hit_top_limit);
  frc::SmartDashboard::PutNumber("Elevator Amp", elevator_motor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Elevator Position", elevator_motor->GetSelectedSensorPosition(0));
  frc::SmartDashboard::PutBoolean("Elevator Solenoid Extended", !(bool)(elevator_solenoid_lock->Get()-1));
}