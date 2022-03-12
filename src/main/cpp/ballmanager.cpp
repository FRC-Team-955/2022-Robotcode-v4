#include "ballmanager.h"
using namespace frc;

std::string BallManager::GetHopperState(int slot){
    if(slot != 0 && slot != 1){
        return "NULL";
    }
    else{
    return position[slot];
    }
}
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
        hopper->RunHopperMotor(.31, 0.5);
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
    //if the ball in position[1] is the right color, shoot at the inputted velocities
    shooter->VelocityControl(target_velocity_top, target_velocity_bottom);
    if(std::abs(shooter->VelocityOutput("Top")) >= target_velocity_top - MechanismConst::krange_target && 
        std::abs(shooter->VelocityOutput("Top")) <= target_velocity_top + MechanismConst::krange_target &&
        std::abs(shooter->VelocityOutput("Bottom")) >= target_velocity_bottom - MechanismConst::krange_target &&
        std::abs(shooter->VelocityOutput("Bottom")) <= target_velocity_bottom + MechanismConst::krange_target){
        return true;
    }
    else{
        return false;
    }
}

bool BallManager::RevLimelight(){  
    double target = limelight->GetShooterSpeed();
    shooter->VelocityControl(target,target);
    if(std::abs(shooter->VelocityOutput("Top")) >= target - MechanismConst::krange_target && 
        std::abs(shooter->VelocityOutput("Top")) <= target + MechanismConst::krange_target &&
        std::abs(shooter->VelocityOutput("Bottom")) >= target - MechanismConst::krange_target &&
        std::abs(shooter->VelocityOutput("Bottom")) <= target + MechanismConst::krange_target){
        return true;
    }
    else{
        return false;
    }
}

void BallManager::Shoot(){
    hopper->RunHopperMotor(0.5, 0.5);
}

void BallManager::Reject(){
    // if(position[1] != team_color){
    //     if(Rev(MechanismConst::kreject_target,MechanismConst::kreject_target)){
    //         hopper->RunHopperMotor(0.5,0);
    //     }
    // }
    // if(position[0] != team_color){
    //     hopper->RunHopperMotor(0, -0.5);
    //     intake->RunIntake(-0.5);
    // }
    if(Rev(MechanismConst::kreject_target,MechanismConst::kreject_target)
    && (position[1] != team_color || position[1] == "None")){
        hopper->RunHopperMotor(0.5,0);
    }
    if(position[0] != team_color || position[0] == "None"){
        hopper->RunHopperMotor(0, -0.5);
        intake->RunIntake(-0.5);
    }
}

void BallManager::DisplayBallManagerInfo(){
    std::string color_state[3] ={"None","Red","Blue"};
    bool top[3] = {false};
    bool bottom[3] = {false};
    for(int i = 0; i<3; i++){
        if(GetHopperState(0) == color_state[i]){
            bottom[i]=true;
        }
    }
    for(int i = 0; i<3; i++){
        if(GetHopperState(1) == color_state[i]){
            top[i]=true;
        }
    }
    SmartDashboard::PutBoolean("Bottom None", bottom[0]);
    SmartDashboard::PutBoolean("Bottom Red", bottom[1]);
    SmartDashboard::PutBoolean("Bottom Blue", bottom[2]);

    SmartDashboard::PutBoolean("Top None", top[0]);
    SmartDashboard::PutBoolean("Top Red", top[1]);
    SmartDashboard::PutBoolean("Top Blue", top[2]);
}

bool BallManager::IsEmpty(){
        if(position[0] == "None" && position[1] == "None"){
            return 1;
        }
        else{
            return 0;
        }
    }