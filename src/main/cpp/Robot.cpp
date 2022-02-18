// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

//Not our classes?
#include <cameraserver/CameraServer.h>
#include "rev/CANSparkMax.h"
#include <frc/Joystick.h>
#include "ctre/Phoenix.h"
#include <frc/Timer.h>

//our classes
#include "xy_align.h"
#include "drivebase.h"
#include "intake.h"
#include "hopper.h"
#include "shooter.h"
#include "ballmanager.h"
#include "elevator.h"

#include "settings.h"

using namespace rev;

XYalign *xyalign;
DriveBase *drive;
Intake *intake;
Hopper *hopper;
Shooter *shooter;
BallManager *ball_manager;
Elevator *elevator;

//Timers
frc::Timer *m_timer_intake;
frc::Timer *m_timer_elevator;

//Joysticks
frc::Joystick *joystick_0;
frc::Joystick *joystick_1;

//Drive
CANSparkMax *m_leftLeadMotor;
CANSparkMax *m_rightLeadMotor;
CANSparkMax *m_leftFollowMotor;
CANSparkMax *m_rightFollowMotor;
//Intake
TalonSRX *intake_talon;
Solenoid *intake_solonoid_left;
Solenoid *intake_solonoid_right;
//Hopper
TalonSRX *talon_hopper_top;
TalonSRX *talon_hopper_bottom;
//Shooter
CANSparkMax *shooterneo_top;
CANSparkMax *shooterneo_bottom;
//Elevator Talon
TalonFX *elevator_motor;

photonlib::PhotonCamera camera{"BallDetect"};
photonlib::PhotonCamera limecamera{"gloworm"};

ButtonToggle intake_deploy;
ButtonToggle elevator_lock;

void Robot::RobotInit() {
  frc::CameraServer::StartAutomaticCapture();
  cs::CvSink cvSink = frc::CameraServer::GetVideo();
  cs::CvSource outputStream = frc::CameraServer::PutVideo("Blur", 640, 480);
}
void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() {}
void Robot::AutonomousPeriodic() {}

void Robot::TeleopInit() {
  //joysticks
  joystick_0 = new frc::Joystick(0);
  joystick_1 = new frc::Joystick(1);
  //drivebase
  m_leftLeadMotor = new CANSparkMax(DriveConst::kleft_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightLeadMotor = new CANSparkMax(DriveConst::kright_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_leftFollowMotor = new CANSparkMax(DriveConst::kleft_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightFollowMotor = new CANSparkMax(DriveConst::kright_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  drive = new DriveBase(m_leftLeadMotor, m_rightLeadMotor, m_leftFollowMotor, m_rightFollowMotor, joystick_0);
  xyalign = new XYalign(drive, joystick_0);
  //Intake
  intake_talon = new TalonSRX(MechanismConst::kintake_motor);
  intake_solonoid_left = new Solenoid{PneumaticsModuleType::CTREPCM, MechanismConst::kintake_solonoid_port_left};
  intake_solonoid_right = new Solenoid{PneumaticsModuleType::CTREPCM, MechanismConst::kintake_solonoid_port_right};
  intake = new Intake(intake_talon,intake_solonoid_left,intake_solonoid_right);
  //Hopper
  talon_hopper_top = new TalonSRX(MechanismConst::khopper_motor_top_port);
  talon_hopper_bottom = new TalonSRX(MechanismConst::khopper_motor_bottom_port);
  hopper = new Hopper(talon_hopper_top,talon_hopper_bottom);
  //shooter
  shooterneo_top = new CANSparkMax(MechanismConst::shooter_top_port, CANSparkMax::MotorType::kBrushless);
  shooterneo_bottom = new CANSparkMax(MechanismConst::shooter_bottom_port, CANSparkMax::MotorType::kBrushless);
  shooter = new Shooter(shooterneo_top, shooterneo_bottom); 
  //BallManager
  ball_manager = new BallManager(intake,hopper,shooter);
  //elevator
  elevator = new Elevator(elevator_motor);
  //timer
  m_timer_intake = new frc::Timer();
  m_timer_elevator = new frc::Timer();
}
void Robot::TeleopPeriodic() {
  photonlib::PhotonPipelineResult result = camera.GetLatestResult();
  photonlib::PhotonPipelineResult limeresult = limecamera.GetLatestResult();
  
  if(joystick_0->GetRawAxis(Joy0Const::kshoot_trigger) && xyalign->HasTargetLimeLight(limeresult)){
    //auto align
    xyalign->Align(limeresult);
  }
  //driver control
  else{
    drive->Drive(result);

    if(joystick_1->GetRawAxis(Joy1Const::kshoot_wall_trigger)){
      if(ball_manager->Rev(MechanismConst::khigh_target,MechanismConst::khigh_target)){
        ball_manager->Shoot();
      }
    }else{
      //if not shooting
      if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
        ball_manager->Reject();
      }else{
        //if not rejecting
        // if(ball_manager->IsFull()){
        // }
        // if(joystick_)
        if (intake_deploy.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
          intake->PistonDown();
          //If the intake is in the down state allow the intake to run
          if(joystick_1->GetRawButton(Joy1Const::kintake_motor_run)){
            intake->RunIntake(1);
            m_timer_intake->Start();
            m_timer_intake->Reset();
          }
        }else{
          intake->PistonUp();
        }
        //if the m_intake_timer is less than 5s then run the hopper
        if(m_timer_intake->Get()<5_s){
          ball_manager->LoadHopper();
        }
      }
    }
    //check if its around time to climb
    if(m_timer_elevator->GetMatchTime()>90_s){
      if(elevator_lock.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kelevator_lock_button))){
        elevator->LockElevator();
      }else{
        elevator->UnlockElevator();
      }
    elevator->ElevatorMove(joystick_1->GetRawAxis(Joy1Const::kelevator_axis));
    }
    
  }
  
}

void Robot::DisabledInit() {
  delete m_leftLeadMotor;
  delete m_rightLeadMotor;
  delete m_leftFollowMotor;
  delete m_rightFollowMotor;
  delete shooterneo_top;
  delete shooterneo_bottom;
  delete shooter;
  delete drive;
}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
