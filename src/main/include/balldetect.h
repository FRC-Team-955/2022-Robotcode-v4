#ifndef BALLDETECT
#define BALLDETECT

#include "photonlib/PhotonCamera.h"
#include "photonlib/PhotonUtils.h"

class BallDetect {
public:
  BallDetect() {}
  static double BallDetectorY(photonlib::PhotonPipelineResult result);
  static double BallDetectorX(photonlib::PhotonPipelineResult result);
};
#endif
