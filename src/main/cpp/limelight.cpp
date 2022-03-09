#include <limelight.h>


/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeed() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  if (result.HasTargets()) {
    // Rotation speed is the output of the PID controller
    return -controller->Calculate(result.GetBestTarget().GetYaw(), 0);
  } else {
    // If we have no targets, stay still
    return 0;
  }
}

/**
 * @return shooter wheel speed from distance to target and regression
 */
double Limelight::GetShooterSpeed() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    // First calculate range (in meters)
    double range = double(photonlib::PhotonUtils::CalculateDistanceToTarget(
        AutoConst::camera_height, AutoConst::target_height,
        AutoConst::camera_pitch,
        units::degree_t{result.GetBestTarget().GetPitch()}));

    return AutoConst::shooter_m * range + AutoConst::shooter_b;
  } else {
    // If we have no targets don't spin up
    return 0;
  }
  return 1;
}