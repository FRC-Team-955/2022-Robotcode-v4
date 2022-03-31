#ifndef LIMELIGHT
#define LIMELIGHT

#include "settings.h"
#include "navx.h"
#include <iostream>
#include <algorithm>
#include <photonlib/PhotonCamera.h>
#include <frc/controller/PIDController.h>
#include <photonlib/PhotonUtils.h>
#include <units/angle.h>
#include <units/length.h>

using namespace frc;

class Limelight {
public:
  Limelight() {
    camera = new photonlib::PhotonCamera("gloworm");
    //0.03, 0.015, 0.05
    //0.023, 0.003, -0.01
    //0.05,0.025,0.014
    //0.02,0.01,-0.3
    //0.04,0.015,0.005
    controller = new frc2::PIDController(0.055,0.018,0);
    navx = new Navx();
  };
  ~Limelight(){
      delete camera;
      delete controller;
  }
  double GetDrivebaseSpeed();
  double DeadReckCaclulate();
  double GetDrivebaseSpeedToOffset(double offset);
  double GetShooterSpeedClose(std::string shooter_position);
  double GetShooterSpeedFar();
  bool IsAligned();
  bool IsAligned(double offset);
  double GetOffset();
  bool ShootIsCloseFromClose();
  bool ShootIsCloseFromFar();
  void DisplayLimelightFar();
  void DisplayLimelightClose();
  void DisplayLimelightInfo();
private:
  Navx *navx;
  photonlib::PhotonCamera *camera;
  frc2::PIDController *controller;
  photonlib::PhotonPipelineResult result;
  double velocity_offset = 0;
  double ramp_speed = 0;
  double range = 0;
  double targetYaw = 42069;
};

#endif