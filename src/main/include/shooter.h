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

    m_pidController_top = new rev::SparkMaxPIDController(shooterneo_top->GetPIDController());
    m_pidController_bottom = new rev::SparkMaxPIDController(shooterneo_bottom->GetPIDController());
  
    shooterneo_top_encoder = new rev::SparkMaxRelativeEncoder(shooterneo_top->GetEncoder());
    shooterneo_bottom_encoder= new rev::SparkMaxRelativeEncoder(shooterneo_bottom->GetEncoder());

    m_pidController_top->SetP(MechanismConst::top_kP);
    m_pidController_top->SetI(MechanismConst::top_kI);
    m_pidController_top->SetD(MechanismConst::top_kD);
    m_pidController_top->SetFF(MechanismConst::top_kFF);
    m_pidController_top->SetOutputRange(MechanismConst::top_kMinOutput,
                                   MechanismConst::top_kMaxOutput);

    m_pidController_bottom->SetP(MechanismConst::bottom_kP);
    m_pidController_bottom->SetI(MechanismConst::bottom_kI);
    m_pidController_bottom->SetD(MechanismConst::bottom_kD);
    m_pidController_bottom->SetFF(MechanismConst::bottom_kFF);
    m_pidController_bottom->SetOutputRange(MechanismConst::bottom_kMinOutput,
                                    MechanismConst::bottom_kMaxOutput);
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

  rev::SparkMaxPIDController *m_pidController_top;
  rev::SparkMaxPIDController *m_pidController_bottom;

  rev::SparkMaxRelativeEncoder *shooterneo_top_encoder;
  rev::SparkMaxRelativeEncoder *shooterneo_bottom_encoder;
};

#endif