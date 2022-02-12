#ifndef BALLDETECT
#define BALLDETECT

#include "photonlib/PhotonCamera.h"
#include "photonlib/PhotonUtils.h"
#include <iostream>

class BallDetect {
public:
  BallDetect() {}
  double BallDetectorY(photonlib::PhotonPipelineResult result);
  double BallDetectorX(photonlib::PhotonPipelineResult result);
};
#endif
