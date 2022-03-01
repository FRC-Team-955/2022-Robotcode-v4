#include "drivebase.h"

// void DriveBase::Drive(photonlib::PhotonPipelineResult result) {
void DriveBase::Drive() {
  differential_drive->ArcadeDrive(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis)*ReverseDrive, joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)*0.5);
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