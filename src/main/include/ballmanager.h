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
  BallManager(Intake *intake, Hopper *hopper, Shooter *shooter, ColorSensor *color_sensor_bot, ColorSensor *color_sensor_top):
  intake(intake), hopper(hopper),shooter(shooter), color_sensor_bot(color_sensor_bot), color_sensor_top(color_sensor_top){};
  std::string GetHopperState(int position);
  bool Rev(double target_velocity_top, double target_velocity_bottom);
  void CheckHopperState();
  bool IsFull();
  bool IsEmpty();
  void LoadHopper();
  void Reject();
  void Shoot();
  void DisplayBallManagerInfo();
  std::string team_color = "Red";

private:
  std::string position[2] = {"None", "None"};//position[0] = color of position 1 position[1] = color of position 2

  Intake *intake;
  Hopper *hopper;
  Shooter *shooter;
  ColorSensor *color_sensor_bot;
  ColorSensor *color_sensor_top;

};
#endif