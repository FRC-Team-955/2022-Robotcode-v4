#include "drivebase.h"

// void DriveBase::Drive(photonlib::PhotonPipelineResult result) {
void DriveBase::Drive() {
  if (joystick_0->GetRawButton(Joy0Const::kreverse_drive)){
    ReverseDrive = 1;
  }else{
    ReverseDrive = -1;
  }
  differential_drive->ArcadeDrive(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis)*ReverseDrive, joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)*0.5);

  // drive = joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis);
  // turn = -joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)/2;
  // drive = (drive + drive_old + drive_old + drive_old)/4;
  // turn = (turn + turn_old + turn_old + turn_old)/4;
  // differential_drive->ArcadeDrive(drive*ReverseDrive, turn*ReverseDrive);
  // drive_old = drive;
  // turn_old = turn;
  // ball_aimbot = joystick_0->GetRawAxis(Joy0Const::kball_aimbot_button);
  // is_quick_turn = joystick_0->GetRawButton(Joy0Const::kquick_turn_button);

  // if (reverse_drive_toggle->GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kreverse_drive))) {
  //   //in reverse
  //   if(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis)<0.05){
  //     differential_drive->CurvatureDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), true);
  //   }else{
  //     differential_drive->CurvatureDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), false);
  //   }
  //   std::cout<<"reverse drive"<<std::endl;
  // }else {
  //   // if (ball_aimbot > 0.2 && BallDetect::BallDetectorX(result) < 10000) {
  //   //   std::cout<<"aimbot drive"<<std::endl;
  //   //   differential_drive->TankDrive(
  //   //     ((BallDetect::BallDetectorX(result)/DriveConst::kturn_div)+0.20),
  //   //     -((BallDetect::BallDetectorX(result)/DriveConst::kturn_div)+0.20)
  //   //     );
  //   // } 
  //   //normal drive
  //   if(abs(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis))<0.05){
  //     std::cout<<"quick drive"<<std::endl;
  //     differential_drive->CurvatureDrive(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), true);
  //   }else {
  //     std::cout<<"normal drive"<<std::endl;
  //     differential_drive->CurvatureDrive(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), false);
  //   }
  // }
}

void DriveBase::DisplayDriveInfo() {
  frc::SmartDashboard::PutNumber("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("leftFollowMotor", m_leftFollowMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightLeadMotor", m_rightLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightFollowMotor", m_rightFollowMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Reverse Drive", 2);
}

void DriveBase::DriveTank(float leftWheelInput, float rightWheelInput){
    differential_drive->TankDrive(leftWheelInput, rightWheelInput, false);
}