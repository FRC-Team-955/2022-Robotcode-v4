#ifndef LIMELIGHT
#define LIMELIGHT

#include "settings.h"
#include <photonlib/PhotonCamera.h>
#include <frc/controller/PIDController.h>
#include <photonlib/PhotonUtils.h>
#include <units/angle.h>
#include <units/length.h>


using namespace frc;

class Limelight {
public:
  Limelight() {
    camera = new photonlib::PhotonCamera("photonvision");
    controller = new frc2::PIDController(.1, 0, 0);
  };
  ~Limelight(){
      delete camera;
      delete controller;
  }
  double GetDrivebaseSpeed();
  double GetShooterSpeed();

private:
  photonlib::PhotonCamera *camera;
  frc2::PIDController *controller;
};

#endif