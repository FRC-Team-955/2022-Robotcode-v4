#include "balldetect.h"

double BallDetect::BallDetectorX(photonlib::PhotonPipelineResult result) {
  if (result.HasTargets()) {
    return result.GetBestTarget().GetYaw();
  }
  return 999969;
}

double BallDetect::BallDetectorY(photonlib::PhotonPipelineResult result) {
  if (result.HasTargets()) {
    return result.GetBestTarget().GetPitch();
  }
  return 0;
}