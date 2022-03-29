#ifndef DRIVEBASE
#define DRIVEBASE

#include "rev/CANSparkMax.h"
#include <frc/Joystick.h>
#include <frc/drive/DifferentialDrive.h>

#include "limelight.h"
#include "ballalign.h"
#include "settings.h"

using namespace rev;

class DriveBase {
public:
  DriveBase(CANSparkMax *m_leftLeadMotor, CANSparkMax *m_rightLeadMotor, CANSparkMax *m_leftFollowMotor, CANSparkMax *m_rightFollowMotor, frc::DifferentialDrive *differential_drive, frc::Joystick *joystick_0, Limelight *limelight, Ballalign *ballalign):
  m_leftLeadMotor(m_leftLeadMotor),m_rightLeadMotor(m_rightLeadMotor),m_leftFollowMotor(m_leftFollowMotor),m_rightFollowMotor(m_rightFollowMotor), differential_drive(differential_drive), joystick_0(joystick_0), limelight(limelight), ballalign(ballalign){
    m_leftLeadMotor->SetInverted(false);
    m_rightLeadMotor->SetInverted(true);
  };
  void Drive();
  void Align();
  void DriveBallAlign();
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
  Ballalign * ballalign;
};
#endif
