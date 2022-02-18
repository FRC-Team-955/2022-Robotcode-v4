#include "drivebase.h"

void DriveBase::Drive(photonlib::PhotonPipelineResult result) {

  ball_aimbot = joystick_0->GetRawAxis(Joy0Const::kball_aimbot_button);
  is_quick_turn = joystick_0->GetRawButton(Joy0Const::kquick_turn_button);

  if (buttontoggle.GetToggleNoDebounce(
          joystick_0->GetRawButton(Joy0Const::kreverse_drive))) {
    m_robotDrive.CurvatureDrive(
        -joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),
        joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), is_quick_turn);
        std::cout<<"reverse drive"<<std::endl;
  } else {
    if (ball_aimbot > 0.2 && BallDetect::BallDetectorX(result) < 10000) {
      std::cout<<"aimbot drive"<<std::endl;
      m_robotDrive.TankDrive(
        ((BallDetect::BallDetectorX(result)/DriveConst::kturn_div)+0.20),
        -((BallDetect::BallDetectorX(result)/DriveConst::kturn_div)+0.20)
        );
    } else {
      m_robotDrive.CurvatureDrive(
          joystick_0->GetRawAxis(Joy0Const::kdrive_speed_axis),
          joystick_0->GetRawAxis(Joy0Const::kdrive_curvature_axis), is_quick_turn);
    }
  }
}

void DriveBase::DisplayDriveInfo() {
  frc::Shuffleboard::GetTab("Telop").Add("LeftLeadAmps", m_leftLeadMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("leftFollowMotor", m_leftFollowMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("rightLeadMotor",m_rightLeadMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("rightFollowMotor", m_rightFollowMotor->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Reverse Drive", ReverseDrive).WithWidget(frc::BuiltInWidgets::kBooleanBox);
  frc::Shuffleboard::GetTab("Telop").Add("Quick Turn", is_quick_turn).WithWidget(frc::BuiltInWidgets::kBooleanBox);
  frc::Shuffleboard::GetTab("Telop").Add("Ball Aimbot", ball_aimbot).WithWidget(frc::BuiltInWidgets::kBooleanBox);
}

void DriveBase::DriveTank(float leftWheelInput, float rightWheelInput){
    m_robotDrive.TankDrive(leftWheelInput, rightWheelInput, false);
}