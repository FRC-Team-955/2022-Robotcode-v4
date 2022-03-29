#ifndef BALLALIGN
#define BALLALIGN

#include "settings.h"
#include <iostream>
#include <algorithm>
#include <photonlib/PhotonCamera.h>
#include <frc/controller/PIDController.h>
#include <photonlib/PhotonUtils.h>
#include <units/angle.h>
#include <units/length.h>
#include "ballmanager.h"

using namespace frc;

class Ballalign {
public:
  Ballalign() {
    camera = new photonlib::PhotonCamera("ballcam");
    //0.03, 0.015, 0.05
    //0.023, 0.003, -0.01
    //0.05,0.025,0.014
    //0.02,0.01,-0.3
    //0.04,0.015,0.005
    controller = new frc2::PIDController(0.055,0.018,0);
  };
  ~Ballalign(){
      delete camera;
      delete controller;
  }
  double GetDrivebaseSpeed();
  double GetDrivebaseSpeedToOffset(double offset);
  bool IsAligned();
  bool IsAligned(double offset);
  double GetOffset();
  bool CheckOtherColor(std::string teamcolor);
private:
  photonlib::PhotonCamera *camera;
  frc2::PIDController *controller;
  double ramp_speed = 0;
  double range = 0;
};

#endif