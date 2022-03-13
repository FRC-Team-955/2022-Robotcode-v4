#include "elevator.h"

#include "iostream"

using namespace frc;

void Elevator::ElevatorMove(double joystick_position) {
  //is amperage limit switch
  // if (elevator_motor->GetOutputCurrent() > 50) hit_top_limit = true;
  // else {
  //   joystick_position = -joystick_position;
  //   // If elevator is still in the setup phase which is below the bottom limit
  //   // switch
  //   if (set_up_done == false) {
  //   //   // if during set up it started below the bottom limit
  //     if (limit_switch_bottom->Get() == 0) {
  //       set_up_done = true;
  //       elevator_motor->SetSelectedSensorPosition(0);
  //     }
  //   //   // if it somehow started above the bottom limit switch as a fail safe
  //   //   // (will not slow down when reaching this point)
  //     else if (elevator_motor->GetSelectedSensorPosition() > 280000) {
  //       if (joystick_position > 0) {
  //         elevator_motor->Set(ControlMode::PercentOutput, 0);
  //       } else if (joystick_position < 0) {
  //         elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
  //       }
  //       set_up_done = true;
  //     }
  //     else if (joystick_position > 0) {
  //       elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
  //     } else {
  //       elevator_motor->Set(ControlMode::PercentOutput, 0);
  //     }
    // }
    //316000 //top
    //290000 //slow down
  joystick_position = -joystick_position;

  if (limit_switch_bottom->Get() == 0) {
    ResetPosition();
  }
  // if (elevator_solenoid_lock->Get() == 1){
  //     elevator_motor->Set(ControlMode::PercentOutput, 0);
  // }else{
  //   if ((elevator_motor->GetSelectedSensorPosition() <= 5000) && joystick_position < 0) {
  //     elevator_motor->Set(ControlMode::PercentOutput, 0);
  //   }else{
  //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
  //   }
  // }

  // if (joystick_position > 0 && elevator_motor->GetOutputCurrent() > 50) {
  //   hit_top_limit = true;
  // }
  // if (joystick_position < 0){
  //   hit_top_limit = false;
  // }
  std::cout<<"Elevator Position: "<<elevator_motor->GetSelectedSensorPosition()<<std::endl;
  if (elevator_solenoid_lock->Get() == 1){
        elevator_motor->Set(ControlMode::PercentOutput, 0);
  }else{
   //if solenoid not out
    if (joystick_position < 0 && (elevator_motor->GetSelectedSensorPosition() <= 5000)) {
      //if trying to move downwards and position is less than 5000 
      elevator_motor->Set(ControlMode::PercentOutput, 0);
    }else if(joystick_position > 0 && (limit_switch_top->Get() == 1 || elevator_motor->GetSelectedSensorPosition() > 300000 || hit_top_limit)){
      //if trying to move upwards and position is greater than 300,000
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
  frc::SmartDashboard::PutNumber("Elevator Amp", elevator_motor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Elevator Position", elevator_motor->GetSelectedSensorPosition(0));
  frc::SmartDashboard::PutBoolean("Elevator Solenoid Extended", !(bool)(elevator_solenoid_lock->Get()-1));

}