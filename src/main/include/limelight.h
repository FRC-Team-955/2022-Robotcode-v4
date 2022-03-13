#ifndef LIMELIGHT
#define LIMELIGHT

#include "settings.h"
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
    controller = new frc2::PIDController(0.03,0.03,0);
  };
  ~Limelight(){
      delete camera;
      delete controller;
  }
  double GetDrivebaseSpeed();
  double GetShooterSpeed(std::string shooter_position);
  bool IsAligned();

private:
  photonlib::PhotonCamera *camera;
  frc2::PIDController *controller;
};

#endif