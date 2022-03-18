#include <limelight.h>

/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeed() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()) < 3){
      controller->SetI(0.05);
    }else{
      controller->SetI(0.018);
    }
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
double Limelight::GetShooterSpeed(std::string shooter_position) {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    // First calculate range (in meters)
    double range = double(photonlib::PhotonUtils::CalculateDistanceToTarget(
        AutoConst::camera_height, AutoConst::target_height,
        AutoConst::camera_pitch,
        units::degree_t{result.GetBestTarget().GetPitch()}));
    range = range*39.3701;
    std::cout<<"Range: "<<range<<std::endl;
    if (shooter_position == "Top"){
      return AutoConst::shooter_m_top * range + AutoConst::shooter_b_top;
    }else if (shooter_position == "Bottom"){
      return std::max(AutoConst::shooter_m_bottom * range + AutoConst::shooter_b_top +50, MechanismConst::ktarget_limelight_bottom_min);
    }else{
      return 0;
    }
  } else {
    // If we have no targets don't spin up
    return 0;
  }
}

/**
 * @return If yaw is in range
 */
bool Limelight::IsAligned(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  return std::abs(result.GetBestTarget().GetYaw()) < 3;
}