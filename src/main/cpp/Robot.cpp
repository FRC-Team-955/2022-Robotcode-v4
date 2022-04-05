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
Spark *rgb_spark_color_sensor;
//auto
Auto *trajectory_auto;
int AutoState = 0;
double offset = 0;
 
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
  // m_auto_Chooser.AddOption("Ganyu Gamer Auto","5BR");
  m_auto_Chooser.AddOption("Ganyu Wall","Wall");

  frc::Shuffleboard::GetTab("Pre").Add("Auto Chooser", m_auto_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
 
  m_auto_wait_Chooser.SetDefaultOption("False","False");
  m_auto_wait_Chooser.AddOption("True","True");
  frc::Shuffleboard::GetTab("Pre").Add("Wait", m_auto_wait_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
 
 
  m_team_color_Chooser.SetDefaultOption("Blue","Blue");
  m_team_color_Chooser.AddOption("Red","Red");
  frc::Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
  
  frc::SmartDashboard::PutNumber("Velocity Offset Limelight", 0);
  frc::SmartDashboard::PutNumber("Velocity Offset Fixed", 0);
  trajectory_auto = new Auto();
}
void Robot::RobotPeriodic() {}
void Robot::AutonomousInit() {
  Build();
  DisplayShuffle();
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
  else if(AutoState == -1){
    AutoState++;
  }
  
  // if(ganyu_auto_selection == "5BR"){
  //   if(AutoState == 0){
  //     shooter->SolenoidDown();
  //     //init shoot
  //     if(ball_manager->RevLimeLightClose()){
  //       ball_manager -> Shoot();
  //       intake->RunIntake(0);
  //       if (ball_manager -> IsEmpty()){
  //         timer_auto->Reset();
  //         timer_auto->Start();
  //         AutoState++;
  //       }
  //     }
  //   }
  //   if(AutoState == 1){
  //     //shoot rest
  //     if(ball_manager->RevLimeLightClose()){
  //       ball_manager -> Shoot();
  //     }
  //     if(timer_auto->Get()>0.6_s){
  //       shooter->ShootPercentOutput(0,0);
  //       hopper->RunHopperMotor(0,0);
  //       timer_auto->Reset();
  //       timer_auto->Start();
  //       AutoState++;
  //     }
  //   }

  //   if(AutoState == 2){
  //     intake->RunIntake(0.8);
  //     ball_manager->LoadHopper();
  //     shooter->SolenoidUp();
  //     trajectory_auto->LoadTrajectory("5BallOut1.wpilib.json");
  //     AutoState++;
  //   }
    
  //   if(AutoState == 3){
  //     //to ball
  //     intake->RunIntake(0.8);
  //     ball_manager->LoadHopper();
  //     ball_manager->RevLimeLightFar();
  //     if(trajectory_auto->FollowTrajectory()){
  //       AutoState++;
  //     }
  //   }
  //   if(AutoState == 4){
  //     intake->RunIntake(0.8);
  //     ball_manager->RevLimeLightFar();
  //     AutoState++;
  //   }
  //   if(AutoState == 5){
  //     //init shoot
  //     if(ball_manager->RevLimeLightFar()){
  //       ball_manager -> Shoot();
  //       intake->RunIntake(0);
  //       if (ball_manager -> IsEmpty()){
  //         timer_auto->Reset();
  //         timer_auto->Start();
  //         AutoState++;
  //       }
  //     }else{
  //       ball_manager->LoadHopper();
  //     }
  //   }
  //   if(AutoState == 6){
  //     //shoot rest
  //     if(ball_manager->RevLimeLightFar()){
  //       ball_manager -> Shoot();
  //     }
  //     if(timer_auto->Get()>0.8_s){
  //       shooter->ShootPercentOutput(0,0);
  //       hopper->RunHopperMotor(0,0);
  //       timer_auto->Reset();
  //       timer_auto->Start();
  //       AutoState++;
  //     }
  //   }
  //   if(AutoState == 7){
  //     intake->RunIntake(0.8);
  //     ball_manager->LoadHopper();
  //     trajectory_auto->LoadTrajectory("5BallOut2.wpilib.json");
  //     AutoState++;
  //   }
  //   if(AutoState == 8){
  //     //to terminal and back
  //     intake->RunIntake(0.8);
  //     ball_manager->LoadHopper();
  //     if(trajectory_auto->FollowTrajectory()){
  //       timer_auto->Reset();
  //       timer_auto->Start();
  //       AutoState++;
  //     }
  //   }
  //   if(AutoState == 9){
  //     //shoot
  //     intake->RunIntake(0);
  //     if(ball_manager->RevLimeLightFar()){
  //       ball_manager -> Shoot();
  //       if (ball_manager -> IsEmpty()){
  //         timer_auto->Reset();
  //         timer_auto->Start();
  //         AutoState++;
  //       }
  //     }
  //   }
  //   if(AutoState == 10){
  //     //shoot rest
  //     if(ball_manager->RevLimeLightFar()){
  //       ball_manager -> Shoot();
  //     }
  //     if(timer_auto->Get()>3_s){
  //       // shooter->ShootPercentOutput(0,0);
  //       hopper->RunHopperMotor(0,0);
  //       timer_auto->Reset();
  //       timer_auto->Start();
  //       AutoState++;
  //     }
  //   }
  //   if(timer_auto_wait->GetMatchTime()<1_s){
  //     ball_manager->RevLimeLightFar();
  //     ball_manager->Shoot();
  //   }
  // }
 
  if(ganyu_auto_selection == "4BR"){
    if(AutoState == 0){
      shooter->SolenoidUp();
      trajectory_auto->LoadTrajectory("Out4-1.wpilib.json");
      AutoState++;
    }
    if(AutoState == 1){
      //to ball
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 2){
      intake->RunIntake(0.8);
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
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
    if(AutoState == 4){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }else{
        hopper->RunHopperMotor(0,0);
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
      if(timer_auto->Get()>1_s){
        AutoState++;
      }
    }
    if(AutoState == 6){
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("Out4-2.wpilib.json");
      AutoState++;
    }
    if(AutoState == 7){
      //to terminal
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(AutoState == 8){
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("Back4-2.wpilib.json");
      intake->RunIntake(0.8);
      if(timer_auto->Get()>0.5_s){
        AutoState++;
      }
    }
    if(AutoState == 9){
      //to goal
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      intake->RunIntake(0.8);
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 10){
      //shoot
      intake->RunIntake(0);
      drive->AlignAuto();
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
    if(AutoState == 11){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager -> Shoot();
      }else{
        hopper->RunHopperMotor(0,0);
      }
      if(timer_auto->Get()>5_s){
        // shooter->ShootPercentOutput(0,0);
        hopper->RunHopperMotor(0,0);
        timer_auto->Reset();
        timer_auto->Start();
        AutoState++;
      }
    }
    if(timer_auto_wait->GetMatchTime()<1_s){
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
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      ball_manager->RevLimeLightFar();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if (AutoState == 2){
      intake->RunIntake(0.8);
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
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
        intake->RunIntake(0);
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }else{
        ball_manager->LoadHopper();
      }
    }
    if(AutoState == 4){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
      }else{
        ball_manager->LoadHopper();
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
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("3BTurn.wpilib.json");
      AutoState++;
    }
    if(AutoState==7){
      //turn back
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        AutoState++;
      }
    }
    if(AutoState == 8){
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      trajectory_auto->LoadTrajectory("3BOut2.wpilib.json");
      AutoState++;
    }
    if(AutoState==9){
      //to 2nd ball
      intake->RunIntake(0.8);
      ball_manager->LoadHopper();
      if(trajectory_auto->FollowTrajectory()){
        offset = limelight->GetOffset();
        AutoState++;
      }
    }
    if(AutoState==10){
      //set up shoot
      intake->RunIntake(0.8);
      drive ->Align();
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }else{
        ball_manager->LoadHopper();
      }
    }
    if(AutoState == 11){
      //shoot rest of balls
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
      }else{
        ball_manager->LoadHopper();
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
      intake->RunIntake(0.8);
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
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
        intake->RunIntake(0);
        if (ball_manager -> IsEmpty()){
          timer_auto->Reset();
          timer_auto->Start();
          AutoState++;
        }
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
    if(AutoState == 3){
      //shoot rest
      if(ball_manager->RevLimeLightFar() && limelight->IsAlignedAuto()){
        ball_manager -> Shoot();
      }else{
        hopper->RunHopperMotor(0,0);
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
    }else{
        hopper->RunHopperMotor(0,0);
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
        intake->RunIntake(0.8);
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
        intake->RunIntake(0.8);
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
  // frc::SmartDashboard::PutBoolean("Compressor", false);
 
  ball_manager->team_color = m_team_color_Chooser.GetSelected();
  hopper_init = false;
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
    // frc::SmartDashboard::PutBoolean("Compressor", true);
  }else{
    compressor->Disable();
    // frc::SmartDashboard::PutBoolean("Compressor", false);
  }
  //The toggle for pid mode
  if (toggle_pid_only.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kpid_only_toggle_button))){
    ball_manager->pid_only = true;
  }else{
    ball_manager->pid_only = false;
  }
  //The toggle for Low Goal
  if(toggle_shooter_goal.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kshooter_goal_toggle_button))){
    low_goal_mode = true;
  }else{
    low_goal_mode = false;
  }

  if (joystick_0->GetRawAxis(Joy0Const::kshoot_limelight_trigger)>0.3){
    hopper->InitShoot();
    drive->Align();
    if(ball_manager->ShootFromClose(shooter_solenoid->Get())){
      shooter->SolenoidDown();
      limelight->DisplayLimelightClose();
      if (ball_manager->RevLimeLightClose() && limelight->IsAligned()){
        ball_manager->Shoot();
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }else{
      shooter->SolenoidUp();
      limelight->DisplayLimelightFar();
      if (ball_manager->RevLimeLightFar() && limelight->IsAligned()){
        ball_manager->Shoot();
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
  }else if (joystick_0->GetRawButton(Joy0Const::kshoot_launchpad_button)){
    hopper->InitShoot();
    shooter->SolenoidUp();
    drive->Align();
    limelight->DisplayLimelightFar();
    if (ball_manager->RevLaunchPad() && limelight->IsAligned()){
      ball_manager->Shoot();
    }else{
      hopper->RunHopperMotor(0,0);
    }
  }
  else{
    drive->Drive();
    shooter->SolenoidDown();
    limelight->DisplayLimelightClose();
    if(joystick_0->GetRawAxis(Joy0Const::kshoot_wall_trigger)>0.3){
      hopper->InitShoot();
      shooter->SolenoidDown();
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
      //Reject code
      if (joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
        shooter->SolenoidDown();

        hopper->hopper_on = true;
        toggle_intake_deploy.SetToggleState(true);
        ball_manager->Reject();
      }else if(joystick_1->GetPOV() == 0){
        shooter->SolenoidDown();
        hopper->hopper_on = true;
        toggle_intake_deploy.SetToggleState(true);
        ball_manager->RejectTop();
      }else if(joystick_1->GetPOV() == 180){
        shooter->SolenoidDown();
        hopper->hopper_on = true;
        toggle_intake_deploy.SetToggleState(true);
        ball_manager->RejectBottom();
      }else{
        //if not rejecting
        //if the m_intake_timer is less than 5s then run the hopper
        //hopper in manual or auto will add the run loadhopper automatically later
        if(joystick_1->GetRawButton(Joy1Const::khopper_manual_button)){
          hopper->RunHopperMotor(-joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis), -joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis));
          // frc::SmartDashboard::PutBoolean("Manual Hopper", true);
          hopper->hopper_on = true;
        }else if(!m_timer_intake->HasElapsed(3_s)){
          // frc::SmartDashboard::PutBoolean("Manual Hopper", false);
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
    if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button)|| joystick_1->GetPOV() == 0|| joystick_1->GetPOV() == 180){
      //if running any reject dont run or shut off the intake
    }else if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)>0.3){
      hopper_init = true;
      intake->RunIntake(0.8);
      m_timer_intake->Start();
      m_timer_intake->Reset();
    }else if (joystick_1->GetPOV() == 270){
      intake->RunIntake(-1);
    }else{
      intake->RunIntake(0);
    }
  }else{
    intake->PistonUp();
    intake->RunIntake(0);
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
  // drive->DisplayDriveInfo();
  // intake->DisplayIntakeInfo();
  // hopper->DiplayHopperInfo();
  shooter->DisplayShooterInfo();
  ball_manager->DisplayBallManagerInfo();
  elevator->DisplayElevatorInfo();
  limelight->DisplayLimelightInfo();
  frc::SmartDashboard::PutBoolean("Low Goal Mode", low_goal_mode);
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
  delete rgb_spark_color_sensor;
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
  red_target_top= new frc::Color(0.680054, 0.200073, 0.119995); //g
  blue_target_top= new frc::Color(0.137085, 0.274048, 0.588989); //g

  red_target_bot = new frc::Color(0.5670717, 0.325073, 0.108032); //g
  blue_target_bot= new frc::Color(0.165161, 0.394653, 0.440063); //g

  rev_color_sensor_bot = new ColorSensorV3(frc::I2C::Port::kOnboard);
  rev_color_sensor_top = new ColorSensorV3(frc::I2C::Port::kMXP);
  color_match = new ColorMatch();
  color_sensor_top= new ColorSensor(rev_color_sensor_top,color_match, red_target_top, blue_target_top);
  color_sensor_bot= new ColorSensor(rev_color_sensor_bot,color_match,red_target_bot, blue_target_bot);
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
  rgb_spark_color_sensor = new Spark(1);

  //timer
  timer_auto_wait = new frc::Timer();
  timer_auto = new frc::Timer();
  m_timer_intake = new frc::Timer();

  toggle_compressor.SetToggleState(false);
  toggle_elevator_lock.SetToggleState(false);
  toggle_pid_only.SetToggleState(false);
}
void Robot::UpdateRGB(){
  rgb_spark_color_sensor->Set(0.93);
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
 
/*
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXXKK0000KKKXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNNXXKKK00OkkkxdddooodddddddxkO0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXK0000000OOOOkxxxddddddddddddxxxxddddxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXKK000KXXXKK0OOkxddoooooooodddddddddxxxxxxxdxOKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNK000KKKKKXXXXKOkxoooxkkO000OOOkxdddddxxxxxxxxxxxxkXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWK00KKKKKK0KKKXXOdodk0XWWMMMMMMMMWWNK0kxdddxxxxxxxxxxxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMX00KKKKKKK000KKKkdx0NMMMMMMMMMMMMMMMMMMMWX0OxddxxxxxxxdkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNXXXXNWWMMMMMMMMMMMMMMX00KKKKKK0KKK0KOxxKWMMMMMMMMMMMMMMMMMMMMMMMMMWX0kddxxxxdxKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNXOxdl:;,,,,,;:cokKNWMMMMMMMMN0OKKKKK000KK00OxOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxdxxxdxKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOxo:,..........'',;;;::lx0NMMMMMNOOKKK00000KKK0kOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0xxxdxXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXxc,..........'',;:::;;;,,,'.,:d0NMMWOOKKK000O0KKKOkKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOxkd0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXd;.........',,,;:::;,'''''''''''.':xXWXO0KKK0OO0KKKkkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOdx0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx;........',,,,;:::;,'''''''''''''''..;kXKO0KK0OOO0KKkkNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKxxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0c.........,,,,,,:cc:,''''''''''''''''''.'lKKOOK00OO00KkxXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;........',,,,,,;:cc;'''''''''',,,,;;;;;;,;oOkxx000OO0KOoxOOOOOOOOOO0000KKXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd'........',,,,,,,;:c:,'',;:cloddddddxxxxxdddddooodk00OO0OocodxxkkkkkxxxxxddxxkO0KNWMMMMMMMMMMMMWNX0OkxdxxxxkO0KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd'........',,,,'',,,;:::ldxO000OOOOOOOOOOkkkkkkkkxxxxkOO0O0kodkkkkkkOOOkkkxxxxdddoddk0XWMMMMWNKkdl:,'..........'',:cokKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx'.........',,,...',,:ok0KKK0OOOO0000K000000000OkkkkOOOkxxkO0OxxxxxkkkOOOOOOOOOOkxddolloxkOkdc;'......................',cok0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO;..........',,'..'cdO0KKK00000000000000KKKKOxdooddxxk0000OxdxOOxxkkOOOOO00KKKXXXXKK0Oxoc;'...............''''''''''..'',,'',coOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNo...........',,,cd0XXKK00000000000KKKKKKK0OxdodkO000OOO0KKKKOdoxkOO000000KKKKKKKKKKXXXXK0ko:,..........''''''''''''''''''''',''';lkNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXc...........,cd0XXKK0000000000000KKKKKK0kxxkO000Okxxkkk0KKKXK00Okxdolccclodk0KKKKKK0KKKKKKK0ko;'..',;;::::::::::;;;;,,,''''''',,,'':xXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:..........:kKXXKK000000000KKK000KKKK0OxkO00OkxxxkO0KKKKXXXXXK0dlccldxxxxxddddxk0KKK0O000KKKK0Oxl::ccccccccccccccccccc::;;,,''',,,,,,:xXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0;.......'ckKXKK0000000KKKK00000000KK0kk000OkkOO00KKKKXKKKKKKKOoclddx0KKKKKKKKOkdddkO000OO00000KK0Odc:cccccccccccccccccccccc::;,''''''.'cONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0;......ckXXK000000KKKKKK000000000KKOO000OO00KKKKKKKKKKKK00KKOdldkO000KKKKKKKKKKK0kxxkO000OOOO00KKXK0dc:cccccccccccccccccccccccc:;,,''...'oKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMk,...'ckKXK0O0KKKKKKKK0000000000KK000K000KKKKKKKKKKKKKKKKKKK0xdk0KKKKKKKKK00KKKKKKK00OkkO00000000KKXXXOo:::cccccccccccccccccccccccc:;,'....;kNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd'';oOKXK0O0KXXXXXKK00000000000KK00KKKKKKKKKKKKKKKKKKKKKKKKKOxOKKKKKKKKKKK00000KKKKKKK00OO0KKK000KKKXXXKkl:cccccccccccccccccccccccccc::;'...'dXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKc,o0XXK0O0KNNNXXKK000000000000KK0KKKXXXKKKKKKKKKKXXXKKKKKKK0kOKKKKKKKKKKKKKKKKKKKKKKKKKKK000KKKK00KKXXXXXKxlcccc::;;;,,,,,,;;;::ccccccc::,'..'lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOdkXXK000XNNNNXK0000KXXKKKKKKXKK0KXXXNXXKKKXXKKXXXNXXKKKKKKKOOKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXXXXXK0xl:;,'''''''''''''',,;::ccccc:,'..'lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNK000XX00KKXXXXXXK000KXXNNNNNNNNNX0KXNNNNNXKXXNNXXXNNWWNXKXXKKKK00KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXXXXKOOkc,'''''''''''''''''',,;:ccccc;,..'lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWN0O0KXXK00KXXXK0KK000KXNNNNNNNNNNNK0KXNNNNNXKKXNNNXXXNWWWWNXXXXXXKKKKKKKKKKKKKKKKKKKXXXXXXXXXXXKKKKKKKKKKKKKKXXXXX0kkOo;'''''''''''''''''',,,,;cccc:;'.'oNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0O0XXXK000KKKK000000KXXXXXXXXXXNNNX00XXXXXXXKKXXXXXXXXNWWWWWNNNNNXKKKKKKKKKKKKKKKKKXXXXXXXXXXXXXXXKKKKKKKKKKKKKXXXXXKkk0k:'''''',,;::;,'',,,,,,,;:cccc;'.,xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0k0XNK0OO0000K00OO000000000KKKKKXXXX00KXXXXXXXKKXXKKKKXXNWWWWWWWWNXKKKXXNXKXXKKKKKKKXXXXXXXXXXXXXXXXKKKKKKKKKKKKKKKXXXXX00KOc,'',;:ccccc:;,,,,,,,,,,;cclc:,':OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOO0XNXKOO000000OOOO0000000000KKKXXXXX0O0XXXXXXXKKKKKKKKKXXXXNNWWWWNXXKKKXNWNXXNNXKKXXXXXXXXXXXXXXXXXXXXKKKKXXXKKKKXKKKXXXXXXKXOl;::ccccccccc::;,,,,,,,,;:clc:,'lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOk0XXXKOO000000OOO0000000KK000KKXXXXXX0kOKXXXXXXXKKKKKKKKKXXXXXXNWWWNXXKKKXNNWNNNNNXXXXXXXKKXXXXNNNNXXXKXXKKKXNNNXXXXXXK0KXXXXXXX0dcccccccccccccc:;,,,,,,,;:clc:,,dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0O0XXXK0OO000OOkkO0KK0KKKKKK0O0KXXXXXXXX0kOKXXXXXXX00KKKKKKKXXXXXXXNWNXXX00KNNNWNNNWWNXXXXXXXKXXXXXNWWNNXXKKXXXXNNNNNXNNXXK00KXXXXXX0o::ccccccc:::;;,,,,,,,,,,;clc:,,kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNK0KXXXK0OkO0OkkkkO0KKKKKKXXK0O0KXXXXXXXXXK0kk0KKKKKXXOOKKKKKKKKXXXXXXXXXXKX0OKXNNNNNNWWNNXXXXXXXKXNXXXNNNNNXXXXNNNWNNWWNNXXXXK00KXXXXXX0l;,;:;;;,,,,,,,,,,,,,,,,,;:cc:,cKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX00XXKKK0kkkOkxxkO0KKKKKKXXKK00KKXXXXXXXXKKX0kx0KXXKKXXOxOKKKKKKKXXXXXXXXXXXX0O0XXXXKKXNNNNNNXXXXKXXNNXXNWNNNNNNNNWNWWWWWWNXXXXXKO0XXXXXXXOl,'''''''',,,,,,,,,,,,,,,;:cc:;xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX00KKKKK0Oxxxkxxk0KKKKKKKKXXK00KKXXXXXXXXKKKKXKkdkKXXKKXX0dx0KKKKXKXXXXXXXXXXXXKO0XXKKKKKXXNNNNXXXXXXXNNXXXNNNNNNNXXNNNWWWWWWNXXXXXKO0XXXXXXX0o;'''''''',,,,,,,,,,,,,,,;:cc;oXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXK0KK00000OxxxxxxO0KKKKKKKKXXK0KKXXXXXXXXXK0KKXXKOod0XKKKKX0xox0KKKKXXXXXXXXXXXXXK0OKXKKKKKKKXXXXXKXNXXNNNXKXNNXXKXXXXXXXXNNWWWNNXXXXX0O0XXXXXXX0o;'''''''',,,,,,,,,,,,,,,;:c:c0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNXNWMMMMMMMMMWXK0000000000kxxxxxk0KKKKKKKKKKKK0KXXXXXXXXXXK00KKKXK0dlkKKKKKXKOkxxOKKKKXXXXXXXXXXXXXKO0XXKKKKKKKXXXXXXNNXNNNNXKXXKKKKKKKKKKKXXNNNNNXXXXXX0OKXXXXXXX0o,'''''''''',,,,,,,,,,,,,;:::kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKO0KXXXXXXXK0OO000OO0000kxxxxkO0KKKKKKKKKXK00KXXXXXXXXXXK00KKKKKKOxooOKXXXKK000kxk0KKKXXXXXXXXXXXXK0OKXXKKKKKXXXXXXXXXXXXNXXKXKKKKKKKKKKKXXXXXNNXXXXXXXKO0XXXXXXXXOl,''''''''''',,,,,,,,,,',,:;dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKkxkkkOOOOO0OOOOkkO000kxxxxkO0K00KKKKKKKKOOKXXXXXXXXXXK0OKXKKKK0OOkddkKXXXKKKXX0kkO0KKKKXXXXXXXXXXKOOKXXXXXXXXXXXXXXXKKXXXKXXKKKKKKKXXXKKXXXXNNXXXXXXXX0O0XXKKXXXXk:'''''''''''''''''''''''',;lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNOxddxxxkkkkkxxxO000kxxkkk00000KKKKKKKKOOKXKXXXXXXXXK0k0KKKKKKOOKKOxdx0KXXXKKXXX0kxO0KKKKXXXXXXXKK0O0XXXXXXXXXXXXXXXXXXXXXXXXKKXK0KXXXXXXXXXXNXXXXXXXXXOOKXXKKXXXKx;''''''''''''''''''''''',;:OMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOxdddddddddxO00Oxdxkkk00000KKKKKKK0OOKKKKKKXXXKKK0kOKKKKKXKO0KXK0kddk0KXXKKXXXKOxxk0KKKKKKKKKKKK0O0XXXXXXXXXXXXXXXXXXXXXXXKKXK00XXXXXXXXXXXXXXXXXXXX0kOKXK0KKKXKo'.'''''''''''''''''''''';;dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNKOkxddddxO00OxdxkkO00000KKKKKKK0OO0KKKKKKXXXKKKkxOKKKKKK00KKXXXK0OkxkO0KKKKXXXKOkxkO00KKKKKKKKKK00XXXXXXXXXXXXXXXXXK0KXXXXXK00XXXXXXXXXXXXXXXXXXXXKOk0XK00K0KXO:...'''''''''''''''''''';;lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWXKxdkO00kddkkOO000O0KKKKKK00Ok0KKKK00KXKKKKOddOKKKKKK00KXXXXXXXK0OOOO000000KKK0kkkkkO0KKKKKKKK00KXXXXXXXXXXXXXXXKO0XXXXXX00KXXXXXXXXXXXXXXXXXXXX0kOKK00000KKx,...''''''''''''''''''',::kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXkk000kodkOOO000OO0KKKKK0OOkk0KKKOO0KKKKK0dcd0KKKKKK00XXXXXXXXXXXKK00OOOOO0KXXK0kxxxxkkkO0KKKKK00KXXXXXXXXXXXXXXOkKXXXXX00KXXXXXXXXXXKXXXXXXXXX0kk0K0O0000K0c.....''''''''''''''''',::dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOxO00kodkOOOO00OO00KKKKOOOkkO00KOkOO0KKK0xlcd0KKKKKK00KKKXXXXXXXXXXXXKK000KXXXXXKOkxkxxddxkkO00KK0KKKKKXXXXXXXXX0kOXXXXX00KXXXXXXXKKKKXXXXXXXXX0OkO00O0000KKx;......''''''''''''''',;coKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNWNKkxO00kdokOOOOO0OO00KKKKOkOkdkOO00kkOOO0KKOoooxO0KKKK0O0KKKKKXXXXXXXXXXXXXXXXXXXXXXXKOkxxxxxxxdk0KKKKKKXXXXXXXXXXX0xOKXXXX00KXXXXXXXKKKKXXXXXXXXXKOkkOOOO000KKKd'......''''''''''''''',:ckWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0OxxxkkkkxoxOOOOOOOO0O0KKK0kkkxdxOO0OxkOOO0K0xooodOO0KKK0O0KKKKKKXXXXXXXXXXXXXXXXXXXXXXXKKOkxxdxxox0XXXXXXXXXXXXXXXXX0xkKXXXKO0XXXXXXXXXKKKXXXKKKKKKKOkkkOOO000KKK0d;......''''''''''''''';:c0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNXKOddxdxOOOOOOOOOOO0KK0kxkxddxO00kxOOOOkOkololokkO0KK0kOKKKKKKKXXXXXXXXXXXXXXXXXXKKKKKKKKKOxdoloOKXXXXXXXXXXXXXXXX0xk0KXXKO0XXXXXXXXXKKKXXXK0KKKK0kxkkkOOO00KKKKK0xo:,...'''''''''''''',:,lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0ddxddkOOOOOOOOOO0KKK0xxxxxddk0OdokOOOOOxodxooxOkO0K0xk0KKKKKKKXXXXXXXXXXXXXXXKKKKKKKKKK0kxodxox0KXXXXXXXXXXXXXXXOxk0KXX0OKXXXXXXXXXKKKXXX0O0KKK0kxxxkOOOOO0KKKKKKKK0kdl::;,,,''''''''',,,xNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOddkkdxOOOOOOOOOO00KKKOxxxxddoxOOoldk00OOdd0K0ddOOOO0OxxOKKKKKKKKXXXXXKXXXXXXXKKKKKKK000Okxollolok0XXXXXXXXXXXXXXKkxxOKKKOOKXXXXXXXXKKKKXXX0kkKKK0kkkxxkOOOOkkO0000KKKKKKK0Okdc,''''''''''.'l0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOdxOOxdkOOOOOOO0OO0KKK0kdxxdddoldkxxdok00OdkXWXkoxOOOOOkxk0KKKKKKKKXXXKKKXXXXKKKKKKKK000Okxocloolcdk0KXXXXXKXXXXXX0kxxO0K0O0KXXXXXXXXKKKXXXX0xx0KKOOOkxkOOOO0OkxxkkOOOOOOOxdl:,'''''''''''''..,l0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxkOOkddkOOOOOOOO00KK0OOkdxdddddolddkOxodxkdkNWWXkoxOOOOkxxk0KKKKKKKXXK0KKKKXKKKKKKKK0O0OxxoldkxddoldkO0KKKKKKXXXXKOxxkO000O0XXXXXXXKKKKKXXKXKxoOK0OOOkxkOO0OO00xddddddolccl;....'''''''''',;;'..,lkKWMMMMMMMMMMMMMMMMMMWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOOOOOkddkOOOOOOOO00KK0xxxdddddddoddldO0Kkllld0XXXKxoxOOOOkkOO0KKKKKKXKK0KKKKKKKKKKKK0O0OddookKNNXK0xodkkO0KKKKXXXXKkxxkOOOOOKXXXXXXKKKKKXKKKKKxok00OOOOxkO0000OOOdc::::c::lxxl'...'''''''''',::;'..';ldxOKXNWWMMMWWNX0OxdkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOOOOOOkddkOOOOOOOO00KK0xdxddooddoldxddk0KOo,;:okkkkxolxOOkkkOOkO0KKKXKK00KKKKKKKKKKKOO00xolokKNWWWWNXkodkOOO0KKKXXX0xdxkOkkk0XXXXXXKKKKKKKKKKK0xok0OOOOOkkO0KKK000Od:;::clclxkko:'..'''''''''',;::;'''''',;:loodddolc:,,,,c0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOOOOOOOxddkOOOOOOO000KK0xdddooooo::doccllcc,...,:cloxkdodkOkkO0OkkO0KKK000KKKKKKKKK0OO00xllxOKNWWWWWWWXkdxkO000KKKKKOxdxkkkkOKXXXXXXXKKKKKKKKKK0dok0O0O0Okxk0KKKK0000xc::clcokkkxkOl...'''''''''',;:;,'''''''''',,,''',,;;:xNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOOOOOOOkddxkOOkkOOO000KKKkddolool:,';;'...  . .......';llcokkxO00OOO0000000KKKKKKKK0kk00xlokO0XWWWWWWWWNKkooxkO00KK0OxdxxkkkkOKXXXKKXXKKKKKKKKKK0ddO0O000Okdx0KKKKKKK00OoccloxkkkxkNNk;..'''''''''''',,,,''''''''',,,,;;;;;dXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXOOOOOOOOxodkkOOkkOOO000KKKOxdlllo:''','.....';,...''.....',:ldxO00000O000000KKKKKKKOxk0OklokOOOO00KXNNWWWNKxloxkkO0Okddddxkkkk0KXKXXXXKKKKKKKKKKKOdxO00O00Okdxk00KK00KK00OxoloxkkkxkXMWKo'..'''''''''''''''''',,,;;:;;:;;,:dXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXOkOOOOOOOdodkOOkxkOOO0KKKKK0kdlcol;',,coc',ldkxc'';cc:;,,'.,clcoO00000000000KKKKK0OxxO00xllxOkkkkkkkkkO0KXXX0xllxkkxddddodkkkkOKXXXXXXXKKKKKKKKKKKkxOO00000OxxxxxO0KKKK00000OkdoodxxkXMMMWOc'.''',''''''''''''''',,,,,,,,;ckNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOkOOOOOOOxodxxOOkxxOOOO0KKKKKOdlclc,';ccdxcck0Xk::odddlccc:,,okld000000000000KK00kdoxO0Oxodxdoc:;;,;::cclodkOOkdlcdkxxdolldkkkOKXXXXXXKKKKXKKKKKKK0xxO00000Okxxxxdxk00KKKKK0000Okdlllok0KKK0xc;'.''''''''''''''',,,,,,,,;:dKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0kOOOOOOkO0kkxxkOkxxkOOO0KKKKK0klcc;,;cl:oOdckWWk:ldxdoc:cooc:d0xx00000000000000OdodkO0kdoxdl:;,''..''......,;;:dkdclxkxoldkkkk0KXKKXXKKKXXKKKKKKKK0xxO00000OkxxxxxddxkO000KKKK00OkxollllllldkKXOo;'.''''''''',,',,,,,,;lxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOkOOOOOkONXOOkxxOkxdxOOOO0KKKKKOdc:,,:ll:oKOlxNWOldkkxdlccoxdlkKxO00O0KK000000Odl:oO0Oxox00xxxxdc,','''.........cdxl;;oxxdxkkk0KXKKKKKKKXXXXXKKKKKK0xxO000K0Okdoxxxxxdddxxxxxxxxxddoooooodk0NWMMMWX0xl:,'''..'''',,;cox0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXkkOkOOk0WMXOOOkxkkxddkOOOO0KKKK0Ol;',:ll:oKKxd0WXdoOOOOkxk0XKkOOkOOO0K0000OkxddolxOOxdk0NNXXXX0o;,:ll:;;;,'.........,loloxkkkOKXKKKKKKKXXXXXXKKKKKKOxxO00000K0dcclodxdddddooooooooooodk0KNMMMMMMMMMMMWNK0OxddddxkkOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXkkOkOO0WMMXOOOOkxkxddxkOOOOO0KKK0kc'':co:lKX0kdkXKxk0000000K00OxkOOOOOkkdddxxkxxkkxxkKNWWWWWWWkclldxxolccc:;'....   .:dookkkOKXKK0000KKXXXXXKKKKKK0OxxO0OOkk0KKxc:cccccllllllllllooodKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxkOkO0NMMMN0OOOOkkxdxdxkOkOxdO0K00k:',co:lKXXX0k0XXKKKKKK0OkkkxkkxxxxxkkOOkkxxxkO0KNWWWWWWMMMWxcddddolc:coolc;,:c,.  ..;dkkkKKKK000000XXXXXXKKKKKKOkxxOOOkxxdkKKklcclcccccccccccok0OkONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkxkkOKWMMMW0OOOOOOkxxxdxOkkkolxO000kc,:lcoKXXXXXXXXXXXXXXK0OkkkkkOO0XNNWWNXKKXXNWWWWWWWWWWWMMWkoxkkdolc:codddocokkc..';lxxk0KK00000OOKXXXXXKKKKKKOkkxdkkxxxo:cOXKkl:cclllllllloooxO00kOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0xkkOXWMMMMXOOOOOOOkxxxxxkkkxl:cdO00OkdlcoKNXXNXXNNNNNNNNNNNNNNWWWWWWWWWWWWWWWWWWWWWWWWNNWWWWW0dxOOkxdoodkOK0xlkKx::okdodxOKK0000Okk0XXXXXKKKKKKOkkkxdxkkxo;,:xKKKOl::cclllllokkddk000kkKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXOxxkKWMMMMWKO00OOOOkxxxxxxxkd:,,:lxO00OddOXNNNNNNNNNNNNNNNNWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNWWWN0kOOOOOOOkO0NWKO0kdx0X0olokKK00OOkxx0XXXXXXKKKK0Okkkkddkkxl;,,;o0KKKOl:cllcccokkkkxk0000OkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXkxx0WMMMMMN0O00OOOkxxxxdxxxko,,,,,:clodddkOKNNNNNNNNNNNNNWWWWNNWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNXK0000000Okkkkkkk0XNXxcok000OkkxxxOKXXXXXKKK0Okkkkkxodko:,,;;;l0XKKKOl:ldxoldkkkkkO00000kOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxdkXMMMMMMWK0000OOkxxxddxxddc,,,,,,,,,lOKXNNNNNNNNNNNNWWWWNNNWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNXXXKK0OkdodkOKXXNXOloO0OkkkxkxxkKXXKKKKKOkkkkkkkkdloc;,;:::;l0XKKKKxc:lkOkkkOOOOO000000kOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0xd0WMMMMMMWNK00000Okkkxxkxool;,,,,,,,lKNNNNNNNNNWWNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNNNNNXXXKKXXNNNNXOodkOkkxxxxxdk0XXXXK0Okxxkkkkkkxl:;,;:::::;o0KKKKXOl::lk00000000000000OkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKkkXMMMMMMMMWN0kxkOOOOkxxxdcll:,,,,,,:ONWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNNNNNNNNNNNNNNKOxxkkkkkkkOOO0KKK00Okxxxkkkkkkkkd:,,;:ll::::dKKKKKK0o:;:lk00000000000000O0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN00NMMMMMMMMMNd::oxxxxdddxo;coc;,,,,;xNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNNNNNNNNNNXOxxkxxxkkkkkOOOOkkxxxxxxkkkkkkkkkxc,;cc:cooc:ckKKKKKX0o:;;:oO00000000OOO00OONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWMMMMMMMMNx::::lxxxdddddc,:llc:,,,lKWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNNNNNX0kxxxxxddoollodkOOOOkddxxkkkkkkkxxkl;;cccc:cdocd0KKKKKKOl:;;::dO0000000OOOO00OXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNk::ccc::oxkxxddo:,;:llc:,;xNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNNNNNX0Oxxxxxdoollllox0KXX0kxdxkkkkkkxxxdxd:;ccclc::oxdOKKKKKK0dc:;:::cx000000OOOOO00OKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0c;:cccc:::oxkkxdo:,;:ccc:;:ONWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNNNNNNNNK0Okxxxdoollllllx0KXKOkxdxxkkkkkkxxddddl::cccllc:cx0KKK00KKxccldocccoO0000OOOOOO00k0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNd:;::::::c:;:lodxkxoc:::c:;,:kNWWWWWWWWWWWWWNNNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNNX0OkxxdooolllllllokKK0Oxddxxkkkkxdxxdddocolcccllllc:ckKKKOk00xlclkXkllcoO000OOOOOOO00O0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl;;;;;:ccccc:::::cclllc::cc:,;dXWWWWWWWWWWWWWNXXXNNNNNNNNNNNWWWWWWWWWWWWWWWWNXNNXKK0OkxddoollllllllllokOOkxdddxxkkkxxxdodddol:cocccclllollxKKKOxkOdlclxXWKdlloO00OOOOOOOO00OKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxc;::ccccccccc::;,,,;:::cllllc;,l0NWWWWWWWWWWWWWWNNNNNNXXXXXXNWWWWWWWWWWWWWWWXOkdoolllllllllllccllclodkkddddddxxkxxxxxxoodol:;colcccclloookKKOkxxxolllkXMMNxoldOK0OOOOOOO00OOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOdc::cllllccccc::;,,,,,,;::cllllc:;,:xXWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNK0OOkkkkkkOOko:cllooddddddddxxxxxdxxxxdool:;;colcclllclox0K0kxddolloxKWMMMWOood0K00OkOOO000O0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXkl:;;:lodoolccc:;,',,,,,,,,,;:cllllc:;,,lONWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNOlloooddddddddxxxxdddxxxkxlc:;,;lol:ccclllok00kdoolccokKWMMMMMMKdokKK00OOO0000O0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o:::;cooddlcccccldxdc;;;,,,,',::cllllc:,,,;o0NWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWN0xoloddddddddddddddoooxxxxdl:;;;:llcccccccox00kolcccllo0WWMMMMMMMXxdOKK000OO000O0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXklccl::loooolccco0WMMWXkl;;;,'.,;:ccllollc;,;;:dKWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNXXNWNNNXK0Oxdooodddddddddddddoolldxxxxdc::::cccccccccldxxdolllldddddONWWMMMMMMNOkKK0000OO00OKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOoldxOKklcoooolccoKMMMMMMMNkc;;,.,;::cllolcclc:;;;ckXWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWKxdxxxdoollooddddddddddddddddolloxxxdl:clc::cccccccllllccloooookkxxxk0NMMMMMMMWKKKK00KOOO00XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxox0NWMMXdccclolcl0WMMMMMMMMWXxc,',:::cllll:;;::::;,;l0WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWXOolccloodddddddddddddoooollc:oxkdlc:llc::dkoccccllllllllodoodOkxxxkOXMMMMMMWNXKKKKK0OO0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkkXWMMMMMWKoccloockNMMMMMMMMMMMWO:,;:::cllloc,,,;;,,,,,:xXWWWWWWWWWWWWWWWWWWWWWWWWWWNNXK0Okxdl::llcloooooolllccccccc::cdkdlccllllccdXWXkolcllllloolooooxOOOkxOOKWMMMMWXKKKKKK000XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOONMMMMMMMMWKdlcooo0MMMMMMMMMMMMWk:;;::::clll:,;:;;;;;;;''lOXWWWWWWWWWWWWWWNNXKK00OOkkxddddxxxddddodkkkkxdoollllcccccloddlccooolllldKX0kdc:cooooooooododkK0OkkOOKWMWWNXKKKKKK00KNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXkKMMMMMMMMMMMN0dloxKWMMMMMWMMWWW0c;::::::cllc;,:::;;;;;;'',;ldxkkkkkxxxddxxddooooddddxxxxxxxdlllodkkxdooollllllc:;::cllccloddolllllol;,'...,;codooooodok0K0kkOOOKWNXXXKKKKK00KXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0x0WMMMMMMMMMMMWKxooONNNKKNWWWMWOc::::::::cll:;;::;;;;;;;'',:::::;,,;;,,,:oddxxxxxxxxxxxxxxxxdocokkdolllllccccccccccllloooodxdllll:,......'';;:lododdoox0KKOkO000XXXXNNKKK000XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKxkNMMMMMMMMMMMMMWX0kOOOOXWMMMNkc:::::::::lc:;;:c:;;;;;;,',;:cccc:;;;;;,;;oxxxxxxxxxxxxxxxxxdodxxollllccccllccccllllooooooxxdolll:,......,:clllll;,;cdk0KKOO0OdlkNWWWNK000KXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0x0WMMMMMMMMMMMMMMMMWWWMMMMWKo::::::::::cc::::c::;;;;;;,,,'',;cllc:;;;;;,cxdlldxxxxxxxxxxxooxxollllcclllllllllllloooooodxkxdool;'....';:c:;;;:cc'.'o0KKK000k:..dWMWX00KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXKNMMMMMMMMMMMMMMMMMMMMMMXkl::::::::::::;:::c:::;;;;;:;:,......,;;:::;;,;c:cclldxxxxxxoccdxdlllllllllllllllllllooooodxxkxxdo:'....';clll;....''.;d0KKK00ko,. .xWMWNNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOoc:cccc::::::;;:ccc:c:;;;;:c;:;.............';,':oooolcldxdl:;:dxolllllolllllllllllloooooodxxkxxdc,.....,clllll;.....'lk00000Oo:..  .kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNNXXXXXXXXXXNNNNWWN0xocccccccc:::::;::cc::::;;;:cc:::..';:cc:'......';col;;coolcc::lxxdlllloollllllllllllloooooddxxxxdl;.......';;:ccc:...'ck0000Oxl;...   ,0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXK000OOOkkkkxxxxxxxxxxxxxxxkxlc:::::ccc:::::::ccc:::;;;:clc:::..;c:,';lc'.....;:c:'...:oollcoxdolclllllllllllllllooooooddxxxxxdl,..............'..'cxO0Okdl;'.....  .:KMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNK0Okxxxkkkkkxxxxxdddddddddddddddddooollccc::::::ccc::::;:cllc::c;..;c;...'cllc:::loc::'...;looldxolcllllllllclllllloooooddxxxxxxdoollc;'..............:oolc:,........   .lNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNKOkxxxxxxxxxxxxdddddddddddooooooooooooddddddoolcccccccccllllc::;;:,..,c:'...,;,;::::cl;;ll;;clolcdkocccllllllclllllooooodddxxxddddlc;;;;:cl:'..........................   ..dWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0kxxxxxxxxxxxxdddxxxxddoooooooooooooooooddddddddddddddoollll:,';;,,..,:clc,...........,cc;:clllcoodOklcllclllllllloooddddxkxoc:,,cl:,......':lc:;,'......................  ..'kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOkkkxxxxxxxxxxxxxxdddoolllooooodddddddddddddddddoooolllcccc;'.':;'..':l:,;c;'......',;;clccccc:cx0kk0xlllllllllloodddxxkkkO0k;...;ll;........'cllcccc:;,'.................  ..,OMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOOkxxxxxxxxddxxxxdooollllllllllllllloooooooooollllccc::::;,...:c;...'cl:''';;'..',;;:cccccc:ccloox0xkKkollllllloddookOO000OOO0x;,clllc:;,,'...':l:'',;:clc:,...................;0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0OOkkxdxxxxxxdddxxdolllllllllllllllllccccccccc:::::loxkdl,.....;c:'.  .:oc,'',:,..':lllllclllllcldkxdxxkOkxlllllloodlok0000K00OOOOkol:;;:cc:;'...':c'......';:cc,.................lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0kOOOkxdxxxxxddodxdollllllllllllllloddxkkOO0Okdocc::coxO0d:,.,;::;'..  .,oddlccloc;;:lccllcclolcclloxxdccdkxkxolllldkkxx0KKKK000OO000Okdc;'.......':lolodddooc::cc;...........'.....dWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKkOOOOkxxxxxxdooddolcllllllllodxkO0KXNWWMMMMMMMWNK0Okxddddc;;;;,'....',;:coooolccclllolcc:;;;;,',:ccloddl;cxxkkkdolodxdox00000OO0KKK0OOkkkxo:,,:ldxOKXNNNNNNNNNXKOo,............'....dWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOk0OOOkxxxxxxdodolccllllldkOKXNWMMMMMMMMMMMMMMMMMMMMMWWWWXl.....'',,:loolldxxdddkOOkkxxdxddolc;'.;:clloooccoxkkkkkxdxkkkkOOOO0KXNXK0OkkOkkkOO0KXNNNNNNNNNNNNNNNNNNXOc'...........'...cXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0xkOOOOkkxxxxxdolcclllok0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNl'''',;:cldxkOkdxxxxkO00000OOkdoodddxdl::cccccclcoxkkOOOOOkkxkOOO0KXNXK0000000O0KNNNNNNNNNNNNNNNNNNNNNNNNNXd,...........'...kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOxxOOOOOkxdddkkolllloONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWN0xl;,;::cdkkxkkxxxxxkOOOO0000000kxoddddxO0xc;:clodkdokkkO00OOOOOkkk0NWNK000000O0KXNNNNNNNNNNNNNNNNNNWWWWNNNNNNXx,..........'...lNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0OkkOOOOxddk0OdllldKMMMMMMMMMMMMMMMMMMMMMMMMMMMN0xoc:;:ccoxodOOOkxxxkkOOOOOO000000Okxxkkxdodkkko,'cdxxkdok00OOOOOOOOOOOO00OOO000O0KXNNNNNNNNNNNNNNNNNNNNWWWWNNNNNNNNx;'.........'..;KMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0kkkOkkkO00OoloxXMMMMMMMMMMMMMMMMMMMMMMMWX0dc;,;:cldoldxodkxdxkOOOOOOOOOO0000Okkkkkkkkkkxdodl,',:oddoox00OkkOOOOOOOk00OxxkOO0KNNNNNNNNNNNNNNNNNNNNNNNWWWNNNNNNNNNXxc;...........,OMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWN0kkkkO000OdlodONWMMMMMMMMMMMMMMMMMMNKkoc::clodxkxxxdxxxddxkkkkkkOOOOOOO00Okkkkkkkkkkkkkdlll,''';oddodkOOOOOOOkkOOO0K0kxOO0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNKdc:'.........'OMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXkkOO000koodddx0NMMMMMMMMMMMMMMWKxc;;;:ldxkkkkkkkkkkkkkxxxkkkkkxkkOOOOkkkkOkkOOOkkkOkxdol;'''..:oddoxkkOOOOOkkO0KKK0kkOKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNOlc;.........'kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0kkOOOkdoodxddd0WMMMMMMMMMMMMWOc,,,,,,:llodxxkkkkkkkkkkkkkkxxkkkkkkkkkOOOOkkkOOOOkxxxdoc,.'''',cdddddxkkkkOO0KKK0OOO0NNWNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXd;..........'kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0kOOOxdooddxxdokXMMMMMMMMMMMMNo,;;,,,;;::::cclloxxkkkkkkkkkkkkxxxxxkOOOOOOOkkkxxxddxxxo;'.',;;,,cdxdddxkOO0KK000OOk0XWWNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNO,...........kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWWWWWWWNXKOkkkxdooddddxxdoxXMMMMMMMMMMMMXl,;;,'...''',,,;;;::cldxkkkkkkkkkkxddkkkkkkkOOOkxxdddxxd:'.,;;::;,,cdxxkO00000000OOOkKNWWNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNWNNNNXc...........xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOOOOOOkkkkxxddddddoooddddxxxdokNMMMMMMMMMMMMKo;,,;;,'''..'''..',,;;;:ldxkkkkkkkkkxxkkkkkkkxxxxddddxd:'';;:::::;;lkO0000000000OOOOOXWWWNXXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNWNNNNNd...........kMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXKK0OOkxxddddddddddoodddxxdokXMMMMMMMMMMMMXxddl:,,,;c;',:::::,...',;;;codkkkkkkkkkkkxkkkxxxxxxxxxo:',;::::::cdk000000000000OOOOO0NWWWNXXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNWWWWWNNNO,.........;0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNXXXXKKKKKKOdooddxdodONMMMMMMMMMMMMWOoxkkxl::c:,,,;;cdOxlc;'..';;;:cdkkkkkO0K0Oxddddddddddl;,;;:::::coO00000000000000000O0NWWWWNXXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNWWWWWWWWWK:........'xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNNNNXXKK0kooddddxk0XWMMMMMMMMMMMMMKddxdoc,,cl;,,;;lk0OOOOko;...';;;;cdkO00KK0Okkxxxxxddoc,,;::::::lxO0000000000000000OOKNWWWWNNXXNNNNNWNNNNNNNNNNNNNNNNNNNNNNWWWWWWWWWWWXd;;;;;,;l0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNNXXXKK000KKKXXKKKXNWWMMMMMMMMMMWN0dcllc;,'';:;;,:dOOkkkkkOOd;'...,;:;:lk00Okkxxkkxddddo:,,;;;;:coxO0000000000000000OOOKNWWWWWNXXNNNNNNWWWWWNNNNNNNNNNNNNNNWWWWWWWWWWWWWWNklldxxxxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXOkkkkO0KXWMWNKOxdlc::::;;;;,,::;cxOkxxxxxxxkxl'.'..';:;:cdOOxxxxxxxdddl;,;;;;;cokOOOO0000000KK000OOxll0WWWWWWWNKKXNNNNWWWWWWWWWWWWNNNWWWWWWWWWWWWWWWWWWWWW0lcodxxxxxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0Okxxdddooddoc:;:;;;;;;;;;;;;:cdkOkxxxdoollc:::c:,'.';;;;:okkxxxxxxdl:,,;;;;:oxOOOOO0000000000OOOxl,';xNWWWWWWNK0XNNNNWWWWWWWNWWWWWWWWWWWWWWWWWWWWWWWWWWWWKo:loodxxxxxkOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN00koc:::;;::;;;;;;;;;;;;;;;::coooooollc::::::::;;,,,,;;;;;lkkxxxxdl,,;;;;:loodkOkOO00000OOOOOOxl,..',cKWWWWWWWX0KXNNWWWWNNNNNNWWWWWWWWWWWWWWWWWWWWWWWWWWWXo;:loodxxxxxxxkOKXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dl:;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,''''''''',::,,;;;;;;lxxxxo:,,;;;;:ooclxkkOOOOOOOOOOOkxc,''',,,,dNWWWWWWNK00XNNWWNNNNNWWWWWWWWWWWWWWWWWWWWWWWWWWWWNKo;:clooddxxxxxxxxxkOO0KKXXXXXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNOdc;;;;;;;;;;;;;;;;;:;;;;;;;;;;,,,''''''''''''''''',:c,,;;;;;;:ooc;,;;;;;cddcokkkkOOOOOkkkkxoc,,,,,,,,,,c0WWWWWWWX0O0XNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNKo;;:coooodxxxxxxxxxxxxxxxO0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWWWWWWWWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0xl:;;;;;;;;;;;;;;::::::;;;;;;;;:;;,,,''''',,,,,,,,,,,,,;:,',,,,,,,,,;;;;;;:ddldkkkkkkkkkkkxdl:,,,,,,,,,,,,;xNWWWWWNNK00KXNNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNN0l;;;:cloooddxxkkkkkOOO0KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKkl:;;;;;;;;;;;;;;;;;:::::;;;;;;::;;:;;;;,,,,;;;;;;;;;;;;;,,,,,,,,,,,,;;;;;;;;lxlokkkkkkkkkkxl:;,,,,,,,,,,,,',,cOWWWNNNXK0O0KXNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNXOc;;;;:coddoodxkkkd0WWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWWWNNNNNNNNNNNNNNNNNNNNNWWWWWWWWMMMMMMMMMMMMMMMMMMMMMMMMMWKkl;,,;;;;;;;;;;;;;;;;;;;;;;;;;;;::;::::;;;;;;;;;;::;;;;;;;;;;;;;;;;;;;;;;;;;;;;:dddkkkkkkkkkkko;,;,,,,,,'''','''',l0NNNXXK000OkKNWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWNXXx:;:::ccldxxkO000ddKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWNNNNNNNNXXXXXXXXXXXXXXXXXXXNNNNWWWWWWMMMMMMMMMMMMMMMMMMMMMXx:,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::;;;lkOOOkkkkkkxddkxl;,,,,,,,'''''''''',o0XKKK0OO0OxONWWWWWNKOO0XWWWWWWWWWWWWWWWWWWWWNXKd::ccodxkOKKKK0OoldkKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWNNNNNNXXXXXXXXXXXXXXXXXXXXXXKKKKXXXXXNNNWWWWWMMMMMMMMMMMMMMMMMXo,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::::;;o00Okkkkkkdc;:cll:;,,,,''''''''''''',o0K0000000xxXWWWWWKkO0kkOXNWWWWWWWWWWWWWWWWNXKOlcodkOKKKKKK00klcoxxxOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNNXXXXXK00OOOO0KXXXKKKXXXXXXXKXXXKKKKKKXXNNNWWWWMMMMMMMMMMMMMMXo,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::;;;;o0KOkkkxdl:;;;,,,,,,,,''''''''''''..',o00KXXKKKkxKWWWWNOkKKK0kk0XWWWWWWWWWWWWWWWXOdodkOOO0KKKKK0Oxlcododxxk0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNXXXXXXXXK0OkkxxxkOKXXXK000KNNNNNNNXXKKKKKKKXXNNWWWWMMMMMMMMMMMWk;,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;l0KOkxoc:;;;;,,,,,,,,'''''............,dKXNNXKKOx0NWWWXOOKKKKKKOk0NWWWWWWWWWWWXOdoxOOkkxkOKKKK0kdolodoooddxxkKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNXXXXXXXNXXXXXXK0Oxxxk0XNNNX00XNWWWWNNNNXXKKKKKKKXXNWWWWMMMMMMMMMMXo,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ckK0Od:;;;;;;,,,,,,,'''''..............:ONNNXXX0kOXWWWKO0KKKKKKK0kx0NWWWWWWWXkddkOkkxxxxxO0KK0kolcododdooodxxxOKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXXKKXXNXXXXXXNNNXXKOxdk0XNNNNKKNWWWWWWNNNNNXXKKKKKKKXNNWWWWMMMMMMMMO:,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;lkKKOoc;;;,,,,,,,,,''''...............'c0NNNXX0OOXNWW0kKKKKKKKK0OkdkXWWWWXkddkOkxxxxxxxxkOK0xlc:cdoooddoooddxxxOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKKXXXXXXKKXNNNNNNXKOxOKNNNNNKXNWWWWWWNNNNXXXXKK0KKKKXXNNWWMMMMMMMWx;,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;cdO00kdl:;,,,,,,'''''.................'l0NNXKKOOKNWNkk00KKKKKKOxxkdd0NNOddkkkxxxxxxxxxxxkkdc::cdxdoooddoooodxxxxOKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNXKKXXXXXXXXXKKKNNNNNNXKKXNNNNNXXNWWWWWWN0kkOO0000000KKKKXNNWWWWMMMMMNd,,',,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;:coxkOkxl:;,,,'''''...................'l0NXKKOk0XWXxxOkO0KKK0kxxdxdoxxdxkxxxxxxxxxxxxxxdc:::;lxxxdoooddooooddxxxxOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKXXXXXNNXXXNNXXNNXXXXXNNNNNNXXNWWWWWNKxl::clloodxk0KKKKKXNNWWWMMMMMNd,,',,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;::cc::;,,,'''''.....................'l0XXKOkOKWKdxkxkkO00Okxxxxdoldkxxxxxxxxxxxxxxxdc:::coxkkxdoooddooodoodxxxxxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKKXXXXNXKKXXXNNNNNXK000KKXXXNNWWWWNWWXkollcccllloodk0KKKKXXNNWWWMMMMNd,''',,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,'''''.......................':ox0Oxk0N0dxkxxxxkkkxxxxxxdxxxxxxxxxxxxxxxxxoc;;cdkkxdxdoooooddoodddoddxxxxxxOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKKXXXXXNXKXKXNNNNNNNNNXKKK00KKXNWWNWWNX0kdllllloddddxk0KKKKXXNNWWMMMMWk;''',,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,'''''.........................,clxkxkOXOdxxxxxxxxdxxxxxxxxxxxxxxxxxxxkxxxo:;cdkkxdddddooooooddoodddodddddxddxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNXXKKXXXXXXNNXNNNNNNNNNXNWWWWNK000KXNNNWNNXK0Oxooodxkxxxdxk0KKKKXNNWWWMMMMKc''''',,,,,,,;;;;;;;;;;;;;;;;;;;;;;,,,,,,;;;;;;,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,'''''........................':odllxdxO0kdxxxxxxxxkOkdxxkxkkkkkkkkkkkkkkxoc:dkkxdddoddoooooooooooodddoodddddddddxk0XNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKKXXXXXXXNNNNNNWNNNNXNWWNWNX0000KNNNNNNNX00OkkkkkkxxxxxxOKKKKXXNWWWMMMMWk;'''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,'''''........................,odxxdldddOOdoxxxxxxxxKNX0kxxkkkkkkkkkkkkkkxoccx0kxdddddddooooooolooooddddoodddddddddddxk0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXKKXXXXXXXXXNNNNWWNNNXKXNNWNNXK000KXNNNNNNNXK0OOkkkkxxxxxxOKKKKXXNWWWMMMMMNd,'''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,'''''........................'cxxxddodolkkolxxxxxkxxKNXNNKkxkkkkkkkkOOkkxoclkOkkxddddxkdooooooollooooddddoodddddddddddddxkO0XNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXXKKXXXXXNNXXNNNNNNNXXXKXNNNWWNXKKKKKXNNNNNNNXXK0OkkkxxxxxxOKKKKXXNNWWMMMMMMXo,'''''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'',,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;,,,,,,,,,,,,'''''.........................':ldxxxxdllloocoxxxkkkdxKNXXNNXOxxOOOOOOOOkxocoOOkOkdddddxkkoooooooollooodddddooddddddddddddddddxkOKXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNXKKXXXXXNXXNNNNNNNNNWWWWWWWWWWNNXXKKKXNNNNNNNXXKOkkkxxxxxxOKKKKXXNNWWMMMMMMMXd;''''''''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,''',,,,,,,,,,,,,;;;;;;;;;;;;;,,,,,,,,,,,,,,,,'''''.........................,cccodxkkxolcccldxkkkkkdxKNXXXXNNKxxOOOOOOkxdcoO0O0OxddddxddkxooooooolllooodddddoodddddddddddddddddddxkOKXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNXKKXXXXXXXXNNNNNNNNNWWWWWWWNNNNNNNXXXXXNNNNNNNNX0kkkxxxxxx0KKKKXXNNWWMMMMMMMMNx:,'''''''''''''',,,,,,,,,,,''''',,,,,,,,,,''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,''''''.......................,;colccoodkxxdoolccoxkkkkkddKNXXXXXNNXkxO000OxdloO0000kdddodxooxkdooooooolclloddddddooddddddddddddddddddddddxkO0XNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNXXXKKXXXXXXXNNNNNNNNNNNNNNNNNNNNXXXXXXXXNNNNNNNNXOkkxxxxxk0KKKXXXNWWWWMMMMMMMMWOc,''...'''''''''''''''''''..''',,,,,,,,,,''..''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'''''''.......................;ooollllooodoooloolclxkkkkkodKNXXXXXXNNNOxO00kxolkK000OxddookkdddkkdooooooolclodddddddodxxxddddddddddddddddddddddxkO0XNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWMMMWWWNNXXXKKKXXXXXXNNNNNNNNNNNNNNNNNNXXXKKKKXXNXNNNNNXNX0kxxxxxxkKXKKXXNNWWWWMMMMMMMMMWXx:,'......................''',,,,,,'''',''...''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,'''''''........................':odl::::c:cdllxkdoooldkkOOkoxXXXXXXXXXNNN0xOOxdlxKK000kdddldOkkkkkOkdooooooollloddddddddkKX0OkxddddddddddddddddddddddxxO0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXXKKKXXXXXXXXNNNNNNNNNNNNNNNXXKOxx0KXXXXKXNXXXXX0kxxdddxOKKKXXXNNWWWMMMMMMMMMMMMWKo'.....................'',,,,,,,'''''''''.....''''''',,,,,,,,,,,,,,,,,,''''''''''''.....................''',:ccodc:::;,:oocoO0kdoldkOOOxokNXXXXXXXXXXNN0xxxdoOK000OxddolkOkkkkOOOkxdddooollllodddddddd0WMWNXK0OxxddddddddddddddddddddddxO0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNNXXK00KXXXXXXXNNNNNNNNNNNXXXKKOocclk0KXXK0KKXKKK0xdooodOKXKKXXNNWWWMMMMMMMMMMMMMMM0;....................'''',,,,,'''''''''''........''''''''''''''''''''''''''''''.....................''''',:ccccooc::::clolcx000kolxOOOxoONXXXXXXXXXXXNNkoxdd0K000kxddldOOkkkkkOO0Okkkxxddolloodddddddx0WMMMMMWNX0Okxddddddddddddddddddddddxk00KKKXNNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNXXXK0O0KKKXXXXXXXXNNXXXXXXXXKkl:::coxOO000KKKKK0kdoodOKXKXXKOO0NWWMMMMMMMMMMMMMMM0:..................'''''',,,,'''''''''''''...........'''''''''''''''''''''............................'';:ccccc:loc::::coo:oO000Odox0Odo0NXXXXXXXXXXXXNKdddokK000kddlokOkkO00OOOO0OOOOkkkkxdddddddddddxKWMMMMMMMMWWNK0OkxddddddddddddddddddddddddxxkkkO0KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNNXXKOxxOKKKXXXKXXXXXXXXXXKK0dl:;;;;cododxkO0000OxdxOKXKKXKxl::lkNMMMMMMMMMMMMMMWOl,.....'''''.......'''''',,,''''''''''''.............................................................',;::ccc::::ooc:::clocoO00000kdkOoo0NXXXXXXXXXXKXNKdddlo000OxdoldxkkkO00OkkO00OOkkkkkkOkkkxxddooddx0WMMMMMMMMMMMMWNXK0OkxdddddddddddddddddddddddddddxkO0KXXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWNNXXXK0xx0XKK000KKKKKKXXXXKKKkoc;;;:looodddxxxkkxxk0KXXXXXOdc;;;;lONMMMMMMMMMMMMNkoc.....'''',;'.......''''''''''''''''''...........................................................',;;::::cc::::ccol::c:clllk000000Okkoo0NNXXXXXXXKKKXX0ddxllk00OxdloxddxkkkkkkkkO00OkkkkkkkkkOOOkkxdddodOXWMMMMMMMMMMMMMMMWNXK0OkxddddddddddddddddddddddddddddxxkOO0KXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOxddddddooooooooodxk0KXXNNNXXXX00KKKKKKK0KKKKKXXKKKXKkdc:;coddxxdddddddx0KXXXXXXNOoc;;;;;;lkXWMMMMMMMMMXxol;..'''''',;;,........'''''''.''''''........................................................',;cllc:::::c::::ccccclc:ccccllx0000000OdloONNXXXXXXKKKKXXkoxdocoO0kxooxxdddddkkkkkkOOO0OkkkkkkkkkkkOOOOOkkxxkO0KXNWWMMMMMMMMMMMMMMWNXK0OkxdddddddddddddddddddddddddddddddxkO0KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXd,..                 ...',;codkOKKKKKKKK00KKKKKKKKKKKKKXKKOkxxkkO0OxdddoccoOKXXXXNNNXkc;;;;;;;;coOXWMMMMMMKdll:'..''''',,;;;'.........''......................'',,,'''''''''''''......''',''''......'';cloxxdoc::::::;;;:cccclllc;:ccclld00000000kolo0NXXXXXKKKKKK0doxxolcx0kdodxxkkxdooxkkkkkkOO0OOOkkkkkkkkkkOOOOOOOOOkkkOO00KKXNNWWMMMMMMMMMMMWNXK0OkxddddddddddddooooollllllllcccccllodxkkkO00KXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:.                       ......',;cclodxOO000KKKKKKKKKKKKKK0000000Okdo:,...,lkKNNNNWWXx:,;;;;;;;,,:ldk0NWMKdllc;.....'',,,;;;,..........................'',,;;;;;;,,''''''''.',::::;;,,,,,,,''.',;:lodkkOkdl:::::::;,,;:cccccodxdl;:cccldO0000000Odlod0XXXXKKKKKK0dcoxkkxkOOkooddodxkkxdoodxkkkkkO0OOkkkkkkkkkkkkkkOOOOOOOOOOOOkkOOO000KKKKXXNNWWWWMMMWWNK0OxdoolllooooooodddddddddddddddddooollllllodxkO0XNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxlc;'...                 .............';:cldkOO0000000000000OOOkdl:,'.......'ckXWWWWMWKd:,,,;;;;,,,,,;ckXNdllcc'.''..'';;,;;;,.....................',;;;;;;,,,,''''''''''',;cdkOOxdolc:;,,,;:coxkkOOOOxoc:::::::;,,;:ccccccclodkxolccclodkO000000xloooOXXXKKKKKKKkk0KKK000OxoodoooodxxkkkxxxxxkkkO0OOkkkkkkkkkkkkkkkkkOOOOOkOOkkkkOOOOOkkxkkkkkOOO000000OOkddoddxxxkkkkkkkkkkkkkkkkkkkkkkkkkkxxxddoolllllloxkOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNX0Oxdoc:;,'...         ................',;;:cloodddddoolc:;,'..............;o0NMMMMWXkl;,,;;;;;;;,,,lOxlc:l:',;,;;,',;;;;;'..................',;;;,,,,'''''',,,,'''',:clllok0kol:;;:lodxkOOkkOOOxoc:::::::;,',;::cccccc:''cdxkkxoccllldkO00O0xloocckXXXXXNNNNXXKK00000kxlddooooooodxxkkkxxxkkkOO0OOkkkkkkkkkkkkkkkkkkkkkkkkkkOOkkkkkkkkkkkkkxxddooodxxkkkkkkkkkkkkkkkkkkkkkkkkkOOOOOOOOOOkkkkkkxxddoolllccloxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNXKOkxolc:;'..   .......................''.............;,............:xXWMMMMWKx:,,,;;;;;;,';:cc:cl:'.';;;::;;;;cc,,,............',;;,,,,''',,,,,;;;;,'',:ccccclllooc:cloxkOOOOOOkOOkxdc:::::::;,',;::::cccc:,...'coxkkkxocclcldOOkOdcclokKNNNNNXXXXKK0000000kdoddlooooooooddxkkkkkkkkkOOOkxxdxxxxxxxkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkOOOOOOOOOOkkxxxxxxxddoollcclodkO00KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWXKK000OOkkkkkkkkOO0KKXXXXXKOkdl:,... .................................''.......      .'oKWMMMMWKo,.,;;;;;;,,',;:cll;.';;',;;::cc;,'','...',,;;::,,,'',,,;;;;;;;;;;,'';ldxolclcc:;:ldk0KKKXXXK0OOOkdolc::::::;,'';::::::cc:;'...,:ldxkkkkxdlclc:oxkOkdkKXNNXXNNXXKKKKK0000000koodollllooooooooddxkkkkkkkkOOkkxxdddddddddddddddddddddxxxxxxxxxxxkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxkkkOOOOOOOOkkxddxxdddddddddoolllccllodxkOKXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNKOxoc;'....................''',,;;:ccc:;,'..     .................................           .'dNMMMMMNx,.',,,,;;,,,,,,,c:'.'::;,'',:::c;...,,;;;,,;:lc'';;;;;;;;;;;;;,'';lxxxxdl:::ldOKNNNNNNNNNNNKOkdlcc::::::;,'';:::::::;;,,,,.'oKNNXNKkkkxkdccccokOO00KK00OOxOXXKKXXXXXXK000OkooollllllllooooooooddxxkkkkkOOOOOkkkkkkkxxxxxxxxxxkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxkkkOOkOOkxdddddddddddddddddddoooollccclodxOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXd;...                                     ...        .........................                   .lKWMMMMXl......'''''',,'''...'::;,,,''',;;,,;;,.'',,';c,.;;;;;;;;;;;,'':oxxxxxdccld0XNNNNNNNNNNNNNKkdooddoc::;,'',;::::::;;,,,;::,:OWMMMMMW0xxkxocloxOOkxdxkO0KXXOd0NXNNNNXXXK000OxlloclllllllllooooooooooddxxkkkOOOOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxkkkkkkkxxdddddddddddddddddddddddddoolllcccloxOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWd.                                                ..          ..   .....   .                 .......ckOkxdc,..''',,,,,,,,,,;;,''.';:;;;;;,,'.',;clc;,,,;:;'';;;;;;;;;,'.;oxxxxxolox0XNNNNNNNNNNNNNNKkdoooxO0kl;,'',;::::::;,',;:ccc;lKMMMMMMMMNkdkkddkkxoll:;xXNXKKKOokNNNNNXXXK0000OdlllcccllllllllooodddoooooooddxxkkOOOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxkkkkkkxxddddddddddddddddddddddddddddddoollcccldk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOc'.....                                                       .........................''',,,;;;;;,,,,,,,,;;;;;;;;;;;;;;;;;;;,'''';c:;;;;:::;,'.';:c:;,'',;;;;;;;,''.;lxxxxdodx0XNNNNNNNNNNNNNNN0kdollcllddl;'',;:::::;,'',:ccccc;:0MMMMMMMMMXxdkkxdolcclddllkXKxoodldXNNNXXXXK00OOkololcccccclllllllooddxxxddddoooodddxxkOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxxxxxxxxxddddddddddddddddddddddddddddddddddoollcclox0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXK00Ol.                                            ...''',,,'''''',,;;;;;;;;;;;,,',,;;;;,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;,''..;c:::::::cc:;,'.',;:::;;;;;;,'..,lxxxxoox0XNNNNNNNNNNNNNNNX0xolllccclccll;;c:::::;''';:ccccc:;;kWMMMMMMMMWOddolccldxkkOOOxox0X0xo:cONNXXXXXK0OOOxlloclllcccccllllllllooddxxxxxxxxxxxddxkkOOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxdddddddddddddddddddddddddddddddddddddoollcccox0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0c....                                   ......',,,,'''.................''''''''''',,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;,'..........,cccccccclllllc:,''';::c:,'.,codxxxddkKNNNNNNNNNNNNNNNNXOdoooollccccldxl:lc:::;,'';:cc:::::;,dNMMMMMMMMM0lclloxkOOOOOOOOOOddkKXKkdONNXXXXKK0OOOdclc::ccllcccccclllllllllooddxxxkkkkkkkkkkkkOOOOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxxxxxkkkkkxxxddddddddddddddddddddddddddddddddddddddddddddoolcccokXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKOkxddollc:;,''......                ..',''..........................''''''''''''',,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;'.....'''...,clllllllloooooolc:,'',;;;lxxxxxddOXNNNNNNNNNNNNNNNNXkolodddddlcloxkkoclc:::,'';:::::::::;;dNMMMMMMMMWNkldkOOOOO00000OOOOOkdx0XXXXXXXXXXK00OOkollc::;::ccllccccclllllllllllloooddxxxxxkkkkkkkkOOOOOOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxkkkkkkkkxxxdddddddddddddddddddddddddddddddddddddddddddddddoollclx0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNXXKK0Okxdl:,...        ..'..............................''''''''''''''''',,,,,,,,,,,,;;;;;;;;;;;;;;;;;,....'''''.,looooooooooooooooolc:;;;:lodddkXNNNNNNNNNNNNNNNNXklcoxxxxdoodkOOkkdcll:::,',:::::::;;;;;xNMMMMMMMMMKxkOOOOOO0KXNNNX0OOOOOOxokKXXXXXXKKK0OOOklll:ccc:::::cccllcccclllllllllllllllooooddddxxxxxkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxxxxxxxxxxxxkkkkkkkkkkkkkkkkkkkxxxxxxddddddddddddddddddddddddddddddddddddddddddddddoolldOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOxoc,'...''......................'.........'''''''''''''''........''',,,,,;;;;;;;;;;;;;:::;,....''''.,looooooooooooooooooooolc:::cldxO0KKKKKKKKKKXXX0dcoxkkkxddx0XKOkkxlcl::;,',;:::::;;;;'.cXMMMWWWWWNXxdOOOOO0KXNNNNNNXK0OOOOOxodOXKXKKKKK0OOOxool::ccccc::::::ccllllllllllllddolllllllllllooooooooddddddxxxxxkkkxxxxxxxxkkkxxxxkkxxxxxxxddddddddddddddddddddddddddddddxxxxxxxdddddddddddddddddddddddddddddddddddddddddddxxddoloOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXk:........................',,''''''.''..',,;,,,''',,,''........'',,,;;;;;;;;;;;;::::::;'....'''.'cooooooooooooooooooooooooddddoollcloxkOOOOOOOkkOOOOOOkkk00Okkkkxodollc:clcccc::::,...,xKK0OOOOOOkxkOOOO00KKKKXXXNNNK0OOOOOkdokKKKKKKK00OOdooc:::cccccccc:::::::cclllllllodolllllllllllllllllllllllllloooddkkxxxddddddddooooooooolllllllcccccccccccccccccccccccccccclllloooddddddddddddddddddddddddddddddddddddddddddddxxxxdodOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOxdoooddxkkxxdol:'.....................................''''''.....','...'''''',,,,,,,;;;;;;;;;;;:::::;,'.....',,.':oooooooooooooooooooooooooodxkkkdooodxk0XNNNNNNNNNNNNXXXXXKKKK0000OOOOkkkxxdooll:;;;,lO000000OOOOO000KKK00OOOO00KKXK0OOOOkkdldOKKKKK0OOOollcc:::ccccccccccc::;;;;::ccllllllllodolllllllllllllllloxdllllllloooodddddddddddddoooooooooolllllllllllllllllllllllooooooooooooddddddddddddddddddddddddddddddddddddddddddddddxxxxxdodONWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0l,...     ......   .....'''..................................'''''''''''',,,,,,,,,,,;;;;;;;;;;;;;;;;;;,'.....'',;;;;'.;oooooooooooooooooooooooooodxkOOOOOkxdddxk0XNNNNNNNNNNNNNNNNNNNNNNNNNNNXXXKK00OO00000K000OkddddddxkO0KXXXXK0OOkkkOO0OOkOkkkxoldO000OOOklclccc:::cccccccccccc::;,,,;;;:::ccclx0xlllllllllllllllloxdlllllllllllllllloooodddddddddddddddddddddddddddddddooooooooooooooolllooollcc::;;:::::ccclooddddddddddddddddddddddoooooddddddkO0KKKXXXNNNWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:.                     .................................'.''''''''''''''',,,,,,,,,,,,,;;;;;;;;;;;;,,'.......',,;;;;:::,',loooooooooooooooooooooooooodxOOOOO000Okkxxkk0KXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXXXXXKKOkxxddooodxk0KXXXNXXK0OOkOOOkkkkkkxoclxOOOOOklcc:ccc:ccclllccccccc:::;;;;,,,,,,;;;:c::::cccccllllllllllllllllllllllllllllllllllooooddddooooooooollcccllooooollllllccccccccc::ccccccc:;,,,,,;ldxxxkkkxxddoooodddddddddddddooooooooooooookKXXXKKKKKKKKKXXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNklcccc::;;,,''....   ........    .........................''''''''''''''''',,,,,,,,,,,;;;;;;;;;;,'.........'',,;;;;::::;'':loooollloooooooooooooooooodxkO0000000KK00kkxkkO0KXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXXKKK00OkxdddxO0KXXXNNXXK00OOkkkkkkxolcokOOOxldxc:ccccccllllllllllcccc::c:::::::::;;;;;;::::::ccccccllllllllllllllllllllllccccccllloooddddddddddolcc:::cloodddooooooooooooooooooodddddolc:,,:oOXWMMMWWNXK0OkkxddodddddddddddddxxxxxxxxddkKWWMMMWWWNNXKKK0KKXNWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNNNXXK0kdodxkOOOOOkkxxdddooolllc,..........................''''....'',,,,,,,,,,;;;;;;;;;,'...........'',,;;::;;,..,::;;,,,:looooooooooodddxxkkOOO000KKKKKKKKKK0OkkkkkO0KXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXXKKKKKXXK0OkkxkO0KXXXXXXXKOkkkkkkkxdocldkOxokXxc:c::cclllllllllllcclccccccccccccccccccccccccccccccccccccccccllllllllllllllccccccclllllloodddddddolcc::::clodooooooddddddddddddddddddddddol:;;cxKWMMMMMMMMMWNXK0OkdooddxxdxddddxxxxxxxxddxkO0KXNWWMMMMWWNXKK00KXNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNk:,'.................................'''''',,,,,;;;;;;;;;;;;;,'.........'','''',,;:;,',,,',;coooodddxxxxkkkOOOOOOOOO000KKKKKKKKKKKK0OkkkOOO00KXXNNNNNNNNNNNNNNNNNNNNNNNNNNNNXXK00KXXXXXK0OkkO0KXXXXX0kkkkkkkkxoolcokkdOWNkc:::::cccllllllllccllcccccccccccccccccccccccccccccccccccccccccccccccclllllllccccccccccccccloodddddolc:::::cloddooloooddddddddddddddddddddddolc::d0WMMMMMMMMMMMMMWNKOkdooodddddxxxxxxxxxxolloddxxkO0KXWWMMMWWNXK00KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o:::;;,'...................''.........'''''',,,,;;;;;;;;;;:::::;'....'..''',,;::cccc:;,,,,,,;ldxxxxxkkkkkkkkOOOOOOOOO00000KKKKKKKKKKKKXXXK0000000KXNXNXNNNNNNNNXXXXXXXNNNNNNNNNXXKKKKXXXXXXKOkkOKKXX0kkkkkkkkxdooccdxxkNMWOl:::::cccllllllllccllcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc::ccccccclloddddolcccccccloddooooooddddddddddddddddddddddddoccd0WMMMMMMMMMMMMMMMWNX0kdoooodddddddddxddodO0OkxddddxkOKXNWMMWWXK00KXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0l:;,,,;::;,,.........'''''''''''',,'''',,,,,,,;;;;;;;;;;;;;;:;,'.......'',,;;:::::ccccc:::::ccldxxxxxkkkkkkkkOOOOOOOOO000000KKKKKKKKKXXXXXXXXXXXK0KXNNNNNNNNNNNXXXXXXXXXNNNNNNNNNNNXXKKKXXXXXXKOxxO0K0kkkkkkkkkxdocxKOdo0MMWKo::::cccclllllllcccllc:cccccccccccccccccccccccccccccccccccccccccccccccc:::cccccccccc::::cccccccloodooccccccclloddooooooddddddddddddddddddoooddddlcd0WMMMMMMMMMMMMMMMMMMWX0kdolloddddddddddx0NWNXKOkxddodxkOKXWWWWNK00KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0l::,,,,,,,,;;;,',,;;,,,;;;;;;;;,,'.'',,,,,,,,,,,;;;;;;;;;;;,,.........'',,,;;;::::ccccccccccccccloxxxkkkkkkkkkOOOOOOOO0000000KKKKKKKKKKKXXXXXXXXKKXXXXNNNNNNNNXXXXXXXXXXXXNNNNNNNNNNNNNXXXXXXXXXXKOxxkOkkkkkkkkkxdlcOMWXKXMMMMXd::::cccccclllllcccllcllc:cccccccccccccccccccccccccccccccccccccccccccccc::::::cccccc:::::::ccc:cclooolcccccccllodddooooooodddddddddddddddooooddxdolxKWMMMMMMMMMMMMMMMMMMMMWX0xollodddddddddkXWMMMWNXKOkxdoddkO0XNWWX00KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl::,,,,;;;;;:::;;;;;;;;:::ccclllc;.....',,,,,,,,,,;;;;;;;;;;'.......'''',,,,;;;:::::cccccccccccccccodxkkkkkkkkkOOOOOO00000000000KKKKKKKKKKXXXXXKKKXXXXXXXXXXXXXXXXXXXXXXXXXXNNNNNNNNNNNNNNXXXXXXXXXX0xdxkkkkkkkkkxdclKMMMMMMMMMMNkc::::ccccccclllccclloxkxoc::ccccccccccccccccccccccccccccccccccccccccccccc::;;;;:ccccc:::::::::::cloolcccccccclooddooooooooooooooddddddddooooddxxdookXWMMMMMMMMMMMMMMMMMMMMMWN0xollooddddddx0WMMMMMMMWNK0kxdoodxOKXNXK0KNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXo;:;,,;;,.....''',,,;::::::::;,'.........',,,,,,,,,,;;;;;;;;;,'...''''''',,,,;;;:::::::ccccccccccccccldxkkkkkkOOOO0000000000000KKKKKKKKKKKKKXKKKKXXXXXNXXXXXXXXXXXXXXXXXXXXXXXNNNNNNNNXXXXXXXXXXXXXKK0Okkkkkkxxxxxxo:oNMMMMMMMMMMMWKo:::::ccccccccccccllld0XKkdl:::cc::::ccccccccccccccccccccccccccccccccccccc::;,,;;:cccc::::::::::::cllccccccccclooooooooooooooooooddddddddoooodxxxdodONMMMMMMMMMMMMMMMMMMMMMMMWN0dlcclodddddkXWMMMMMMMMMWNK0kddoddk0KK00KNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx;;;,,,;;'............',,;;;,'...'''''''''',,,,,,,,,,,,,,,,;,,'''''.'''''',,,;;;;::::::::::::::::::cc:ccoxkkOOO00000000000000000KKKKKKKKKKKKKKKKXNXXXXXXXXXXXXXXXXXXXXXXXXXXXXXNNNNNNNXXXXXXXXXXXXK0OOkkkkkkxxxxxxxxo:xWMMMMMMMMMMMMMNOl:::::cccccccc:ccccclxXWWX0xoc:::::::::cccccccccccccccccccccccccccccccccccc::;,,,;::c:::::::::::::ccccccccccccloooooooooooooooooddddddddooooddxxxddxKWMMMMMMMMMMMMMMMMMMMMMMMMWKkl::clodddx0WMMMMMMMMMMMMWXKOxdoodxkOO0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWO:;;,,,,,,;;;,..............'''''''''''''',,,,,,,,,,,,,,,;;,'''.....''''',,,,,;;;:::::::::::::::::c::cclldxOO000000000000000000000KKKKKKKKKKKKKXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXNNXXXXXXXXXXXXXXK0OOkkkkkxxxxxxxxxxxc:OMMMMMMMMMMMMMMMWXxc::::::cccccc::ccccclkXWMMWX0kdl::;::::::cccccccccllccccccccccccccccccccccc::;,,,,;:::;;::::::::::cccccccccccclooooooooooooodddddddoodddoooodxxxxdx0NMMMMMMMMMMMMMMMMMMMMMMMMMWXxc::cloddokXWMMMMMMMMMMMMMWNKOxdooddxkKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl;:,,,,,,,,,,;;;,,..........''''''',,;;::::cclcccccccc;,,;,'.........'''',,,,;;;;;::::::::::::::::ccloddxxkkkOOO00000000000000000000000000O0KXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXKK0Okkxkkxxxxxxxxxxxxx:cXMMMMMMMMMMMMMMMMMWKd::::::cccccc::cccccclkXWMMMMWNKOxoc:;;::::::ccclkOocccccccccccccccccccccccc:::;,,,;:;;;;;;:::::::cccccccccccclooooddddddddxxxxxddooodddddoddxxxxxxOXWMMMMMMMMMMMMMMMMMMMMMMMMMW0o::::codod0WMMMMMMMMMMMMMMMWNKOxdoodx0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO:;;,,,,,,,,,,;;;:::;;;;;,,,,,,;;;::::cccc:::cccccccc:;;,,'..........'''',,,,,;;;;::::::::::::::cclodxxxxxkkkkkkkOOO00000000000OOOOOOOOOOOO0KXXXXXXKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK0000000OOOkxxxxxxxxxxxxxxxxxxxockWMMMMMMMMMMMMMMMMMMMW0o:::::cccccc:::ccccc:lxKWMMMMMMMWX0kdoc:;;;:::ldoc:c::c::::::::::::::::::::::::;;,,;;;,,;;;::::::cc::ccccccccclooddxxxxxxxxxxxxxdddddddddddxxxxxxkOKWMMMMMMMMMMMMMMMMMMMMMMMMMMNkl::::loookNMMMMMMMMMMMMMMMMMWNKkdodxkO0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkc;;;,,,,,;;;;;;;,,,;;::cccccccc:::::::::;;cccccccc:;;;.............''''',,,,;;;;;;;;:::::::cclodxxxxxxxxkkkkkkkkkOOOOOOOOkkkkkkkkkOO0000KXKKKK00OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkkkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxxoo0WMMMMMMMMMMMMMMMMMMMMMMNOl:;:::::::::::::::c::cdOXWMMMMMMMMMWXKOkdolc:;;;;;::::::::::::::::::::::::::::::;,,;;;,,;;;::::::::ccccccccccloddxxxxxxxxxxxxxxxxdddddddddxxxxxxxkKWMMMMMMMMMMMMMMMMMMMMMMMMMMW0o::::clooxXMMMMMMMMMMMMMMMMMMMWX0xdxxdxOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxc;;;;,,,;;;;,,,,,,;;::::::::::::::::;;:cccccccc:,';;,,,,,,,''......''',,,,;;,,'''',;;::clodxxxxxxxxxxxkkkkkkkkkxxxxxxxxxxxxkkkkkOkO0KKKK00OOOOkkkkkkkkkkkkkkkkkkxxxxxkkkkxxkxxkkkkkxxxxxxxxxxxxxxxxxxxxxxxxxxdodkXMMMMMMMMMMMMMMMMMMMMMMMMMMN0dc;;::::::::::::::::::lx0XWMMMMMMMMMMMMWWNK0kxdolc::;;::;;,,,,;;:;::::::::::::::;,;;;,,,;;;:::::::cccccccccccldxxxxxxxxxxxxxxxxxxddxddddddddxxxdx0WMMMMMMMMMMMMMMMMMMMMMMMMMMMXd:::::cooxXMMMMMMMMMMMMMMMMMMMMWNKOkxdodOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNOl;;;,,,;,,,,,;;;;;;::::::::::::::;;:ccccllllc,',,,,;::lxdddollllcc::;;,'''''''''''',codxxxxxxxxxxxxxxxxxddddddddxxxxxxxxkkkkkkkO0KKK00OOkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxkkkxxxxxxxxxxxxxxxxxxxxxxxxxxxxxdolodk0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXko:;:::::::;::::::::::;:ldk0XNWWWWWMMMMMMWWWWNXK0kdlc:;'..''',,,,,,,,,,;;;::::::;;;;,,,,,;;:::::ccccccccccccldxxxxxxxxxxxxxxxxxxddxdddddooodddddOWMMMMMMMMMMMMMMMMMMMMMMMMMMMXd:::::cloxKWMMMMMMMMMMMMMMMMMMMMMNOxxdoodOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx;;;,,,,,,,;;;;;;;;;;:::::::::::;;:lccclllll;.,c:'',',ldddddddxxxxxxxdc'',,,''''',;:cloddxxxxxxxddddddddxxxkxxxddddxxxxxkkkkkkOKKK0000OOkkkkkkkkkkkkkkkkkkkkkkkkkkxkkxxkkkkkxxxxxxxxxxxxxxxxxxxxxxdolodxOKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdc;::::::;::::::::::;,'.';clloooooddddoooolc::;,,,,,,,,,,;;;;;;;;,,,,,,,,,;;::::;;,,,,,;;::lxkdc:ccccccccccodxxxxxxxxdddddddxxddxxddooooooddooOWMMMMMMMMMMMMMMMMMMMMMMMMMMMKo::::::cld0WMMMMMMMMMMMMMMMMMMMMW0kxxdooodOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl;;,,,,,,;;::;,,;;;;;;;:::::::;,;clclllllll:'',c:'',':dddddddddddddxdc',;;;;;;,;;::::::clooddxxxxxxxxxxxkkkkxddddddxxxxxxxxkO0KK00000OOOOkkkkkkkkkkkkkkkkOOOkkkkxxxxxxxxxxxxxxxxxxdddxxxxxxxddooloxk0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXxlc:::::;;::::::::::;,'..',,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;,,,,,;;;::;,,,,,;;:clolc:ccccccc:cccoddxxxxxdooooooooddoddxddoooooooollkNMMMMMMMMMMMMMMMMMMMMMMMMMMW0l::::::cld0WMMMMMMMMMMMMMMMMMMMWKkk0Kkdooox0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;;;,,,;;;;:::;;,,;;;;;;;:::::;;;clllllllllc,.,,,'',',lddddddddddddxo;',,;;;;;:::::::::::::ccloddxxxxxxxxxxxxdoooddddddxxxxk0K00OOO00OOOOOOOOkkkkkkkkOOOOOkkkkxxxxxxxxxxxxxxxxxdddodddddddooloooo::kNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0dc:::::;;:::::::::::;,'.',,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,;;;;,,;,;:::;:::::c::cc::::coddxxxdooooooooooooooddddoooooooolcokKNMMMMMMMMMMMMMMMMMMMMMMMMWkc::::::cloOWMMMMMMMMMMMMMMMMMMWKkk0WN0xooodkKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNo,;;,,;;;;;;::;;,,;;;;;;;:::::;;:clllllllll:'',,..',,:oddddddddddxxo,',,,,;;;;;::::::::::::::::cloddxxxxxxxdolloodddddddxkO00OOkOOOOOOOOOOOOOOOOOOOOOOOkkkkxxxxxxxxxxxxxxxxddoooooooollllc;,'';cooccdKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKkoc::::;;;::::::::::;,'''',,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,;;;;;:::;::::::::::::::codddxdoooooooooooooooodddoooooooollclokKWMMMMMMMMMMMMMMMMMMMMMMXd:::::::ccoKMMMMMMMMMMMMMMMMMMWKxx0WMWXOdooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKc,;,,;;;;;;;;;;;,,;;;;;;;;;::::;;;:cccllllc;'',,..,,,cddddddddddxxl,',,,,,;;;;;;::::::::::::::::::cloddxxxolcllooooodddxO00OkkkkOOOOOOOOOOOOOOOOOOkkkkkkxxxxxxxxxxxxxxddoooollllllc::;,'.....  .;clllo0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWN0koc:::;;;:::::::::::;,,',,,,,,,,,,,,,,,,,,,,,,,,;;;;,,,,,,,;;;;;;;;;;;;;;;;::;;;;;;:;;::::::::::::::codddoooooooooollooolloooooooooooooollclokXWMMMMMMMMMMMMMMMMMMMW0c:::::::clxNMMMMMMMMMMMMMMMMMW0xx0WMMMWKxooodkXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO;,;;;;;;;;;;;;;;,,,;;;;;;;;;;;:;;;,;;ccccc;;,','.',,,ldddddddddxxc'',,,,,,,;;;;;;::::::::::::::::::::clooolcloooooooddk00OkkkkkOOOOOOOOOOOOOOkkkkkxxxxxxxxxxxxxxxxxdollcccclllllolc;,.. ....    'cololldOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNKOdlc:;;;;;::::::::::;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;,,,;cddc;;;,;::;;::::::::::::::codooooooollllolllooollllooooooooooooolc:cdONMMMMMMMMMMMMMMMMMMNd::;:::::cdKWMMMMMMMMMMMMMMMMNOdkXMMMMMWXkdooox0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWx,,;;,;;;;;;;;;;;;,,,;;;;;;;;;;;;;;;,,,,;:;;:'','.',,;lddddddddxx:'''',,,,,,,;;;;;;:::::::::::::::::::cccccccllooooooxO0OkkkkkkOOOOOOOOOkkkkkkxxxxxxxxxxxxxxxxxxdllc:;;::clc:loollllc:c;........ .'ldddoollod0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKkdl:;;;;;;:::::::::;;;,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;;;,,,,''',,,;clc:;;;;;:;;;::::::::::::::cooolooolllllllllooolllllloooooooooooollc;;lkXWMMMMMMMMMMMMMMMWOc;;;:::::lkNMMMMMMMMMMMMMMMWKxxKWMMMMMMMNOdooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNo,,,;,;;;;;;;;;;;;,,,;;::;;;;;;;;;;;;;,,'',cc'',.',,';ldddddddxd;..''',,,,,,,;;;;;;;:::::::::::::::::ccccccccccclllok0Okkxxxkkkkkkkkkkkkkxxxxxxxxxxxxxxxxxxxdool::::,'':clll:;cllllllc::c,. ...... 'ldddddoocoXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOxoc:;;;;;::::::::::;;;,,,,,''''''',,,,,,,,,,,,,,,,,,,,'',,,;;;;;;:::;,,;::;;;::::::::::::::loollllllllllllloooollllllooooooooooolool:;;cxKWMMMMMMMMMMMMMMKl;;;:::::co0WMMMMMMMMMMMMMWXkOXWMMMMMMMMMW0dooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXl',,,,,,,,,;;;;;;;,,,;;;;;;;;;;;;,,,,,;;,,;l:','.',,',cdddddddd;...'''',,,,,,,;;;;;;;;::::::::::::::ccccc::;:::c:cdOOkkxxxxxxxxkkkxxxxxxxxxxxxxxxxxxxxxxxdooll:,...',:lllllll::lcllloool:'  ..  .. .cdddddolcoXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNX0kxolcc::;;;;::::;;:cc:;;,,,,,,,,''''''''''''''',,,,;;;;:::::cloxd:;:::;;;::::::::::::::lolllllllllllllloooolllllllooooooooolloll:;;;:dKWMMMMMMMMMMMMXo;;;;;;:::lONMMMMMMMMMMMMWK0KNMMMMMMMMMMMMWKxooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKc',,,,,,,,,,,,,;;;;,,,;;;;;;;;;;;;;;,,,,,,cl;','.','',cddddddo;.....''',,,,,,,,;;;;;;;;::::::::::::cc:;;,,,;::::lxOkkxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxdolooodxxoo:. .,,:llllllc;:ccclodddc. .....   .cddddoollkWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNXK0Oxxddolc::;::;;;;;;;;;;;;;;;;;,,;;;;::::ccllooddxxkO0XNWWOl::::;;;;::::::::::::clollllllllllllloooolllllcllolooolllllllll:;;;;cxXWMMMMMMMMMMNd;;;;;;;::cxXMMMMMMMMMMMMWNNMMMMMMMMMMMMMMMWKxooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:',,,,,,,,,,,,,,,,;,,,;;;;;;;;;;;;;;;;;;,:l:,,,'.'''',cdddddo;......''''',,,,,,,;;;;;;;;::::::::;;;;,,,,,;::::cdOkkxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxdooooodkOOOOxddc. ..';:clllllc;;cccloddc. ...... .:odddoolcdXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWNXK0OkkxdddooooooodddxxxxkO0KKKKKKXXXNNNWWWMMMMMMMMMKo::::;;;clc;:::::::::llllllllllllllllllllllllccllllllllllllllll:;;;;;ckXMMMMMMMMMXo;;,;;:;;:ldKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxooodONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0;',,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;;;;;;:lc;','..'''',cddddd;........''''',,,,,,,,;;;;;;;;;;;;;,,,''',;;;;;;:lkOkxxxxxxxxxxxxxxxxxxxxxxxxxxxxdolooddkOOOO0000Okxc'...',,,',;:cl:,;::cllc,. .......'ldddool:cOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWWWWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXo::::;;:c:;:::::;:::clllllllllllllllllllllllccclllllllllllllllc:;;;;;:lOWMMMMMMMKl;;;;;;;;:ld0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0doood0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0:',,,,,,,,,,,,,,,,,,,,,,,,,;;;;;;;;;;;;:ll:,,,'.''''',cdddd:..........'''''''',,,,,,,;;;;;;,,'''''',,,;;;;;cdkkxxxxxxxxxxxxxxxxxxxxxxxxxxdolccldxkOOOOOkkOOOOkxdl,.....''......'....''............;oloool:,,ckXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXd:;::;;,;;;;;;;;;;;:clllllllllllllllllllllllc:clllllllllllllllc;;;;;;;:dXMMMMMWOc;,,;;;;;:loOMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOooooxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl,''',,,,,,,,,,,,,,,,,,''',,,,,,,;;;;,:llc,',,'.''''',cdddc.............'''''''',,,,''',,''...'''',,,,,,,;lxkxxxxxxxxxxxxxxxxxxxxxxxxxdlcc:,.,oxddxkkkxkkkkxxkxoc'.'....'.....  .................'coolll:,,;,;cxXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx:;;;;,,;;;;;;:;;;:clllllllllllllllllllllllc::clllllllllllllll:;;;;;;;;l0WMMMNd;,,;;;;;;:llOMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkooookXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNKOxdlc:;,''',,,,,,,,,,,''''''',,,,,,;ool;',,'..''''';codl'................'''''''............'''',,,,,:cdkxxxxxxxxxxxxxxxddddxxxxdolc;,,:c:,;ododddddkkxkxdxxxo:'''....'.......................,llccclc,,,,,,,;cxXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOc,;;;,,,,;;;;;;;;:clllccllllllllllllllllllc::clllllllllloooll:;;;;;;;;ckNMWOc,;;;;;;;;cll0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxooodOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWNK0kdo;',,,,,,,,,,,,,,,,,,,,,;lxdc,,,,...'''',;cdd,....................''................''''',cxkkxxxxxxxxxxxxdddddddxdolc;'....,:cl:cddooollokkxxdodxxdc'''....'..  ........  ......';cooolccl::c:;,,,,,,cxXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKl,,;;;,,,,;;;;;;;:clllcclllllllllllllllllc:;:llllllllllooollc:;;;;;;;;:kNKl;,;;;;;;;:lldXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0doooxXMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWO;',,,,,,,,,,,,,,,,,,,:oxxo,',,...'''',;:lxl'.......................    ...............,lkxxxxxxxxxxxddddddddddocc:;'.....'loolc:lllllllxkxolccccodc'.....''.......... .'...''''.;looolcll::lllc:,,,,,,cxXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXx;,;;;,,,,,,;;;;;cllllclllllllllllllllllcc;;clllllllllllllllc:;;;;;;;;col;;;;;;;;;;cllOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXkooooOWMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;',,,,,,,,,,,,,,,,:cdxxd:,;,....'',;;;codddooooooooooooollcccc'.....................,dkxdddxxxxxxdddddddddolc,.';'......,lolc:;;;;,,,;:cccclccccl:. ........ ..........','.......coolclc:cllllll:;,,,,;lONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOc,,;;;,,,,,,;;;:clclccllllllllllllllllcc:;:clllclllllllllll:;;;;;;;;;,,,;;,,;;;;cllkNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0doooxXMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;''',,,,,,,,,',cxxdxxdl;::'...,;;;:;;lddxxkO00KKKKKKKKK0kkxxl'....................:xkxdddddxxdddddddddolc:,.. .,........,,,,,,,;;,'..','..'',,;:;. ''.......  ..........'........clccl::llooooollc:,'',;l0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXx;',;;,,,,,,,;:ccclccllllllllllllllllcc:;;:clllllcccllllllc:;;;;;;;;;;,,,,,,;:cco0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWMMMMXxoood0WMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk,'','',',,',ckKkdxddl::c,. .,:::::;:xOkO000KK00KKKKKKK00Okd,....................ckkddddddddddddddddlc:,.......................',;,,,'',,'''...,''',,'..,::;,'......'.............';cc:clloddddoooll:;,',;dKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKo,',;;,,,,,,;:cclcccllllllllllllllllcc;,;clccccccccllllll:;;;;;;;;;;;;;;;:cdxOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXO0WMMWOooooOWMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWx,''''''',lONNOodddl::c;...'::::::;lOK00KK00000000KKKKK00k;....................ckxddddddddddddddolcc,...  ....  ................;:;,'',,,,,''''..'''',,';ccccc:;,'..''''..''.......';:cllddddddddoollc:,,,:xXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOl,',;;;,,,;:ccccccclllllllllllccllcc:;;:cccccccccclllccc:;;;;;;;;;;:ccoxOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0oOWMMKdoookNMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx,'''.,o0WMWOodddl::c:'..':::::c;:x00000000K0000000000K0l....................:xxddddddddddddollc;;:;.. .......  ................,;;;::;,,,;,.........''':ccclcccc,..''....'.........'cloooodddoooooollc:;,,cOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0o;',;;;;;:ccccccccllllllllcccccccc:;,;cccccccccccllccc:;;;;;;;;;;;dKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0loKMMXxoooxXMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx;':dKWMMW0odddl::c:'...;::ccc:;o000000000000000000000x,...................;xxdddddddddddoccldd:',;,.........  ....................,,,,,,'.      ......,ccllllll:..''''.............,clooooooooooooooollc;,;oKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o;',;;;;cccccccclllccccccccccccc:;,;:ccccccccccclcccc;,;;;;;;;;;dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNk:ckWMNkoookNMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWK0NMMMMWKdodol;:cc,...;:cccc:;lO000000000000000000000l...................,dxddddddddddoc;;;codc'',,,........    ...................,........  .........;clloolc,...''...... .........,coooooooooooooooollc:;ckNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o;,;;;:cccccccccccccccccccccccc:,,;:cccccccccclc:cc;,;;;;;;,;;lKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0l:ckWMNkoookNMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKdodol::cc,...;cccccc:lO000000000000000000000k;...................cxddddddddolc:;;;,';oc,''',;'.............   .............'........  ..........,:c::;'.''''''.';''.. .....''..,coooooooooooooooollcccokXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0c,;;:cccccccccccccccccccccccc:;,,:cccccccccccc:cc;,;;;;;;;,,cOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o::cOWMXxood0WMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxodol:ccc;...;cccclc:lO0000000000000000000000o...................,ddddddddoc:;;;;;,,'.;;,,,;::;,'...............  ........ ............ ..''.............''''''.,c:',,. ....',,...;loooooooooooooooollllclxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWx;,;:ccccc:ccccccccccccccc::c:;,,;:cccccccccc::cc;,,,,,;;,,,;cdOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxc:::dXMMKdooxXMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxddol::cc;..':cccclc:lk00000000000000OO000000Oc................;:,cddddddoc:;,;;,,,,''..,:cccc;,,,'............''..............'''....... ..''.'''''.''''''.....,cll:',;......,,,'..,cllloooooolloooooolllcccd0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO;,;:ccccc:ccccccccccccccc::cc;,,,:cccccccccc:;:c;,,,,,,;,,,,;;:coxOKNWMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxl::::oKMMW0old0WMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxodoo::cc:'.,:cccccc:lk000000000000OOkOO000000x'...........';cdk0xcldodolccoo:,,,,,,,,'..;cccc:,,,'..............'''.......'''....................''''''''''....:lllll:,,;;'. .'',,,...:llllllllllllloooolllccccoONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0:,;:cccc:::cccccccccccccc::cc;,,,;:ccccccccc:;::;,,,,,,,,,,,;;;;;;::ldxO0KXNWWMMMMMMMMMMMMMMWNX0koc:::::o0WMMNkooONMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dddooc::c:,',:cccclc:lO0000000000OOkxxxkOO000OOl.........,cdk00000dcodolccokOd:,,,,;;::'';cccc:;,,'... ................. ........ ....................''.........,;:clllc,';::'...',,'. .':llllllllllllllooollccccclkXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM0:,;:cccc:::ccccccccccccc:::cc;,,,,;ccccccccc:,;:;,,,,,,,,,,,;;;;;;;;;;;;:cclodxxkkOOOOOOOkkxdoc:::::::cxXWMMW0ooONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNOdddool::c:,';ccccclc:ok00000000OOkkxxdddxxkkkkxd:.......;okO00000OOdcooccok00Od::::cclc;,:llllc;,,'.................     .......................'..',,....'...........,:clc'.;c:'...'''.....':lllllllllllllllollcccccclkNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO;,;:cccc:::cccc::::::::c:::cc;,,,,;:cccccccc;,;:;,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;::::::::::::::::::cd0WMMMMKxdONMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkoddoolc:c:;,:ccccclc:oO000000OOkkxxxdddddddxxdddd:.....cxO000000OOO0xc:lokOOO0klcllllll:;cllllc;,,''''....................''........................'cllc;..  ............,,'.':c:'. ..'..''...:llllllllllllllllllc:::cccokXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk,,::cccc:::cc::::::::::::::cc;,,,,,;:ccccccc;,;:;,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;::::::::::::lxKWMMMMMNkx0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxoddoolc:c:,,clcccllc:oO00000OOkxxdddddddddddddxxxko'..cxO0OOO0OOOOOOOkddkOOOOOOo:clllll::cllllc:,,'',,................''..........................''..'clll;.....................'::;,.....'''...:lllllllllcllllllllcc:::ccclxKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWNXKKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNo',:::::::::c:::::::::::::::cc;,,,,,;:ccccccc;,;:;,,,,,,,,,,,;;;;;;;;;;;;;;;;;;;;;;;;;;;:::;:::clxOXWMMMMMMNOkXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdodooool::;',lllccllc:oOOOOOOkkxxddddddddddddxxkkOOOOxoxO0OOOOOOOOOOOOOOOOOOOOOOd:clllllcclllllc:,'',,,'...........................................'','..,cllc',cc;,..   .......... ...',.  .'''....;cllllcccccclllllllcc::::c:;:xXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0xolodxOXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMK:';::::::::::::::::::::::::ccc;,,,,,,;:ccccc:;,;;,,,,,,,,,,,coc:;;;;;;;;;;;;;;;;;;;;;;;;;::coxkKNWMMMMMMMMN00NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdodolool:;;';lllccccccxOOOOOkxxdddddddddddxxkkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOdccllllllllllllc;,''',,,...................................'''''''',,,','..,cll:',oddo:'..   ...... ........ ..''.....':cccccccccccllllllc::::::::;;cdk0NWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN0dc;:d0NWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk,,::::::::::::::::::::::::::cc;,,,,,,;:ccccc:,,;;,,,,,,,,,,c0WNK0kxdolcccc:::::::ccclodxkOKXNWMMMMMMMMMMMNKXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dodollllc,,,coolcccc:lkOOOkxxdddddddddddxkkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOxcclllllllllllc:,',;;:::;...............................''''''''',,,,,,,,''..':cc:';odddolc;,'...    ............'.......;cccccccccccccccc::::::::cc:,.',:lx0XWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0l,';dKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXl,::::::::;::::::::::::::;:::cc;,,,',,,;:cccc;,,:;,,,,,,,,,,dNMMMMMMWWNXXXXKKKKKKKXXXNWWWMMMMMMMMMMMMMMMWNXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dddoolloc,';ldllccc::okOkkxdddoooodddxkkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOxc:clllllllllc:,'';cllccc,...........................''''''''''',;:;,,,,,,,,'..':cc;.;ododddxxdolcc,.    ............'''.....'''';:cccccc:::::;;;;;::::,'.''',:okKNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXx;'';kWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWk;;::::::::;:::::::::::::;;::ccc;,,,'',,,:ccc:;,;;;,,,,,,,'';xWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dddoolllc,':odolccc;cdOkxxdddoooddxkkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkdc:cllllllllc:,,;;;clcccl:....................'''''''''''''''''',:ccc:,,,,,,'''..':cc,.:odoodxxxxxxxdo:,..............''''.. ........'.....',;,,''''',,;;;,..''''',cx0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXo,'',dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl;::::::::;;:::::::::::::;:::ccc;,,'''',,;:::;,,;;,,,,,,,,,,:dXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dddooollc;;ldddoll::lxkxddoooddxxkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkxdlcllollllc::;;:codc:lccccl;...........''''''''''''''''''''''''',;cllccc:;,'''''''..':c:'.:oooddxxxxxxxxxxdc'',,,''''....''''. ...........................'',,'..'''''',cxXWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMKl,''':OMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXd;::::::::;;;;:::::::::::;:::::c:;,,''''',,;:;,',;;,,,,,,,,,cxx0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0dodooool:;codddddoccoxxdodddxxkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkxdollllolllc:;;::codxkko:ccccccc'..........''''''''''''''''''''''''',;cllclccc;,'',,''''..';:;'':ooodxxxxxxxxxxxd;.',,,;,','..'''...  .............................''..''''''.';o0NMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXo'''''cKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNx:::::::::;;;;;::::::::::;;:::::::;,,''''',,,;,,,;;,,,,,,',',dKxkWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdodooooc;:lddddddolodddddxkkOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkkkxxddddoollloollcc:;;:loxxkkkkkxc:cccccl;...........'''''''''''''''''''''''',:cllllccc:,'',,''''''..',;;''cooddxxdxxddddddo,.',,,,..,...''........................................''......,lkNMMMMMMMMMMMMMMMMMMMMMMMMMMMWWMMMMWO;'..''cKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWkc::::::::;;;;;;::::::::;;;::::;;::;,,''''',,,,,',;;,,,,,'''':0WOxNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKxodooolc:lodddddoloddddxkkOOOOOOOOOOOOOOOOOOOOOOOOOkkkkxxxxxxdddddoooollllooooollc:;;:codxkkkkkkkkkocccccccc'..........''''''''''''''''''''''''';clllllc:;,'',,,,'''''...',;;''coddddddddddddddl'.',,;'.,,...'.'''........ .....................................':xXWMMMMMMMMMMMMMMMMMMMMMMMNKNMMMMNd'....';OWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkl:::::::;;;;;;;::::::::;;;::::;;;::;,,'''''',,,',;;,,,'''''',oNM0xXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxddoolc:cooddddooloddxkkOOOOOkkOOkkkOOOOOOOOkkkxxxddooooooodddddddddxxdoooooolc:::clodxkkkkkkkkkkkkxlccccccc;..........'''''''''''''''''''''''''',:lllllc;,,,;:;,,''''''...',;;''coddddddddddddddc..',;,..;,.....''....,ccc:;,'....................... ............';dKWMMMMMMMMMMMMMMMMMMMMWKKWMMMMNo'....''lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWOl:::::::;;;;;;;;:::::::;;;::::;,,::::,''''''',,',,;;,,'''''',:OMM0xKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxddollccllllooollodxkkkkkkkkkkkkkkkkkkkkkkkxdooollooooooooooooddxxxdddooolcc::::cloddddddddxkkkkkkkkocccccccc'.........'''''''''''''''''''''''''''';cllllccccccc:;,''''''.....,;,..cddddddddddddddd;..,,;'.';,.....''....:oooolll:,....................................,o0NMMMMMMMMMMMMMMMMMMNOKMMMMMNo'......,dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNkc:::::;;;;;;;;;;::::::;;;;:::::,,;::::,''''''',,,,;;,,'''''',;dNMM0dKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxodolclllcccccclodxkkkkkkkkkkkkkkkkkkkkkkdolllloooooooooooooooooooollc:;::ccloooooolllllllllloodxxkkdccccc::c;........'''''''''''',,,,''''''''''''''';cllllcccccc:;,'''''''.....,;,..cddddddddddddddo,.',;,..,;,....'''...'llllllllc,''.'''...............................'ckXWMMMMMMMMMMMMMMMNxOMMMMMNd'.......,dNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMW0o:;:::;;;;;;;;;;;;:;::;;;;;:::::,',::::;,''''''',,,;;,''''''',,lKMMMOdKMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxoolllllcccccclodkkkkkkkkkkkkkkkkkkkkkkxdollooooooooooooolllllllc:::::clodxxkxolllllllc:;;;:cllllloooccccc::cc'........'''''''''''',,,'''''''''''''''',:cllllcccccc;,'''''''''....,,,.'cddddddddddddddl'.,;;..,,;;,...''....':lllllllc;'.......''.............................;oONMMMMMMMMMMMMMNddNMMMMWO;........,oKWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdc:;;;:;;;;;;;;;;;;;;;;;;;;::::::,',;::::;,''''''',,,;,'''''',,,:OWMMWkdXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxlllloolccccclodkkkkkkkkkkkkkkkkkkkkkxoollloooooolllc::;,;:ccc:;;:cldxkkkkkkkkoclllllolc:;;;;,:ccclcc:;:c::::c;........''''''''''''''''''''''''''''''''',:clllcccc:;''''.'''''''....,,,.'cdddddddddddoooc,,;;'.',,,;'..........,:cllllll:,.......''..................... ........':dONWMMMMMMMMMWk:kWMMMMXo'........';d0WMMMMMMMMMMMMMMMMMMMMMMMMMN0dc;;;;;;;;;;;;;;;;;;;;;;;;;;:::::;,'';:::::;'''''''',,;,'''''',,,;xWMMMNxxNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKdclloolllllllodkkkkkkkkkkkkkkkkkkkkxdollllllollcc::::::c;,:cc::cloxkkkkkkkkkkkxollllllloolclddc;:cccccl:;::::c:'.......''''''''''''''''''''''''''''''''''';cllcccc:,'''''..'''''''...',,;''cdddddddocc::;:;,,;'..,,,,,'...........,::cllllc:,'..''''....................... .........':okXWMMMMMMMKc;kNMMMW0:...........,ckKNWMMMMMMMMMMMMMMMMMWX0ko::;;;;;;;,;;;;;;;;;;;;;;;;;;::::;,'..';::::;,'''''''',;,'''''',,,,dNMMMMKokWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWX0xolllllllllllodxkkkkkkkkkkkkkkkkkkkxolllllllllc::ccldxxkkdlclc:coxkkkkkkkkkkkkkkkdlc::::::cloxkkxl;:cccccc;;:::c;.......'''''''''''''''',''''''''''''.'''''',:cclccc:;,'''''...'...''...',;;'.:oddddo:;;;;;,'',;,..,,,,,,'. ..........',:ccllllc:;,'''.....................................,cd0XWMMMWk;,l0WMMWx,............',cdkOKXNNNNNNNXKKOkxoc:;;;;;,,,,;;;;;;,,;;;;;;;;;;;;::;;,'.':c::::::;,'''''',,;,'''''',,',oXMMMMWkl0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOxollllllllllllldxkkkkkkkkkkkkkkkkkkkdocclllllcc:clodxkkkkkkolllc:lxkkkkkkkkkkkkkkkkkkxdoollodxxkkkkko::cc::::;,::::.......'''''''''''''''''''''''''''''''.'''''',:cccccc:;''''.''''''.'.....';;,'.:odooc,;;,,,;,,,,,..',,,'',,........ .'...',:cccllcc:,,'.......................................';lx0XWNx,.,lOXWNx,..............'',::cccccccc:;;;;;,,,,,,,,,;;;;;;,,;;;;;;;;;;;:;;;,,''',d0d:::::;,'''''',,;,'''''',,',oXMMMMMKldXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWXOdlcllllllllllllloxkkkkkkkkkkkkkkkkkkkxocclllllcclodxkkkkkkkkkdlllccdkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkxl::::::::,,;::,......''''''''''''''''''''''''''''''''''''''',;ccccccc:,'''',;,'.''.......';;;'.;ooo:,;,,''',,,,,..''''''','........ ..'....';:cccccc:;,'.........................................';o0NOc'.';cdxl'................',,;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;,,'''',o0WXo;::::;'''''',,,'''''''''',oXMMMMMNdcOWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMXxlccllllllllllllccoxkkkkkkkkkkkkkkkkkkkdlccccllc:ldkkkkkkkkkkkkko:clcokkkkkkkkkkkkkkkkkkkkkkkkkkkkkxkxxkko:;::::::;'';:;......''''''''''''''''''',''''''''''''''''''''',:ccccc::;''',::;'.'.........,;;,..:oo:,,,'...',,,,...''''..,,......... ..'''...';:ccccc:;;;;,,,,,'''''..','........... ........',;:loxKWMXo'.....''...................',;;;;;;;;;;;;;;;;;;;,,;;;;;;;;;;;;;;;;,,'''''.;xNMMKl::::;,''''',,,'''''''''',lKMMMMMWO:lXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
*/
