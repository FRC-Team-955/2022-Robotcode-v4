#ifndef BALLMANAGER
#define BALLMANAGER

#include "hopper.h"
#include "colorsensor.h"
#include "hopper.h"
#include "shooter.h"
#include "ultrasonic.h"
#include "intake.h"
#include <frc/DigitalInput.h>


#include "settings.h"
using namespace frc;
class BallManager {
public:
  BallManager(Intake *intake, Hopper *hopper, Shooter *shooter):
  intake(intake), hopper(hopper),shooter(shooter){
    digitalinput = new DigitalInput(0);
  };
  std::string GetHopperState(int position);
  bool Rev(double target_velocity_top, double target_velocity_bottom);
  void MoveIndex();
  void CheckHopperState();
  bool IsFull();
  void LoadHopper();
  void Reject();
  void Shoot();
  void DisplayBallManagerInfo();
  std::string team_color = "Red";

private:
  std::string position[2] = {"NULL", "NULL"};//position[0] = color of position 1 position[1] = color of position 2
  std::string inbetween = "NULL";
  
  ColorSensor color_sensor;
  UltraSonic ultrasonic;
  DigitalInput *digitalinput;
  Intake *intake;
  Hopper *hopper;
  Shooter *shooter;
};
#endif