#include "drivebase.h"

void DriveBase::Drive() {
  if (std::abs(joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis))<0.05){
    differential_drive->CurvatureDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis),true);
  }else{
    differential_drive->CurvatureDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis),false);
  }
  // differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis)*0.5);
}
void DriveBase::Align() {
  differential_drive->ArcadeDrive(-joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis), limelight->GetDrivebaseSpeed());
}

void DriveBase::DisplayDriveInfo() {
  frc::SmartDashboard::PutNumber("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("leftFollowAmps", m_leftFollowMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightLeadAmps", m_rightLeadMotor->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("rightFollowAmps", m_rightFollowMotor->GetOutputCurrent());
}