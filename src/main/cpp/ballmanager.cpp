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
        // hopper->RunHopperMotor(1, 1);
        hopper->RunHopperMotor(.3, 0.5);
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
    //if(position[1] == team_color)
    if (true){
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
    //if the ball in position[1] is not the right color shoot it at the reject velocity
    else if (position[1] != team_color){
        shooter->VelocityControl(MechanismConst::kreject_target, MechanismConst::kreject_target);
        if(shooter->VelocityOutput("Top") >= MechanismConst::kreject_target - MechanismConst::kreject_range  
        && shooter->VelocityOutput("Top") <= MechanismConst::kreject_target + MechanismConst::kreject_range
        && shooter->VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::kreject_range
        && shooter->VelocityOutput("Bottom") >= MechanismConst::kreject_target - MechanismConst::kreject_range){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

void BallManager::Shoot(){
    hopper->RunHopperMotor(0.5, 0.5);
}

void BallManager::Reject(){
    shooter->VelocityControl(MechanismConst::kreject_target, MechanismConst::kreject_target);
    if(shooter->VelocityOutput("Top") >= MechanismConst::kreject_target - MechanismConst::kreject_range  
        && shooter->VelocityOutput("Top") <= MechanismConst::kreject_target + MechanismConst::kreject_range
        && shooter->VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::kreject_range
        && shooter->VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::kreject_range
        && position[1] != team_color){
        hopper->RunHopperMotor(0.5, 0);
    }
    if(position[0] != team_color){
        hopper->RunHopperMotor(0, -0.5);
        intake->RunIntake(-0.5);
    }
}

void BallManager::DisplayBallManagerInfo(){
    //frc::ShuffleboardTab& tabpre
    // frc::ShuffleboardLayout& ball_layout = frc::Shuffleboard::GetTab("Telop").GetLayout("Ball Slots","List Layout");
    // frc::ShuffleboardLayout& ball_top = frc::Shuffleboard::GetTab("Telop").GetLayout("Top Ball","Grid");
    // frc::ShuffleboardLayout& ball_bottom = frc::Shuffleboard::GetTab("Telop").GetLayout("Bottom Ball","Grid");
    // ball_layout.WithSize(4,2);
    // ball_layout.Add("Top Ball", ball_top);

    std::string color_state[3] ={"None","Red","Blue"};
    bool top[3] = {false};
    bool bottom[3] = {false};

    for(int i = 0; i<3; i++){
        if(GetHopperState(0) == color_state[i]){
            top[i]=true;
        }
    }
    for(int i = 0; i<3; i++){
        if(GetHopperState(1) == color_state[i]){
            bottom[i]=true;
        }
    }
    frc::Shuffleboard::GetTab("Telop").Add("Bottom None", bottom[0]);
    frc::Shuffleboard::GetTab("Telop").Add("Bottom Red", bottom[1]);
    frc::Shuffleboard::GetTab("Telop").Add("Bottom Blue", bottom[2]);

    frc::Shuffleboard::GetTab("Telop").Add("Top None", top[0]);
    frc::Shuffleboard::GetTab("Telop").Add("Top Red", top[1]);
    frc::Shuffleboard::GetTab("Telop").Add("Top Blue", top[2]);

    // if(GetHopperState(0)=="None"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom None", true);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Blue", false);
    // }else if (GetHopperState(0)=="Red"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Red", true);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Blue", false);
    // }else if (GetHopperState(0)=="Blue"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Blue", true);
    // }else{
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Bottom Blue", false);
    // }
    // if(GetHopperState(1)=="None"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Top None", true);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Blue", false);
    // }else if (GetHopperState(1)=="Red"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Top None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Red", true);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Blue", false);
    // }else if (GetHopperState(1)=="Blue"){
    //     frc::Shuffleboard::GetTab("Telop").Add("Top None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Blue", true);
    // }else{
    //     frc::Shuffleboard::GetTab("Telop").Add("Top None", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Red", false);
    //     frc::Shuffleboard::GetTab("Telop").Add("Top Blue", false);
    // }
    
}

bool BallManager::IsEmpty(){
        if(position[0] == "None" && position[1] == "None"){
            return 1;
        }
        else{
            return 0;
        }
    }