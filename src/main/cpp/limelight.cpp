#include <limelight.h>

/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeed() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  std::cout<<range<<std::endl;

  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()) < 7){
      ramp_speed = 0;

      //line to calculate yaw with PID
      return -controller->Calculate(result.GetBestTarget().GetYaw(), 0);
    }else{
      if(ramp_speed < .25){
        ramp_speed+=.08;
      }
      if(result.GetBestTarget().GetYaw()>0){
        return ramp_speed;
      }else{
        return -ramp_speed;
      }
    }
  } else {
    // If we have no targets, stay still
    ramp_speed = 0;
    return 0;
  }
}

//alternative to GetDriveBaseSpeed to lower latency 
double Limelight::DeadReckCaclulate(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  if (targetYaw < 400 && std::abs(targetYaw - navx->GetYawRadians() * (180.0 / 3.141592)) < 0.35) {
    targetYaw = 42069;
    //shoot code here
  } else {
    if (targetYaw > 400) {
      targetYaw = navx->GetYawRadians() * (180.0 / 3.141592) + result.GetBestTarget().GetYaw();
      
    }
    return -controller->Calculate(navx->GetYawRadians()  * (180.0 / 3.141592) , targetYaw);
  }
}
/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeedToOffset(double offset) {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()-offset) < 7){
      ramp_speed = 0;
      return -controller->Calculate((result.GetBestTarget().GetYaw()), offset);
    }else{
      if(ramp_speed < .25){
        ramp_speed+=.08;
      }
      if((result.GetBestTarget().GetYaw()-offset)>0){
        return ramp_speed;
      }else{
        return -ramp_speed;
      }
    }
  } else {
    // If we have no targets, stay still
    ramp_speed = 0;
    return 0;
  }
}

/**
 * @param string shooter position "Top" or "Bottom"
 * @return shooter wheel velocity from distance to target and then regression in the close position
 */
double Limelight::GetShooterSpeedClose(std::string shooter_position) {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  if (result.HasTargets()) {
    // First calculate range (in meters)
    range = double(photonlib::PhotonUtils::CalculateDistanceToTarget(AutoConst::camera_height, AutoConst::target_height,AutoConst::camera_pitch,units::degree_t{result.GetBestTarget().GetPitch()}));
    range = range*39.3701;
    if (shooter_position == "Top"){
      return 39.8 * range -24.5 + velocity_offset;
    }else if (shooter_position == "Bottom"){
      return std::max(-20.1 * range + 3684 + velocity_offset, MechanismConst::ktarget_limelight_bottom_min);
    }else{
      return 0;
    }
  } else {
    // If we have no targets don't spin up
    return 0;
  }
}
/**
 * @returns top shooter wheel velocity from distance to target and then regression in the far position
 * @note bottom shooter wheel velocity should be set to 2300
 */
double Limelight::GetShooterSpeedFar() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
  // First calculate range (in meters)
    range = double(photonlib::PhotonUtils::CalculateDistanceToTarget(AutoConst::camera_height, AutoConst::target_height,AutoConst::camera_pitch,units::degree_t{result.GetBestTarget().GetPitch()}));
    range = range*39.3701;

    // return 64.4 * range - 626;
    // return 51.4 * range - 50.1;
    //1400 - 9.76*range +0.609*range*range 
    return 59.9* range- 519 + velocity_offset;
  }else {
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
bool Limelight::IsAligned(double offset){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  return std::abs(result.GetBestTarget().GetYaw()-offset) < 3;
}
double Limelight::GetOffset(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  return result.GetBestTarget().GetYaw();
}
bool Limelight::ShootIsCloseFromClose(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (range > 60){
    return false;
  }else{
    return true;
  }
}
bool Limelight::ShootIsCloseFromFar(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();

  if (range < 35){
    return true;
  }else{
    return false;
  }
}
void Limelight::DisplayLimelightClose(){
    frc::SmartDashboard::PutNumber("Target Top Velocity", GetShooterSpeedClose("Top"));
    frc::SmartDashboard::PutNumber("Target Bottom Velocity", GetShooterSpeedClose("Bottom"));
}
void Limelight::DisplayLimelightFar(){
    frc::SmartDashboard::PutNumber("Target Top Velocity", GetShooterSpeedFar());
    frc::SmartDashboard::PutNumber("Target Bottom Velocity", 2300);

}
void Limelight::DisplayLimelightInfo(){
  velocity_offset = frc::SmartDashboard::GetNumber("Velocity Offset", 0);
  frc::SmartDashboard::PutNumber("Limelight Range", range);
}