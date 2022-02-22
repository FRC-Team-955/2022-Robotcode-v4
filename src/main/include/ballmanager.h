#ifndef BALLMANAGER
#define BALLMANAGER

#include <frc/DigitalInput.h>

#include "hopper.h"
#include "colorsensor.h"
#include "hopper.h"
#include "shooter.h"
#include "intake.h"

#include "settings.h"

class BallManager {
public:
  BallManager(Intake *intake, Hopper *hopper, Shooter *shooter, ColorSensor *color_sensor, DigitalInput *ir_break_beam):
  intake(intake), hopper(hopper),shooter(shooter), color_sensor(color_sensor), ir_break_beam(ir_break_beam){};
  std::string GetHopperState(int position);
  bool Rev(double target_velocity_top, double target_velocity_bottom);
  void MoveIndex();
  void CheckHopperState();
  bool IsFull();
  void LoadHopper(double joystick_input);
  void Reject();
  void Shoot();
  void DisplayBallManagerInfo();
  bool IrIsBall();
  std::string team_color = "Red";
  std::string inbetween = "None";

private:
  std::string position[2] = {"None", "None"};//position[0] = color of position 1 position[1] = color of position 2

  Intake *intake;
  Hopper *hopper;
  Shooter *shooter;
  ColorSensor *color_sensor;
  DigitalInput *ir_break_beam;

};
#endif