#include "drivebase.h"

// void DriveBase::Drive(photonlib::PhotonPipelineResult result) {
void DriveBase::Drive() {
  if(joystick_0->GetRawButton(Joy0Const::khorizontal_align_button)){
    differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), limelight->GetDrivebaseSpeed());
  }else{
    differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis));
  }
}

void DriveBase::DisplayDriveInfo() {
  frc::SmartDashboard::PutNumber("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("leftFollowMotor", m_leftFollowMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightLeadMotor", m_rightLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightFollowMotor", m_rightFollowMotor->GetOutputCurrent());
}

void DriveBase::DriveTank(float leftWheelInput, float rightWheelInput){
    differential_drive->TankDrive(leftWheelInput, rightWheelInput, false);
}