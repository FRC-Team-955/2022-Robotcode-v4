#include "ballmanager.h"
using namespace frc;

// std::string BallManager::GetHopperState(int slot){
//     if(slot != 0 && slot != 1){
//         return "NULL";
//     }
//     else{
//     return position[slot];
//     }
// }
void BallManager::CheckHopperState(){
    if(!color_sensor_bot->CheckForBall(SensorConst::kvalue_for_ball_bottom)){
        position[0] = "None";
    }
    if(!color_sensor_top->CheckForBall(SensorConst::kvalue_for_ball_top)){
        position[1] = "None";
    }
    if(color_sensor_bot->CheckForBall(SensorConst::kvalue_for_ball_bottom)){
        position[0] = color_sensor_bot->ClosestColor(); 
    }
    if(color_sensor_top->CheckForBall(SensorConst::kvalue_for_ball_top)){
        position[1] = color_sensor_top->ClosestColor();
    }
}

void BallManager::MoveIndex(){
    if(color_sensor_bot->CheckForBall(200) && !color_sensor_top->CheckForBall(2000)){
        hopper->RunHopperMotor(0.1, 0.25);
    }
}

void BallManager::LoadHopper(){
    if(position[1]=="None"){
        //.31, 0.5
        hopper->RunHopperMotor(.31, 0.7);
    }
    else if(position[1]!="None" && position[0] == "None"){
        hopper->RunHopperMotor(0, 0.5);
    }else{
        hopper->RunHopperMotor(0, 0);
    }
}
bool BallManager::IsFull(){
    if(position[1] != "None" && position[0] != "None"){
        return !false;
    }
    else{
        return false;
    }
}

bool BallManager::Rev(double target_velocity_top, double target_velocity_bottom){  
    // shooter->VelocityControl(target_velocity_top, target_velocity_bottom);
    if (pid_only){
        shooter->VelocityControl(target_velocity_top, target_velocity_bottom);
        if(std::abs(shooter->VelocityOutput("Top")) >= target_velocity_top - MechanismConst::ktarget_range && 
        std::abs(shooter->VelocityOutput("Top")) <= target_velocity_top + MechanismConst::ktarget_range &&
        std::abs(shooter->VelocityOutput("Bottom")) >= target_velocity_bottom - MechanismConst::ktarget_range &&
        std::abs(shooter->VelocityOutput("Bottom")) <= target_velocity_bottom + MechanismConst::ktarget_range){
            return true;
        }else{
            return false;
        }
    }
    else if(std::abs(shooter->VelocityOutput("Top")) >= target_velocity_top - MechanismConst::ktarget_range && 
        std::abs(shooter->VelocityOutput("Top")) <= target_velocity_top + MechanismConst::ktarget_range &&
        std::abs(shooter->VelocityOutput("Bottom")) >= target_velocity_bottom - MechanismConst::ktarget_range &&
        std::abs(shooter->VelocityOutput("Bottom")) <= target_velocity_bottom + MechanismConst::ktarget_range){
        shooter->ShootVoltage(target_velocity_top, target_velocity_bottom);
        return true;
    }else if(std::abs(shooter->VelocityOutput("Top")) >= target_velocity_top - MechanismConst::ktarget_switch_control_mode && 
        std::abs(shooter->VelocityOutput("Top")) <= target_velocity_top + MechanismConst::ktarget_switch_control_mode &&
        std::abs(shooter->VelocityOutput("Bottom")) >= target_velocity_bottom - MechanismConst::ktarget_switch_control_mode &&
        std::abs(shooter->VelocityOutput("Bottom")) <= target_velocity_bottom + MechanismConst::ktarget_switch_control_mode){
        shooter->ShootVoltage(target_velocity_top, target_velocity_bottom);
        return false;
    }else{
        shooter->VelocityControl(target_velocity_top, target_velocity_bottom);
        return false;
    }
}
bool BallManager::RevLow(){
    return Rev(MechanismConst::ktarget_low_top,MechanismConst::ktarget_low_bottom);
}
bool BallManager::RevHigh(){
    return Rev(MechanismConst::ktarget_high_top, MechanismConst::ktarget_high_bottom);
}
bool BallManager::RevSide(){
    return Rev(MechanismConst::ktarget_side_top, MechanismConst::ktarget_side_bottom);
}
bool BallManager::RevLimeLightClose(){
    return Rev(limelight->GetShooterSpeedClose("Top"), limelight->GetShooterSpeedClose("Bottom"));
}
bool BallManager::RevLimeLightFar(){
    return Rev(limelight->GetShooterSpeedFar(), 2300);
}
bool BallManager::RevLaunchPad(){
    return Rev(MechanismConst::ktarget_launch_top, MechanismConst::ktarget_launch_bottom);
}
void BallManager::Shoot(){
    if (position[1] == "None"){
        hopper->RunHopperMotor(0.25, 0.7);
    }else{
        hopper->RunHopperMotor(0.25, 0.25);
    }
}
void BallManager::RejectBottom(){
    intake->RunIntake(-1);
    hopper->RunHopperMotor(0,-1);
}
void BallManager::RejectTop(){
    if(Rev(MechanismConst::ktarget_reject,MechanismConst::ktarget_reject)){
        hopper->RunHopperMotor(1,0);
    }else{
        hopper->RunHopperMotor(0,0);
    }
}
void BallManager::Reject(){
    double top = 0.0;
    double bottom = 0.0;
    if(position[1] != team_color){
        if(Rev(MechanismConst::ktarget_reject,MechanismConst::ktarget_reject)){
            top = 1;
            // hopper->RunHopperMotor(0.5,0);
        }else{
            top = 0.0;
        }
    }
    if(position[0] != team_color){
        // hopper->RunHopperMotor(0, -0.5);
        bottom = -1;
        intake->RunIntake(-1);
    }else{
        bottom = 0.0;
    }
    hopper->RunHopperMotor(top,bottom);
}

void BallManager::DisplayBallManagerInfo(){
    std::string color_state[3] ={"None","Red","Blue"};
    bool top[3] = {false};
    bool bottom[3] = {false};
    for(int i = 0; i<3; i++){
        if(position[0] == color_state[i]){
            bottom[i]=true;
        }
    }
    for(int i = 0; i<3; i++){
        if(position[1] == color_state[i]){
            top[i]=true;
        }
    }
    SmartDashboard::PutBoolean("Bottom None", bottom[0]);
    SmartDashboard::PutBoolean("Bottom Red", bottom[1]);
    SmartDashboard::PutBoolean("Bottom Blue", bottom[2]);

    SmartDashboard::PutBoolean("Top None", top[0]);
    SmartDashboard::PutBoolean("Top Red", top[1]);
    SmartDashboard::PutBoolean("Top Blue", top[2]);

    frc::SmartDashboard::PutBoolean("Pid Only", pid_only);
}
bool BallManager::IsEmpty(){
        if(position[0] == "None" && position[1] == "None"){
            return 1;
        }
        else{
            return 0;
        }
    }