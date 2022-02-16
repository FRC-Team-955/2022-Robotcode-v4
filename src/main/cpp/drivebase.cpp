#include "drivebase.h"

void DriveBase::Drive(photonlib::PhotonPipelineResult result) {

  BallAimbot = joystick_0->GetRawAxis(Joy0Const::kball_aimbot_button);
  isQuickTurn = joystick_0->GetRawButton(Joy0Const::kquick_turn_button);

  if (buttontoggle.GetToggleNoDebounce(
          joystick_0->GetRawButton(Joy0Const::kreverse_drive))) {
    m_robotDrive.CurvatureDrive(
        -joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),
        joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), isQuickTurn);
        std::cout<<"reverse drive"<<std::endl;
  } else {
    if (BallAimbot > 0.2 && ball_detector.BallDetectorX(result) < 10000) {
      std::cout<<"aimbot drive"<<std::endl;
      m_robotDrive.TankDrive(
        ((ball_detector.BallDetectorX(result)/DriveConst::kturn_div)+0.20),
        -((ball_detector.BallDetectorX(result)/DriveConst::kturn_div)+0.20)
        );
    } else {
      m_robotDrive.CurvatureDrive(
          joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),
          joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), isQuickTurn);
    }
  }
}

void DriveBase::DisplayDriveInfo() {
  frc::Shuffleboard::GetTab("Telop").Add("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("leftFollowMotor", m_leftFollowMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("rightLeadMotor",m_rightLeadMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("rightFollowMotor", m_rightFollowMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Reverse Drive", ReverseDrive).WithWidget(frc::BuiltInWidgets::kBooleanBox);
  frc::Shuffleboard::GetTab("Telop").Add("Quick Turn", isQuickTurn).WithWidget(frc::BuiltInWidgets::kBooleanBox);
  frc::Shuffleboard::GetTab("Telop").Add("Ball Aimbot", BallAimbot).WithWidget(frc::BuiltInWidgets::kBooleanBox);


}

void DriveBase::DriveTank(float leftWheelInput, float rightWheelInput){

    m_robotDrive.TankDrive(leftWheelInput, rightWheelInput, false);
}