#include "xy_align.h"

void XYalign::Align(photonlib::PhotonPipelineResult limeresult){ 
if (limeresult.HasTargets()) {

  float heading_error = limeresult.GetBestTarget().GetYaw();
  float distance_error = limeresult.GetBestTarget().GetPitch();

  float steering_adjust = 0;
    if (Kp*heading_error > 0){
      steering_adjust = std::max(Kp*heading_error , min_command);
    }
    else if (Kp*heading_error <= 0){
      steering_adjust = std::min(Kp*heading_error, min_command);
    }
    float distance_adjust = KpDistance * distance_error; //Set the variable in shooter code

    //experimental shooting code, uses the matrix to solve for the needed parabola to hit the shot
    // float rim_point_x = 2;
    // float rim_point_x = 9;
    // float shooter_position_y = 2;
    // float distance;

    // double matrix[2][2];

    // matrix[0][0] = rim_point_x * rim_point_x;
    // matrix[1][0] = rim_point_x;
    // matrix[0][1] = distance * distance;
    // matrix[1][1] = distance;

    //float parabola_a = 

    // ball_manage.target_velocity1 =  distance_adjust; 
    // ball_manage.target_velocity2 =  distance_adjust;

    // ball_manage.Rev();
    // if(steering_adjust < 0.1 && steering_adjust > -0.1){
    //     ball_manage.Shoot();
    // } 
    drivebase->DriveTank(steering_adjust, -steering_adjust);
}

}
bool XYalign::HasTargetLimeLight(photonlib::PhotonPipelineResult limeresult){
    if(limeresult.HasTargets()){
        return true;
    } else {
        return false;
    }
  
}