#ifndef DRIVEBASE
#define DRIVEBASE

#include "iostream"
#include "rev/CANSparkMax.h"
#include <frc/Joystick.h>
#include <frc/drive/DifferentialDrive.h>
#include "balldetect.h"
#include <button_toggle.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <cameraserver/CameraServer.h>
#include "settings.h"

using namespace rev;

class DriveBase {
public:
  DriveBase(CANSparkMax *m_leftLeadMotor, CANSparkMax *m_rightLeadMotor, CANSparkMax *m_leftFollowMotor, CANSparkMax *m_rightFollowMotor, frc::Joystick *joystick_0):
  m_leftLeadMotor(m_leftLeadMotor),m_rightLeadMotor(m_rightLeadMotor),m_leftFollowMotor(m_leftFollowMotor),m_rightFollowMotor(m_rightFollowMotor), joystick_0(joystick_0){
    m_rightFollowMotor->Follow(*m_rightLeadMotor);
    m_leftFollowMotor->Follow(*m_leftLeadMotor);
    m_leftLeadMotor->SetInverted(DriveConst::kleft_lead_is_inverted);
    m_rightLeadMotor->SetInverted(DriveConst::kright_lead_is_inverted);
  };
  void Drive(photonlib::PhotonPipelineResult result);
  BallDetect ball_detector;
  void DisplayDriveInfo();
  void DriveTank(float leftWheelInput, float rightWheelInput);

private:
  CANSparkMax *m_leftLeadMotor;
  CANSparkMax *m_rightLeadMotor;
  CANSparkMax *m_leftFollowMotor;
  CANSparkMax *m_rightFollowMotor;

  frc::Joystick *joystick_0;

  frc::DifferentialDrive m_robotDrive{*m_leftLeadMotor, *m_rightLeadMotor};

  ButtonToggle buttontoggle{};

  bool isQuickTurn = false;
  bool ReverseDrive = false;
  bool BallAimbot = false;
};
#endif
