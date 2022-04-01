#include <ballalign.h>


/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Ballalign::GetDrivebaseSpeed() {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    if(std::abs(result.GetBestTarget().GetYaw()) < 7){
      ramp_speed = 0;
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
bool Ballalign::CheckOtherColor(std::string teamcolor) {
  if(teamcolor == "Red"){
    camera->SetPipelineIndex(BallConst::kblue_pipeline_index);
  } else if(teamcolor == "Blue"){
    camera->SetPipelineIndex(BallConst::kred_pipeline_index);
  }
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  if (result.HasTargets()) {
    return true;
  } else {
    return false;
  }

}

void Ballalign::SetDriverMode(bool mode){
  camera->SetDriverMode(mode);
}
/**
 * @return the steering joystick input needed to align to target {-1,1} or 0 if none
 */
double Ballalign::GetDrivebaseSpeedToOffset(double offset) {
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
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

/**
 * @return If yaw is in range
 */
bool Ballalign::IsAligned(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  return std::abs(result.GetBestTarget().GetYaw()) < 3;
}
bool Ballalign::IsAligned(double offset){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  return std::abs(result.GetBestTarget().GetYaw()-offset) < 3;
}
double Ballalign::GetOffset(){
  photonlib::PhotonPipelineResult result = camera->GetLatestResult();
  return result.GetBestTarget().GetYaw();
}

