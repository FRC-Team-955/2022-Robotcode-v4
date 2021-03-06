#ifndef DRIVEBASE
#define DRIVEBASE

#include "rev/CANSparkMax.h"
#include <frc/Joystick.h>
#include <frc/drive/DifferentialDrive.h>

#include "limelight.h"
#include "settings.h"

using namespace rev;

class DriveBase {
public:
  DriveBase(CANSparkMax *m_leftLeadMotor, CANSparkMax *m_rightLeadMotor, CANSparkMax *m_leftFollowMotor, CANSparkMax *m_rightFollowMotor, frc::DifferentialDrive *differential_drive, frc::Joystick *joystick_0, Limelight *limelight):
  m_leftLeadMotor(m_leftLeadMotor),m_rightLeadMotor(m_rightLeadMotor),m_leftFollowMotor(m_leftFollowMotor),m_rightFollowMotor(m_rightFollowMotor), differential_drive(differential_drive), joystick_0(joystick_0), limelight(limelight){
    m_leftLeadMotor->SetInverted(false);
    m_rightLeadMotor->SetInverted(true);
    m_leftLeadMotor->SetSmartCurrentLimit(40);
    m_rightLeadMotor->SetSmartCurrentLimit(40);
  };
  void Drive();
  void Align();
  void Align(double joystick_offset);
  void AlignAuto();
  void AlignToOffset(double offset);
  void DisplayDriveInfo();
private:
  CANSparkMax *m_leftLeadMotor;
  CANSparkMax *m_rightLeadMotor;
  CANSparkMax *m_leftFollowMotor;
  CANSparkMax *m_rightFollowMotor;
  frc::DifferentialDrive *differential_drive;
  frc::Joystick *joystick_0;
  Limelight *limelight;
};
#endif
