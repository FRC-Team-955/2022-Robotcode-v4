#include "elevator.h"
using namespace frc;
void Elevator::ElevatorMove(int joystick_position) {

  if (solenoid0.Get() == 1) {
    elevator_motor.Set(ControlMode::PercentOutput, 0);
  } else {
    // If elevator is still in the setup phase which is below the bottom limit
    // switch
    if (set_up_done == false) {
      if (solenoid0.Get() == 1) {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      }
      // if during set up it started below the bottom limit
      else if (limit_switch_bottom.Get() == 1) {
        set_up_done = true;
        elevator_motor.SetSelectedSensorPosition(0);
      }
      // if it somehow started above the bottom limit switch as a fail safe
      // (will not slow down when reaching this point)
      else if (limit_switch_top.Get() == 1) {
        // add actually top Encoder position later
        elevator_motor.SetSelectedSensorPosition(5000);
        if (joystick_position > 0) {
          elevator_motor.Set(ControlMode::PercentOutput, 0);
        } else if (joystick_position < 0) {
          elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
        }
        set_up_done = true;
      } else if (joystick_position > 0) {
        elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
      } else {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      }
    }
    // if the position of the elevator is now known
    if (set_up_done == true) {
      if (solenoid0.Get() == 1) {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      } else if ((limit_switch_top.Get() == 1 ||
                  elevator_motor.GetSelectedSensorPosition() > 5000) &&
                 joystick_position > 0) {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      } else if ((limit_switch_top.Get() == 1 ||
                  elevator_motor.GetSelectedSensorPosition() > 5000) &&
                 joystick_position < 0) {
        elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
      } else if ((limit_switch_bottom.Get() == 1 ||
                  elevator_motor.GetSelectedSensorPosition() < 0) &&
                 OffGround()) {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      } else if ((elevator_motor.GetSelectedSensorPosition() < -5000) &&
                 joystick_position < 0) {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      } else if ((elevator_motor.GetSelectedSensorPosition() < -5000) &&
                 joystick_position > 0) {
        elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
      } else if (elevator_motor.GetSelectedSensorPosition() > 4000) {
        if (joystick_position > 0) {
          elevator_motor.Set(ControlMode::PercentOutput,
                             joystick_position *
                                 MechanismConst::elevator_slow_multiplier);
        } else {
          elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
        }
      } else if (elevator_motor.GetSelectedSensorPosition() < 1000 &&
                 OffGround()) {
        if (joystick_position < 0) {
          elevator_motor.Set(ControlMode::PercentOutput,
                             joystick_position *
                                 MechanismConst::elevator_slow_multiplier);
        } else {
          elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
        }
      } else if (elevator_motor.GetSelectedSensorPosition() < -4000) {
        if (joystick_position < 0) {
          elevator_motor.Set(ControlMode::PercentOutput,
                             joystick_position *
                                 MechanismConst::elevator_slow_multiplier);
        } else {
          elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
        }
      } else if (elevator_motor.GetSelectedSensorPosition() >= -4000 &&
                 elevator_motor.GetSelectedSensorPosition() <= 4000) {
        elevator_motor.Set(ControlMode::PercentOutput, joystick_position);
      } else {
        elevator_motor.Set(ControlMode::PercentOutput, 0);
      }
    }
  }
}

void Elevator::LockElevator() {
  solenoid0.Set(1);
  elevator_motor.Set(ControlMode::PercentOutput, 0);
}

void Elevator::UnlockElevator() {
  solenoid0.Set(0);
  elevator_motor.Set(ControlMode::PercentOutput, 0);
}

bool Elevator::OffGround() {
  if (elevator_motor.GetOutputCurrent() >= MechanismConst::climb_amperage) {
    return true;
  } else {
    return false;
  }
}

void Elevator::DisplayElevatorInfo(){
  frc::Shuffleboard::GetTab("End Game").Add("Elevator Amp", elevator_motor.GetOutputCurrent());
  frc::Shuffleboard::GetTab("End Game").Add("Elevator Position", elevator_motor.GetSelectedSensorPosition(0));
  frc::Shuffleboard::GetTab("End Game").Add("Elevator Pnemactic State",solenoid0.Get());

}