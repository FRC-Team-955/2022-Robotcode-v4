// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"
#include "rev/CANSparkMax.h"
#include <frc/Joystick.h>
#include "ctre/Phoenix.h"

#include "shooter.h"
#include "drivebase.h"
#include "xy_align.h"
#include "elevator.h"

#include "settings.h"

using namespace rev;

DriveBase *drive;
Shooter *shooter;
XYalign *xyalign;
Elevator *elevator;

//Drive Neos
CANSparkMax *m_leftLeadMotor;
CANSparkMax *m_rightLeadMotor;
CANSparkMax *m_leftFollowMotor;
CANSparkMax *m_rightFollowMotor;
//Shooter Neos
CANSparkMax *shooterneo_top;
CANSparkMax *shooterneo_bottom;
//Elevator Talon
TalonFX *elevator_motor;

frc::Joystick *joystick_0;
frc::Joystick *joystick_1;

photonlib::PhotonCamera camera{"BallDetect"};
photonlib::PhotonCamera limecamera{"gloworm"};

frc::Timer * m_timer_intake;
frc::Timer * m_timer_elevator;

ButtonToggle elevator_lock;
ButtonToggle intake_deploy;

void Robot::RobotInit() {
  // frc::CameraServer::StartAutomaticCapture();
  // cs::CvSink cvSink = frc::CameraServer::GetVideo();
  // cs::CvSource outputStream = frc::CameraServer::PutVideo("Blur", 640, 480);
}
void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() {}
void Robot::AutonomousPeriodic() {}

void Robot::TeleopInit() {
  joystick_0 = new frc::Joystick(0);
  joystick_1 = new frc::Joystick(1);

  //drivebase
  m_leftLeadMotor = new CANSparkMax(DriveConst::kleft_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightLeadMotor = new CANSparkMax(DriveConst::kright_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_leftFollowMotor = new CANSparkMax(DriveConst::kleft_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightFollowMotor = new CANSparkMax(DriveConst::kright_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  drive = new DriveBase(m_leftLeadMotor, m_rightLeadMotor, m_leftFollowMotor, m_rightFollowMotor, joystick_0);
  xyalign = new XYalign(drive, joystick_0);
  //shooter
  shooterneo_top = new CANSparkMax(MechanismConst::shooter_top_port, CANSparkMax::MotorType::kBrushless);
  shooterneo_bottom = new CANSparkMax(MechanismConst::shooter_bottom_port, CANSparkMax::MotorType::kBrushless);
  shooter = new Shooter(shooterneo_top, shooterneo_bottom); 
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
    
    drive -> Drive(result);

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
