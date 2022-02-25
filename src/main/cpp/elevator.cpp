#include "elevator.h"

#include "iostream"

using namespace frc;

void Elevator::ElevatorMove(double joystick_position) {
  if (elevator_solenoid_lock->Get() == 0){
      elevator_motor->Set(ControlMode::PercentOutput, 0);

  } else {
    joystick_position = -joystick_position;
    std::cout<<"bottom: "<<limit_switch_bottom->Get()<<std::endl;
    //std::cout<<"top: "<<limit_switch_top->Get()<<std::endl;
    std::cout<<"elevator position: "<<elevator_motor->GetSelectedSensorPosition()<<std::endl;

    std::cout<<"set up done"<<set_up_done<<std::endl;
    // If elevator is still in the setup phase which is below the bottom limit
    // switch
    if (set_up_done == false) {
    //   // if during set up it started below the bottom limit
      if (limit_switch_bottom->Get() == 0) {
        set_up_done = true;
        elevator_motor->SetSelectedSensorPosition(0);
      }
    //   // if it somehow started above the bottom limit switch as a fail safe
    //   // (will not slow down when reaching this point)
      else if (elevator_motor->GetSelectedSensorPosition() > 280000) {
        if (joystick_position > 0) {
          elevator_motor->Set(ControlMode::PercentOutput, 0);
        } else if (joystick_position < 0) {
          elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
        }
        set_up_done = true;
      }
      else if (joystick_position > 0) {
        elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      } else {
        elevator_motor->Set(ControlMode::PercentOutput, 0);
      }
    }
    //316000 //top
    //290000 //slow down

    //50000
    //0

    // if the position of the elevator is now known
    if (set_up_done == true) {
      if (elevator_motor->GetSelectedSensorPosition() > 290000 && joystick_position > 0) {
        elevator_motor->Set(ControlMode::PercentOutput, 0);
      } else if (elevator_motor->GetSelectedSensorPosition() > 290000 && joystick_position < 0) {
        elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      } else if ((elevator_motor->GetSelectedSensorPosition() < 10000) && joystick_position < 0) {
        elevator_motor->Set(ControlMode::PercentOutput, 0);
      } else if ((elevator_motor->GetSelectedSensorPosition() < 0) && joystick_position > 0) {
        elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      }
      // } else if (elevator_motor->GetSelectedSensorPosition() > 290000) {
      //   //going up slow down
      //   if (joystick_position > 0) {
      //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position * MechanismConst::elevator_slow_multiplier);
      //   } else {
      //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      //   }
      // } else if (elevator_motor->GetSelectedSensorPosition() < 1000 && OffGround()) {
      //   if (joystick_position < 0) {
      //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position * MechanismConst::elevator_slow_multiplier);
      //   } else {
      //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      //   }
      // } else if (elevator_motor->GetSelectedSensorPosition() < -4000) {
      //   if (joystick_position < 0) {
      //     elevator_motor->Set(ControlMode::PercentOutput,
      //                         joystick_position *
      //                             MechanismConst::elevator_slow_multiplier);
      //   } else {
      //     elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      //   }
      // }
      else if ((elevator_motor->GetSelectedSensorPosition() > 0) && (elevator_motor->GetSelectedSensorPosition() < 290000)) {
        elevator_motor->Set(ControlMode::PercentOutput, joystick_position);
      }
      else {
        elevator_motor->Set(ControlMode::PercentOutput, 0);
      }
    }
  }
}
void Elevator::ResetPosition(){
  elevator_motor->SetSelectedSensorPosition(0);
}
// void Elevator::LockElevator() {
//   solenoid0.Set(1);
//   elevator_motor->Set(ControlMode::PercentOutput, 0);
// }

// void Elevator::UnlockElevator() {
//   solenoid0.Set(0);
//   elevator_motor->Set(ControlMode::PercentOutput, 0);
// }

bool Elevator::OffGround() {
  if (elevator_motor->GetOutputCurrent() >= MechanismConst::climb_amperage) {
    return true;
  } else {
    return false;
  }
}

void Elevator::DisplayElevatorInfo(){
  frc::Shuffleboard::GetTab("End Game").Add("Elevator Amp", elevator_motor->GetOutputCurrent()).GetEntry();
  frc::Shuffleboard::GetTab("End Game").Add("Elevator Position", elevator_motor->GetSelectedSensorPosition(0)).GetEntry();
  // frc::Shuffleboard::GetTab("End Game").Add("Elevator Pnemactic State",elevator_solenoid_lock->Get());

}