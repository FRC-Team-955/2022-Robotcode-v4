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
    if(color_sensor.CheckForBall()){
        position[0] = color_sensor.ClosestColor();
    }
    if(ultrasonic.SonicDistance("in") > 3 && !color_sensor.CheckForBall() && position[0] != "None"){
        inbetween = position[0];
        position[0] = "None";
    }
    if(ultrasonic.SonicDistance("in") <= 3){
        position[1] = inbetween;
        inbetween = "None";
    }
    if(ultrasonic.SonicDistance("in") >3){
        position[1] = "None";
    }
}

void BallManager::MoveIndex()
{
    if(ultrasonic.SonicDistance("in") > 3 && position[0] != "None"){
        hopper.RunHopperMotor(0.5, 0.5);
    }
}

void BallManager::LoadHopper(){
    if(ultrasonic.SonicDistance("in") > 3){
        hopper.RunHopperMotor(0.5, 0.5);
    }
    else if(ultrasonic.SonicDistance("in") <= 3 && !color_sensor.CheckForBall()){
        hopper.RunHopperMotor(0, 0.5);
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
    if (position[1] == team_color){
        shooter.VelocityControl(target_velocity_top, target_velocity_bottom);
        if(shooter.VelocityOutput("Top") >= target_velocity_top - MechanismConst::krange_target && 
            shooter.VelocityOutput("Top") <= target_velocity_top + MechanismConst::krange_target &&
            shooter.VelocityOutput("Bottom") >= target_velocity_bottom - MechanismConst::krange_target &&
            shooter.VelocityOutput("Bottom") <= target_velocity_bottom + MechanismConst::krange_target){
            return true;
        }
        else{
            return false;
        }
    }
    //if the ball in position[1] is not the right color shoot it at the reject velocity
    else if (position[1] != team_color){
        shooter.VelocityControl(MechanismConst::kreject_target, MechanismConst::kreject_target);

        if(shooter.VelocityOutput("Top") >= MechanismConst::kreject_target - MechanismConst::krange_reject  
        && shooter.VelocityOutput("Top") <= MechanismConst::kreject_target + MechanismConst::krange_reject
        && shooter.VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::krange_reject
        && shooter.VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::krange_reject){
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
    hopper.RunHopperMotor(0.5, 0.5);
}

void BallManager::Reject(){
    shooter.VelocityControl(MechanismConst::kreject_target, MechanismConst::kreject_target);
    if(shooter.VelocityOutput("Top") >= MechanismConst::kreject_target - MechanismConst::krange_reject  
        && shooter.VelocityOutput("Top") <= MechanismConst::kreject_target + MechanismConst::krange_reject
        && shooter.VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::krange_reject
        && shooter.VelocityOutput("Bottom") <= MechanismConst::kreject_target + MechanismConst::krange_reject
        && position[1] != team_color){
        hopper.RunHopperMotor(0.5, 0);
    }
    if(position[0] != team_color){
        hopper.RunHopperMotor(0, -0.5);
        intake.RunIntake(-0.5);
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