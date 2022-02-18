#ifndef SHOOTER
#define SHOOTER

#include "rev/CANSparkMax.h"

#include "settings.h"

using namespace rev;

class Shooter {
public:
  Shooter(CANSparkMax *shooterneo_top, CANSparkMax *shooterneo_bottom):
  shooterneo_top(shooterneo_top), shooterneo_bottom(shooterneo_bottom){
    shooterneo_top->SetSmartCurrentLimit(40);
    shooterneo_bottom->SetSmartCurrentLimit(40);

    m_pidController = new rev::SparkMaxPIDController(shooterneo_top->GetPIDController());
    m_pidController2= new rev::SparkMaxPIDController(shooterneo_bottom->GetPIDController());
  
    shooterneo_top_encoder = new rev::SparkMaxRelativeEncoder(shooterneo_top->GetEncoder());
    shooterneo_bottom_encoder= new rev::SparkMaxRelativeEncoder(shooterneo_bottom->GetEncoder());

    m_pidController->SetP(MechanismConst::kP);
    m_pidController->SetI(MechanismConst::kI);
    m_pidController->SetD(MechanismConst::kD);
    m_pidController->SetFF(MechanismConst::kFF);
    m_pidController->SetOutputRange(MechanismConst::kMinOutput,
                                   MechanismConst::kMaxOutput);

    m_pidController->SetP(MechanismConst::kP);
    m_pidController->SetI(MechanismConst::kI);
    m_pidController->SetD(MechanismConst::kD);
    m_pidController->SetFF(MechanismConst::kFF);
    m_pidController->SetOutputRange(MechanismConst::kMinOutput,
                                    MechanismConst::kMaxOutput);
  };
  //Takes in the two percent output of the motors and sets them to that output
  void ShootPercentOutput(double percent1, double percent2);
  //Takes in the two velocities of the motors and sets them to the velocities
  void VelocityControl(double top_velocity, double bottom_velocity);
  //Checks what the velocity of in inputed shooter motor ("Top", "Bottom")
  float VelocityOutput(std::string shooter_motor);
  void DisplayShooterInfo();

private:
  CANSparkMax *shooterneo_top;
  CANSparkMax *shooterneo_bottom;

  rev::SparkMaxPIDController *m_pidController;
  rev::SparkMaxPIDController *m_pidController2;

  rev::SparkMaxRelativeEncoder *shooterneo_top_encoder;
  rev::SparkMaxRelativeEncoder *shooterneo_bottom_encoder;
};

#endif