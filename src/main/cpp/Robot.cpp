#include "Robot.h"
#include "iostream"
 
//Not our classes?
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <cameraserver/CameraServer.h>
#include <frc/Joystick.h>
#include <frc/Timer.h>
#include <frc/MotorSafety.h>
#include <frc/motorcontrol/Spark.h>
#include <frc/Compressor.h>
#include "button_toggle.h"
 
//auto
#include <frc/Filesystem.h>
#include <frc/trajectory/TrajectoryUtil.h>
#include <wpi/fs.h>
#include <AHRS.h>
 
 
//our classes
#include "auto.h"
#include "navx.h"
 
#include "drivebase.h"
#include "intake.h"
#include "hopper.h"
#include "shooter.h"
#include "colorsensor.h"
#include "ballmanager.h"
#include "elevator.h"
#include "limelight.h"
#include "BallAlign.h"
 
#include "settings.h"
 
using namespace rev;
 
DriveBase *drive;
Intake *intake;
Hopper *hopper;
Shooter *shooter;
ColorSensor *color_sensor_top;
ColorSensor *color_sensor_bot;
BallManager *ball_manager;
Elevator *elevator;
BallAlign * ballalign;
 
//Joysticks
frc::Joystick *joystick_0;
frc::Joystick *joystick_1;
//Drive
CANSparkMax *m_leftLeadMotor;
CANSparkMax *m_rightLeadMotor;
CANSparkMax *m_leftFollowMotor;
CANSparkMax *m_rightFollowMotor;
frc::DifferentialDrive *differential_drive;
Limelight *limelight;
//Intake
TalonSRX *intake_talon;
DoubleSolenoid *intake_double_solonoid;
//Hopper
TalonSRX *talon_hopper_top;
TalonSRX *talon_hopper_bottom;
bool hopper_init = false;
//Shooter
CANSparkMax *shooterneo_top;
CANSparkMax *shooterneo_bottom;
DoubleSolenoid *shooter_solenoid;
bool low_goal_mode = false;
//Elevator
TalonFX *elevator_motor;
DigitalInput *limit_switch_top;
DigitalInput *limit_switch_bottom;
DoubleSolenoid *elevator_solenoid_lock;
//Color Sensor
frc::Color *red_target_bot;
frc::Color *blue_target_bot;
frc::Color *red_target_top;
frc::Color *blue_target_top;
ColorSensorV3 *rev_color_sensor_top;
ColorSensorV3 *rev_color_sensor_bot;
ColorMatch *color_match;
//IR BreakBeam
DigitalInput *ir_break_beam;
//Compressor
Compressor *compressor;
//Timers
frc::Timer *timer_auto_wait;
frc::Timer *timer_auto;
frc::Timer *m_timer_intake;
//RGB
Spark *rgb_spark;
//auto
Auto *trajectory_auto;
int AutoState = 0;
double offset = 0;
bool TimeToAlign = false;
 
ButtonToggle toggle_intake_deploy;
ButtonToggle toggle_hopper_manual;
ButtonToggle toggle_shooter_goal;
ButtonToggle toggle_elevator_lock;
ButtonToggle toggle_compressor;
ButtonToggle toggle_pid_only;
 
 
SparkMaxRelativeEncoder *m_rightLeadMotor_encoder;
SparkMaxRelativeEncoder *m_leftLeadMotor_encoder;
 
frc::SendableChooser<std::string> m_auto_Chooser;
frc::SendableChooser<std::string> m_auto_wait_Chooser;
frc::SendableChooser<std::string> m_team_color_Chooser;
std::string ganyu_auto_selection = "Sleep";
std::string ganyu_auto_wait = "False";
 
 
// chris is so cool
// bryan ganyu simp
// ganyu is pog
// who?tao
// hi guys its bryan welcome back to my channel today im teaching you * minecraft *
// Thank you for listening to my ted talk
 
void Robot::RobotInit() {
  m_auto_Chooser.SetDefaultOption("Ganyu Wall2","Wall2Ball");
  m_auto_Chooser.AddOption("Ganyu Any*2Ball","Any*2Ball");
  m_auto_Chooser.AddOption("Ganyu 4 Ball Right","4BR");
  m_auto_Chooser.AddOption("Ganyu 3 Ball Right","3BR");
  m_auto_Chooser.AddOption("Ganyu Wall","Wall");
  // m_auto_Chooser.AddOption("Ganyu Side2*","Side2*Ball");
  m_auto_Chooser.AddOption("Ganyu Taxi","Taxi");
  frc::Shuffleboard::GetTab("Pre").Add("Auto Chooser", m_auto_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
 
  m_auto_wait_Chooser.SetDefaultOption("False","False");
  m_auto_wait_Chooser.AddOption("True","True");
  frc::Shuffleboard::GetTab("Pre").Add("Wait", m_auto_wait_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
 
 
  m_team_color_Chooser.SetDefaultOption("Blue","Blue");
  m_team_color_Chooser.AddOption("Red","Red");
  frc::Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
 
  frc::CameraServer::StartAutomaticCapture();
  cs::CvSink cvSink = frc::CameraServer::GetVideo();
  cs::CvSource outputStream = frc::CameraServer::PutVideo("Driver Cam", 640, 480);
 
  trajectory_auto = new Auto();
  frc::TrapezoidProfile<units::meters> profile{
  frc::TrapezoidProfile<units::meters>::Constraints{5_mps, 2_mps_sq},
  frc::TrapezoidProfile<units::meters>::State{5_m, 0_mps},
  frc::TrapezoidProfile<units::meters>::State{0_m, 0_mps}};
}
void Robot::RobotPeriodic() {}
void Robot::AutonomousInit() {
  Build();
  compressor->Disable();
  m_leftLeadMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_rightLeadMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_leftFollowMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_rightFollowMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_rightFollowMotor->Follow(*m_rightLeadMotor);
  m_leftFollowMotor->Follow(*m_leftLeadMotor);
  trajectory_auto->Initilize(m_leftLeadMotor, m_rightLeadMotor, m_leftLeadMotor_encoder, m_rightLeadMotor_encoder);
 
  AutoState = -1;
  m_leftLeadMotor_encoder->SetPosition(0);
  m_rightLeadMotor_encoder->SetPosition(0);
  ganyu_auto_selection = m_auto_Chooser.GetSelected();
  ganyu_auto_wait = m_auto_wait_Chooser.GetSelected();
  ball_manager->team_color = m_team_color_Chooser.GetSelected();
 
  timer_auto_wait->Reset();
  timer_auto_wait->Start();
  timer_auto->Reset();
  timer_auto->Start();
}
void Robot::AutonomousPeriodic() {
  DisplayShuffle();
  ball_manager->CheckHopperState();
  intake->PistonDown();
  std::cout<<AutoState<<std::endl;
 
  if (AutoState == -1 && ganyu_auto_wait == "True" && timer_auto_wait->Get()>5_s){
    AutoState++;
  }
  if(AutoState == -1 && timer_auto_wait->Get()>0.5_s){
    AutoState++;
  }
 
  if(ganyu_auto_selection == "4BR"){
    if(AutoState == 0){
      shooter->SolenoidUp();
      trajectory_auto->LoadTrajectory("Out4-1.wpilib.json");
      AutoState++;
    }
    if(AutoState == 1){
      //to ball
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 2){
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(ball_manager->IsFull()){
        hopper->RunHopperMotor(0,0);
        offset = limelight->GetOffset();
        AutoState++;
      }
    }
    if(AutoState == 3){
      //init shoot
      drive->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
        intake->RunIntake(0);
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }
    }
    if(AutoState == 4){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }
      if(timer_auto->Get()>0.5_s){
        shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(AutoState == 5){
      //align to offset
      drive->AlignToOffset(offset);
      if(timer_auto->Get()>0.25_s){
        AutoState++;
      }
    }
    if(AutoState == 5){
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("Out4-2.wpilib.json");
      AutoState++;
    }
    if(AutoState == 6){
      //to terminal
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(AutoState == 7){
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("Back4-2.wpilib.json");
      intake->RunIntake(1);
      if(timer_auto->Get()>0.5_s){
        AutoState++;
      }
    }
    if(AutoState == 8){
      //to goal
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      intake->RunIntake(1);
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 9){
      //shoot
      intake->RunIntake(0);
      drive->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }
    }
    if(AutoState == 10){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }
      if(timer_auto->Get()>3_s){
        // shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(timer_auto_wait->GetMatchTime()<1_s){
      std::cout<<"force shoot"<<std::endl;
      ball_manager->RevLimeLightFar();
      ball_manager->Shoot();
    }
  }
  if(ganyu_auto_selection == "3BR"){
    if(AutoState == 0){
      shooter->SolenoidUp();
      trajectory_auto->LoadTrajectory("3BOut1.wpilib.json");
      AutoState++;
    }
    if(AutoState==1){
      //to ball
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if (AutoState == 2){
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(ball_manager->IsFull()){
        hopper->RunHopperMotor(0,0);
        offset = limelight->GetOffset();
        AutoState++;
      }
    }
    if(AutoState==3){
      //init shoot
      drive ->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
        intake->RunIntake(0);
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }
    }
    if(AutoState == 4){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }
      if(timer_auto->Get()>0.5_s){
        shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if (AutoState == 5){
      drive->AlignToOffset(offset);
      if(timer_auto->Get()>0.5_s){
        AutoState++;
      }
    }
    if(AutoState == 6){
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("3BTurn.wpilib.json");
      AutoState++;
    }
    if(AutoState==7){
      //turn back
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 8){
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("3BOut2.wpilib.json");
      AutoState++;
    }
    if(AutoState==9){
      //to 2nd ball
      intake->RunIntake(1);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        offset = limelight->GetOffset();
        AutoState++;
      }
    }
    if(AutoState==10){
      //set up shoot
      intake->RunIntake(1);
      drive ->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }
    }
    if(AutoState == 11){
      //shoot rest of balls
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }
      if(timer_auto->HasElapsed(1_s)){
        shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(AutoState ==12){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    }
  }
  if(ganyu_auto_selection == "Any*2Ball"){
    if (AutoState == 0){
      shooter->SolenoidUp();
      intake->RunIntake(1);
      m_rightLeadMotor->Set(0.2);
      m_leftLeadMotor->Set(0.2);
      ball_manager->LoadHopper();
      if ( m_rightLeadMotor_encoder->GetPosition() > 35 && m_leftLeadMotor_encoder->GetPosition() > 35){
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if (AutoState == 1){
      ball_manager->LoadHopper();
      if(timer_auto->Get()> 2_s || ball_manager->IsFull()){
        AutoState++;
      }
    }
    if(AutoState== 2){
      //init shoot
      drive ->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
        intake->RunIntake(0);
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }
    }
    if(AutoState == 3){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }
      if(timer_auto->Get()>1_s){
        shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        AutoState++;
      }
    }
  }
  if(ganyu_auto_selection == "Wall"){
    shooter->SolenoidDown();
    if ((AutoState == 0) && ball_manager->RevHigh()){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    }
    if (AutoState == 1){
      m_rightLeadMotor->Set(0.3);
      m_leftLeadMotor->Set(0.3);
    }
    if (m_rightLeadMotor_encoder->GetPosition() >= 26 && m_leftLeadMotor_encoder->GetPosition() >= 26 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    }
  }else if(ganyu_auto_selection == "Wall2Ball"){
    shooter->SolenoidDown();
    ball_manager->LoadHopper();
    if ((AutoState == 0) && ball_manager->RevHigh()){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    }
    if (AutoState == 1){
      m_rightLeadMotor->Set(0.2);
      m_leftLeadMotor->Set(0.2);
      if (m_rightLeadMotor_encoder->GetPosition() >= 42 && m_leftLeadMotor_encoder->GetPosition() >= 42){
        m_rightLeadMotor->Set(0);
        m_leftLeadMotor->Set(0);
        intake->RunIntake(1);
        AutoState++;
      }
    }
    if(AutoState==2 && !ball_manager -> IsEmpty()){
      intake->RunIntake(0);
      AutoState++;
    } 
    if (AutoState == 3){
      m_rightLeadMotor->Set(-0.3);
      m_leftLeadMotor->Set(-0.3);
      if(m_rightLeadMotor_encoder->GetPosition() <= 5 && m_leftLeadMotor_encoder->GetPosition() <= 5){
        m_rightLeadMotor->Set(0);
        m_leftLeadMotor->Set(0);
        intake->RunIntake(1);
        AutoState++;
      }
    }
    if (AutoState == 4){
      intake->StopIntake();
      AutoState++;
    }
    if(AutoState == 5 && ball_manager->RevHigh()){
      ball_manager -> Shoot();
    }
    if (timer_auto->GetMatchTime()<5_s){    
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
    }
  }if(ganyu_auto_selection == "Taxi"){
    if ( AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    }
    if (AutoState == 1){
      m_rightLeadMotor->Set(0.3);
      m_leftLeadMotor->Set(0.3);
    }
    if (AutoState == 1 && m_rightLeadMotor_encoder->GetPosition() >= 10 && m_leftLeadMotor_encoder->GetPosition() >= 10){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    }
  }
}
 
void Robot::TeleopInit() {
  Build();
  m_leftFollowMotor->Follow(*m_leftLeadMotor);
  m_rightFollowMotor->Follow(*m_rightLeadMotor);
  m_leftLeadMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_rightLeadMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_leftFollowMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
  m_rightFollowMotor->SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
 
  compressor->Disable();
  frc::SmartDashboard::PutBoolean("Compressor", false);
 
  ball_manager->team_color = m_team_color_Chooser.GetSelected();
  m_timer_intake->Start();
}
void Robot::TeleopPeriodic() {
  //random updates
  DisplayShuffle();
  UpdateRGB();
  ball_manager->CheckHopperState();
  //compressor
  if (toggle_compressor.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kcompressor_toggle_button))){
    compressor->EnableDigital();
    frc::SmartDashboard::PutBoolean("Compressor", true);
  }else{
    compressor->Disable();
    frc::SmartDashboard::PutBoolean("Compressor", false);
  }
  if (toggle_pid_only.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kpid_only_toggle_button))){
    ball_manager->pid_only = true;
  }else{
    ball_manager->pid_only = false;
  }
  if (joystick_0->GetRawAxis(Joy0Const::kshoot_limelight_trigger)>0.3){
    hopper->InitShoot();
    drive->Align();
    if(shooter_solenoid->Get() == 2){
      if(limelight->ShootClose()){
        limelight->DisplayLimelightClose();
        shooter->SolenoidDown();
        if (ball_manager->RevLimeLightClose() && limelight->IsAligned()){
        ball_manager->Shoot();
        }
      }else{
        shooter->SolenoidUp();
        limelight->DisplayLimelightFar();
        if (ball_manager->RevLimeLightFar() && limelight->IsAligned()){
          ball_manager->Shoot();
        }
      }
    }else{
      limelight->DisplayLimelightFar();
      if (ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        shooter->SolenoidUp();
        ball_manager->Shoot();
      }
    }
  }else if (joystick_0->GetRawButtonPressed(Joy0Const::kball_aimbot_button)){
    
    if(ballalign->CheckOtherColor()){
      joystick_0->SetRumble(1,1);
      TimeToAlign = false;
    }else{
      TimeToAlign = true;
    }
    
  }else if(joystick_0->GetRawButton(Joy0Const::kball_aimbot_button) && TimeToAlign){
    
  drive->DriveBallAlign();
    
  }else if (joystick_0->GetRawButton(Joy0Const::kshoot_launchpad_button)){
    hopper->InitShoot();
    shooter->SolenoidUp();
    drive->Align();
    limelight->DisplayLimelightFar();
    if (ball_manager->RevLaunchPad() && limelight->IsAligned()){
      ball_manager->Shoot();
    }
  }else{
    drive ->Drive();
    shooter->SolenoidDown();
    limelight->DisplayLimelightClose();
    //The toggle for Low Goal
    if(toggle_shooter_goal.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kshooter_goal_toggle_button))){
      low_goal_mode = true;
    }else{
      low_goal_mode = false;
    }
    if(joystick_0->GetRawAxis(Joy0Const::kshoot_wall_trigger)>0.3){
      hopper->InitShoot();
      //shooting
      if(low_goal_mode){
        //low goal
        if(ball_manager->RevLow()){
          ball_manager->Shoot();
        }else{
          hopper->RunHopperMotor(0,0);
        }
      }else{
        //high goal
        if(ball_manager->RevHigh()){
          ball_manager->Shoot();
        }else{
          hopper->RunHopperMotor(0,0);
        }
      }
    }else{
      //if not shooting
      shooter->VelocityControl(0,0);
      if (joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
        //Reject code
        intake->PistonDown();
        ball_manager->Reject();
      }else if(joystick_1->GetRawButtonReleased(Joy1Const::kreject_ball_button)){
        //When you release shut off motor
        intake->StopIntake();
        hopper->RunHopperMotor(0,0);
      }else{
        //if not rejecting
        // if(toggle_intake_deploy.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
        //     intake->PistonDown();
        //     //If the intake is in the down state allow the intake to run
        //     if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)>0.3){
        //       hopper_init = true;
        //       intake->RunIntake(1);
        //       m_timer_intake->Start();
        //       m_timer_intake->Reset();
        //     }else{
        //       intake->RunIntake(0);
        //     }
        // }else{
        //   intake->PistonUp();
        // }
        //if the m_intake_timer is less than 5s then run the hopper
        //hopper in manual or auto will add the run loadhopper automatically later
        if (joystick_1->GetPOV() != -1){
        intake->RunIntake(-1);
        }else{
          intake->RunIntake(0);
        }
        if(joystick_1->GetRawButton(Joy1Const::khopper_manual_button)){
          hopper->RunHopperMotor(-joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis), -joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis));
          frc::SmartDashboard::PutBoolean("Manual Hopper", true);
          hopper->hopper_on = true;
        }else if(!m_timer_intake->HasElapsed(3_s)){
          frc::SmartDashboard::PutBoolean("Manual Hopper", false);
          hopper->hopper_on = true;
          if (hopper_init){
            ball_manager->LoadHopper();
          }
        }else{
          hopper->RunHopperMotor(0,0);
        }
      }
    }
  }

  if(toggle_intake_deploy.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
    intake->PistonDown();
    //If the intake is in the down state allow the intake to run
    if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)>0.3){
      hopper_init = true;
      intake->RunIntake(1);
      m_timer_intake->Start();
      m_timer_intake->Reset();
    }else if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button) || joystick_1->GetPOV() != -1){

    }else{
      intake->RunIntake(0);
    }
  }else{
    intake->PistonUp();
  }

  if(joystick_1->GetRawButton(Joy1Const::kelevator_allow)){
    if(toggle_elevator_lock.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kelevator_lock_button))){
      elevator->LockElevator();
    }else{
      elevator->UnlockElevator();
    }
    elevator->ElevatorMove(joystick_1->GetRawAxis(Joy1Const::kelevator_axis));
  }else{
    elevator_motor->Set(ControlMode::PercentOutput, 0);
  }
}
void Robot::DisplayShuffle() {
  drive->DisplayDriveInfo();
  intake->DisplayIntakeInfo();
  hopper->DiplayHopperInfo();
  shooter->DisplayShooterInfo();
  ball_manager->DisplayBallManagerInfo();
  elevator->DisplayElevatorInfo();
  limelight->DisplayLimelightInfo();
  frc::SmartDashboard::PutBoolean("Low Goal Mode", low_goal_mode);
  frc::SmartDashboard::PutString("Team Color", ball_manager->team_color);
}
void Robot::DisabledInit() {
  //joystick
  delete joystick_0;
  delete joystick_1;
  //drive base
  delete m_leftLeadMotor;
  delete m_rightLeadMotor;
  delete m_leftFollowMotor;
  delete m_rightFollowMotor;
  delete differential_drive;
  delete limelight;
  delete drive;
  //auto
  delete m_leftLeadMotor_encoder;
  delete m_rightLeadMotor_encoder;
  //intake
  delete intake_talon;
  delete intake_double_solonoid;
  delete intake;
  //hopper
  delete talon_hopper_top;
  delete talon_hopper_bottom;
  delete hopper;
  //shooter
  delete shooterneo_top;
  delete shooterneo_bottom;
  delete shooter_solenoid;
  delete shooter;
  //color sensor
  delete red_target_bot;
  delete blue_target_bot;
  delete red_target_top;
  delete blue_target_top;
  delete rev_color_sensor_top;
  delete rev_color_sensor_bot;
  delete color_match;
  delete color_sensor_top;
  delete color_sensor_bot;
  //Ir break beam
  delete ir_break_beam;
  //BallManager
  delete ball_manager;
  //elevator
  delete elevator_motor;
  delete limit_switch_top;
  delete limit_switch_bottom;
  delete elevator_solenoid_lock;
  delete elevator;
  //compressor
  delete compressor;
  //rgb
  delete rgb_spark;
  //timer
  delete m_timer_intake;
  //Delete();
}
void Robot::DisabledPeriodic() {}
 
void Robot::TestInit() {}
void Robot::TestPeriodic() {}
void Robot::Build(){
  //joysticks
  joystick_0 = new frc::Joystick(0);
  joystick_1 = new frc::Joystick(1);
  //drivebase
  m_leftLeadMotor = new CANSparkMax(DriveConst::kleft_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightLeadMotor = new CANSparkMax(DriveConst::kright_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_leftFollowMotor = new CANSparkMax(DriveConst::kleft_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightFollowMotor = new CANSparkMax(DriveConst::kright_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  differential_drive = new frc::DifferentialDrive(*m_leftLeadMotor,*m_rightLeadMotor);
  limelight = new Limelight();
  ballalign = new Ballalign();
  // differential_drive->SetSafetyEnabled(false);
  drive = new DriveBase(m_leftLeadMotor,m_rightLeadMotor,m_leftFollowMotor,m_rightFollowMotor,differential_drive, joystick_0, limelight);
  // xyalign = new XYalign(drive, joystick_0);
  //auto
  m_leftLeadMotor_encoder = new SparkMaxRelativeEncoder(m_leftLeadMotor->GetEncoder());
  m_rightLeadMotor_encoder = new SparkMaxRelativeEncoder(m_rightLeadMotor->GetEncoder());
  //Intake
  intake_talon = new TalonSRX(MechanismConst::kintake_motor);
  intake_double_solonoid = new DoubleSolenoid(13,PneumaticsModuleType::REVPH, MechanismConst::kintake_double_solonoid_port_forward, MechanismConst::kintake_double_solonoid_port_reverse);
  intake = new Intake(intake_talon,intake_double_solonoid);
  //Hopper
  talon_hopper_top = new TalonSRX(MechanismConst::khopper_motor_top_port);
  talon_hopper_bottom = new TalonSRX(MechanismConst::khopper_motor_bottom_port);
  hopper = new Hopper(talon_hopper_top,talon_hopper_bottom);
  //shooter
  shooterneo_top = new CANSparkMax(MechanismConst::shooter_top_port, CANSparkMax::MotorType::kBrushless);
  shooterneo_bottom = new CANSparkMax(MechanismConst::shooter_bottom_port, CANSparkMax::MotorType::kBrushless);
  shooter_solenoid = new DoubleSolenoid(13, PneumaticsModuleType::REVPH,MechanismConst::kshooter_pnumatic_port_forward, MechanismConst::kshooter_pnumatic_port_reverse);
  shooter = new Shooter(shooterneo_top, shooterneo_bottom, shooter_solenoid);
  //Color Sensor
  red_target_bot = new frc::Color(0.539917, 0.339233, 0.120972);
  blue_target_bot= new frc::Color(0.158325, 0.406372, 0.435181);
  red_target_top= new frc::Color(0.666, 0.333, 0.001);
  blue_target_top= new frc::Color(0.250122, 0.500122, 0.250122);
  rev_color_sensor_bot = new ColorSensorV3(frc::I2C::Port::kOnboard);
  rev_color_sensor_top = new ColorSensorV3(frc::I2C::Port::kMXP);
  color_match = new ColorMatch();
  color_sensor_bot= new ColorSensor(rev_color_sensor_bot,color_match,red_target_bot, blue_target_bot);
  color_sensor_top= new ColorSensor(rev_color_sensor_top,color_match, red_target_top, blue_target_top);
  //Ir Break Beam
  ir_break_beam = new DigitalInput(SensorConst::kir_break_beam_port);
  //BallManager
  ball_manager = new BallManager(intake,hopper,shooter, color_sensor_bot, color_sensor_top,limelight);
  //elevator
  elevator_motor = new TalonFX(MechanismConst::kelevator_motor_port);
  limit_switch_top = new DigitalInput(SensorConst::limit_switch_top_port);
  limit_switch_bottom = new DigitalInput(SensorConst::limit_switch_bottom_port);
  elevator_solenoid_lock = new DoubleSolenoid(13, PneumaticsModuleType::REVPH, MechanismConst::kelevator_pnumatic_port_forward, MechanismConst::kelevator_pnumatic_port_reverse);
  elevator = new Elevator(elevator_motor,limit_switch_top,limit_switch_bottom,elevator_solenoid_lock);
  //compressor
  compressor = new Compressor(13,frc::PneumaticsModuleType::REVPH);
  //rgb
  rgb_spark = new Spark(0);
  //timer
  timer_auto_wait = new frc::Timer();
  timer_auto = new frc::Timer();
  m_timer_intake = new frc::Timer();

  toggle_compressor.SetToggleState(false);
  toggle_elevator_lock.SetToggleState(false);
  toggle_pid_only.SetToggleState(false);
}
void Robot::UpdateRGB(){
  if(toggle_compressor.GetToggleState()){
    rgb_spark->Set(0.93);
  }
  else if(limit_switch_top->Get() == 1){
    rgb_spark->Set(0.77);
  }
  else if (elevator_solenoid_lock->Get() == 1){
    rgb_spark->Set(0.69);
  } 
  else if (joystick_0->GetPOV()==180){
    rgb_spark->Set(-0.07);
  }
  else{
    rgb_spark->Set(-0.99);
  }
}
#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
 

