#include "drivebase.h"

void DriveBase::Drive() {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis)*.4, joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)*.4);
}
void DriveBase::Align() {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), limelight->GetDrivebaseSpeed()+(0.2*joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)));
}
void DriveBase::Align(double joystick_offset) {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), limelight->GetDrivebaseSpeed()+joystick_offset);
}
void DriveBase::AlignAuto() {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), limelight->GetDrivebaseSpeedAuto());
}
void DriveBase::AlignToOffset(double offset) {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), 0.8*limelight->GetDrivebaseSpeedToOffset(offset));
}

void DriveBase::DisplayDriveInfo() {
  frc::SmartDashboard::PutNumber("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("leftFollowAmps", m_leftFollowMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightLeadAmps", m_rightLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightFollowAmps", m_rightFollowMotor->GetOutputCurrent());
}