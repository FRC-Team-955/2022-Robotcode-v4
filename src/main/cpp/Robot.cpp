#include "Robot.h"
#include "iostream"

//Not our classes?
#include <cameraserver/CameraServer.h>
#include <frc/Joystick.h>
#include <frc/Timer.h>
#include <frc/MotorSafety.h>
//del maybe
#include <frc/Compressor.h>

//our classes
#include "xy_align.h"
#include "drivebase.h"
#include "intake.h"
#include "hopper.h"
#include "shooter.h"
#include "colorsensor.h"
#include "ballmanager.h"
#include "elevator.h"

#include "settings.h"

using namespace rev;

DriveBase *drive;


//Joysticks
frc::Joystick *joystick_0;
frc::Joystick *joystick_1;
//Drive
CANSparkMax *m_leftLeadMotor;
CANSparkMax *m_rightLeadMotor;
CANSparkMax *m_leftFollowMotor;
CANSparkMax *m_rightFollowMotor;
frc::DifferentialDrive *differential_drive;
ButtonToggle *reverse_drive_toggle;
//Intake
TalonSRX *intake_talon;
DoubleSolenoid *intake_double_solonoid;
//Hopper
TalonSRX *talon_hopper_top;
TalonSRX *talon_hopper_bottom;
//Shooter
CANSparkMax *shooterneo_top;
CANSparkMax *shooterneo_bottom;
//Elevator
TalonFX *elevator_motor;

frc::Timer *m_timer_intake;
frc::Timer *m_timer_elevator;



ButtonToggle intake_deploy_toggle;
ButtonToggle elevator_lock_toggle;

// chris is so cool 
// bryan ganyu simp
// ganyu is pog
// who?tao
// hi guys its bryan welcome back to my channel today im teaching you * minecraft *
// Thank you for listening to my ted talk

void Robot::RobotInit() {
  // frc::CameraServer::StartAutomaticCapture();
  // cs::CvSink cvSink = frc::CameraServer::GetVideo();
  // cs::CvSource outputStream = frc::CameraServer::PutVideo("Driver Cam", 640, 480);
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
  differential_drive = new frc::DifferentialDrive(*m_leftLeadMotor,*m_rightLeadMotor);
  differential_drive->SetSafetyEnabled(false);
  drive = new DriveBase(m_leftLeadMotor,m_rightLeadMotor,m_leftFollowMotor,m_rightFollowMotor,differential_drive,reverse_drive_toggle, joystick_0);

  m_timer_intake = new frc::Timer();
  m_timer_elevator = new frc::Timer();
}
void Robot::TeleopPeriodic() {

  // drive ->Drive();
  // elevator->ElevatorMove(joystick_0->GetRawAxis(Joy1Const::kelevator_axis)*0.1);
  // if (joystick_0->GetRawButton(4)){
  //   elevator->ResetPosition();
  // }
  
  //runs the shuffle board display
  // DisplayShuffle();
  // compressor.EnableDigital();
    //   if(compressor->DetectPressure()){
    //   compressor->TurnOnCompressor();
    // }else{
    //   compressor->TurnOffCompressor();
    // }

  //updates hopper state
  
  /*if(joystick_0->GetRawAxis(Joy0Const::kshoot_trigger) && xyalign->HasTargetLimeLight(limeresult)){
    //auto align
    xyalign->Align(camera_result);
    ball_manager->CheckHopperState();
  }
  //driver control
  else{
    drive->Drive(photon_result);

    //compressor
    if(compressor->DetectPressure()){
      compressor->TurnOnCompressor();
    }else{
      compressor->TurnOffCompressor();
    }
    //shooting
    if(joystick_1->GetRawAxis(Joy1Const::kshoot_wall_trigger)){
      if(ball_manager->Rev(MechanismConst::khigh_target,MechanismConst::khigh_target)){
        ball_manager->Shoot();
        ball_manager->CheckHopperState();
      }
    }else{
      //if not shooting
      if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
        ball_manager->Reject();
      }else{
        //if not rejecting
        if (intake_deploy_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
          intake->PistonDown();
          //If the intake is in the down state allow the intake to run
          if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)){
            intake->RunIntake(1);
            m_timer_intake->Start();
            m_timer_intake->Reset();
            ball_manager->CheckHopperState();
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
      if(elevator_lock_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kelevator_lock_button))){
        elevator->LockElevator();
      }else{
        elevator->UnlockElevator();
      }
    elevator->ElevatorMove(joystick_1->GetRawAxis(Joy1Const::kelevator_axis));
    }
    
  }*/
  
}
void Robot::DisplayShuffle(){
  drive->DisplayDriveInfo();
  intake->DisplayIntakeInfo();
  hopper->DiplayHopperInfo();
  shooter->DisplayShooterInfo();
  ball_manager->DisplayBallManagerInfo();
  elevator->DisplayElevatorInfo();
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
  delete drive;
  delete xyalign;
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
  delete shooter;
  //color sensor
  //delete rev_color_sensor;
  //delete color_match;
  //delete color_sensor;
  //ir break beam
  //BallManager
  //delete ball_manager;
  delete ir_break_beam;
  //elevator
  delete elevator_motor;
  delete limit_switch_top;
  delete limit_switch_bottom;
  delete elevator;
  //compressor
  delete compressor;    
  //timer
  delete m_timer_intake;
  delete m_timer_elevator;
}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
