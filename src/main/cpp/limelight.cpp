#include <limelight.h>

/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeed() {
  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()) < 7){
      ramp_speed = 0;

      //line to calculate yaw with PID
      return -controller->Calculate(result.GetBestTarget().GetYaw(), 0);
    }else{
      if(ramp_speed < .35){
        ramp_speed+=.1;
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
double Limelight::DeadReckCaclulate(double offset){
  if (targetYaw < 400 && std::abs(targetYaw - navx->GetYawRadians() * (180.0 / 3.141592)) < 0.35) {
    targetYaw = 42069;
    //shoot code here
  } else {
    if (targetYaw > 400) {
    targetYaw = navx->GetYawRadians() * (180.0 / 3.141592) + offset;
    }
    return -controller->Calculate(navx->GetYawRadians()  * (180.0 / 3.141592) , targetYaw);
  }
}
/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Limelight::GetDrivebaseSpeedToOffset(double offset) {
  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()-offset) < 7){
      ramp_speed = 0;
      return -controller->Calculate((result.GetBestTarget().GetYaw()), offset);
    }else{
      if(ramp_speed < .35){
        ramp_speed+=.1;
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
 * @return shooter wheel speed from distance to target and regression
 */
double Limelight::GetShooterSpeedClose(std::string shooter_position) {
  if (result.HasTargets()) {
    // First calculate range (in meters)
    velocity_offset = frc::Shuffleboard::GetTab("Pre").Add("Max Speed", 1).WithWidget(frc::BuiltInWidgets::kNumberSlider).GetEntry().GetDouble(0);

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
double Limelight::GetShooterSpeedFar() {
  if (result.HasTargets()) {
    velocity_offset = frc::Shuffleboard::GetTab("Pre").Add("Max Speed", 0).WithWidget(frc::BuiltInWidgets::kNumberSlider).GetEntry().GetDouble(0);
    // First calculate range (in meters)
    range = double(photonlib::PhotonUtils::CalculateDistanceToTarget(AutoConst::camera_height, AutoConst::target_height,AutoConst::camera_pitch,units::degree_t{result.GetBestTarget().GetPitch()}));
    range = range*39.3701;
    // std::cout<<"Range: "<<range<<std::endl;
    // return 64.4 * range - 626;
    // return 51.4 * range - 50.1;
    return 1400 - 9.76*range +0.609*range*range + velocity_offset;
  }else {
    // If we have no targets don't spin up
    return 0;
  }
}
/**
 * @return If yaw is in range
 */
bool Limelight::IsAligned(){
  return std::abs(result.GetBestTarget().GetYaw()) < 3;
}
bool Limelight::IsAligned(double offset){
  return std::abs(result.GetBestTarget().GetYaw()-offset) < 3;
}
double Limelight::GetOffset(){
  return result.GetBestTarget().GetYaw();
}
bool Limelight::ShootIsCloseFromClose(){
  if (range > 60){
    return false;
  }else{
    return true;
  }
}
bool Limelight::ShootIsCloseFromFar(){
  if (range > 30){
    return false;
  }else{
    return true;
  }
}
void Limelight::DisplayLimelightFar(){
    frc::SmartDashboard::PutNumber("Target Top Velocity", GetShooterSpeedFar());
    frc::SmartDashboard::PutNumber("Target Bottom Velocity", 2300);

}
void Limelight::DisplayLimelightClose(){
    frc::SmartDashboard::PutNumber("Target Top Velocity", GetShooterSpeedClose("Top"));
    frc::SmartDashboard::PutNumber("Target Bottom Velocity", GetShooterSpeedClose("Bottom"));
}
void Limelight::DisplayLimelightInfo(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  frc::SmartDashboard::PutNumber("Limelight Range", range);
}