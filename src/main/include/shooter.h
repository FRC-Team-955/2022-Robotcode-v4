#ifndef SHOOTER
#define SHOOTER
#include <iostream>
#include "ctre/Phoenix.h"
#include "rev/CANSparkMax.h"

#include "settings.h"

using namespace rev;

class Shooter {
  Shooter(){
    shooterneo_lead.SetSmartCurrentLimit(40);
    shooterneo_follow.SetSmartCurrentLimit(40);

    m_pidController.SetP(MechanismConst::kP);
    m_pidController.SetI(MechanismConst::kI);
    m_pidController.SetD(MechanismConst::kD);
    m_pidController.SetFF(MechanismConst::kFF);
    m_pidController.SetOutputRange(MechanismConst::kMinOutput,
                                   MechanismConst::kMaxOutput);

    m_pidController2.SetP(MechanismConst::kP);
    m_pidController2.SetI(MechanismConst::kI);
    m_pidController2.SetD(MechanismConst::kD);
    m_pidController2.SetFF(MechanismConst::kFF);
    m_pidController2.SetOutputRange(MechanismConst::kMinOutput,
                                    MechanismConst::kMaxOutput);
  };

public:
  //Takes in the two percent output of the motors and sets them to that output
  void ShootPercentOutput(int percent1, int percent2);
  //Takes in the two velocities of the motors and sets them to the velocities
  void VelocityControl(int lead_velocity, int follow_velocity);
  //checks what the velocity of in inputed shooter motor ("Top", "Bottom")
  float VelocityOutput(std::string shooter_motor);
  void DisplayShooterInfo();

private:
  rev::CANSparkMax shooterneo_lead{MechanismConst::shooter_lead_port,
                                   CANSparkMax::MotorType::kBrushless};
  rev::CANSparkMax shooterneo_follow{MechanismConst::shooter_follow_port,
                                     CANSparkMax::MotorType::kBrushless};
  rev::SparkMaxPIDController m_pidController =
      shooterneo_lead.GetPIDController();
  rev::SparkMaxPIDController m_pidController2 =
      shooterneo_follow.GetPIDController();
};

#endif