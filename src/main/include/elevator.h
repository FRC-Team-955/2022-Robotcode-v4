#ifndef ELEVATOR
#define ELEVATOR

#include <ctre/Phoenix.h>
#include <frc/DigitalInput.h>
#include <frc/Joystick.h>
#include <frc/DoubleSolenoid.h>

#include "settings.h"

using namespace frc;


class Elevator {
public:
  Elevator(TalonFX *elevator_motor, DigitalInput *limit_switch_top, DigitalInput *limit_switch_bottom, DoubleSolenoid *elevator_solenoid_lock): 
  elevator_motor(elevator_motor),limit_switch_top(limit_switch_top), limit_switch_bottom(limit_switch_bottom), 
  elevator_solenoid_lock(elevator_solenoid_lock){
    elevator_motor->SetNeutralMode(NeutralMode::Brake);
    elevator_motor->SetSelectedSensorPosition(0);
    elevator_motor->TalonFX::ConfigSupplyCurrentLimit(SupplyCurrentLimitConfiguration(true, 40, 40, 0.1));
    // line above sets current limit. SupplyCurrentLimitConfiguration goes as
    // follow: CupplyCurrentLimitConfiguration(enable, current limit,
    // triggerThresholdCurrent, triggerThresholdTime)
  }
  void ElevatorMove(double joystick_position);
  void LockElevator();
  void UnlockElevator();
  bool OffGround();
  void DisplayElevatorInfo();
  void ResetPosition();

private:
  double motor_output_current;
  bool hit_top_limit = false;
  TalonFX *elevator_motor;
  DigitalInput *limit_switch_top;
  DigitalInput *limit_switch_bottom;
  DoubleSolenoid *elevator_solenoid_lock;
};
#endif