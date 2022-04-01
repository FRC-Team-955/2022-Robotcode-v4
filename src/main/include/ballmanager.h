#ifndef BALLMANAGER
#define BALLMANAGER

#include <frc/DigitalInput.h>

#include "hopper.h"
#include "colorsensor.h"
#include "hopper.h"
#include "shooter.h"
#include "intake.h"
#include "limelight.h"

#include "settings.h"

class BallManager {
public:
  BallManager(Intake *intake, Hopper *hopper, Shooter *shooter, ColorSensor *color_sensor_bot, ColorSensor *color_sensor_top, Limelight *limelight):
  intake(intake), hopper(hopper),shooter(shooter), color_sensor_bot(color_sensor_bot), color_sensor_top(color_sensor_top), limelight(limelight){};
  
  void CheckHopperState();
  void MoveIndex();
  void LoadHopper();
  bool IsFull();
  bool IsEmpty();

  bool RevLow();
  bool RevHigh();
  bool RevSide();
  bool RevLimeLightClose();
  bool RevLimeLightFar();
  bool RevLaunchPad();

  void Shoot();
  bool ShootFromClose(int shooter_solenoid_state);

  void RejectBottom();
  void RejectTop();
  void Reject();

  void DisplayBallManagerInfo();
  std::string team_color = "Red";
  bool pid_only = false;

private:
  bool Rev(double target_velocity_top, double target_velocity_bottom);

  std::string position[2] = {"None", "None"};//position[0] = color of position 1 position[1] = color of position 2

  Intake *intake;
  Hopper *hopper;
  Shooter *shooter;
  ColorSensor *color_sensor_bot;
  ColorSensor *color_sensor_top;
  Limelight *limelight;
};
#endif