#ifndef SHOOTER
#define SHOOTER

#include "rev/CANSparkMax.h"
#include <frc/DoubleSolenoid.h>
// #include <frc/controller/SimpleMotorFeedforward.h>
#include <frc/DriverStation.h>
#include <frc/Timer.h>

#include "settings.h"

using namespace rev;

class Shooter {
public:
  Shooter(CANSparkMax *shooterneo_top, CANSparkMax *shooterneo_bottom, frc::DoubleSolenoid *shooter_solenoid):
  shooterneo_top(shooterneo_top), shooterneo_bottom(shooterneo_bottom), shooter_solenoid(shooter_solenoid){
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
    m_pidController_top->SetOutputRange(MechanismConst::top_kMinOutput,MechanismConst::top_kMaxOutput);

    m_pidController_bottom->SetP(MechanismConst::bottom_kP);
    m_pidController_bottom->SetI(MechanismConst::bottom_kI);
    m_pidController_bottom->SetD(MechanismConst::bottom_kD);
    m_pidController_bottom->SetFF(MechanismConst::bottom_kFF);
    m_pidController_bottom->SetOutputRange(MechanismConst::bottom_kMinOutput,MechanismConst::bottom_kMaxOutput);
    

    bottom_ff = new frc::SimpleMotorFeedforward<units::radians>(AutoConst::kS,AutoConst::kV, AutoConst::kA);
    top_ff = new frc::SimpleMotorFeedforward<units::radians>(AutoConst::kS,AutoConst::kV, AutoConst::kA);

    // timer_shooter = new frc::Timer();
    // timer_shooter->Reset();
    // timer_shooter->Start();
  };
  ~Shooter() {
    delete m_pidController_top;
    delete m_pidController_bottom;
    delete shooterneo_top_encoder;
    delete shooterneo_bottom_encoder;
    // delete timer_shooter;
    delete bottom_ff;
  }
  //Takes in the two percent output of the motors and sets them to that output
  void ShootPercentOutput(double top_percent, double bottom_percent);
  //Takes in the two velocities of the motors and sets them to the velocities
  void VelocityControl(double top_velocity, double bottom_velocity);
  //Checks what the velocity of in inputed shooter motor ("Top", "Bottom")
  float VelocityOutput(std::string shooter_motor);

  void ShootVoltage(units::volt_t top_voltage, units::volt_t bottom_voltage);
  double GetTargetVelocityFromVoltage(units::volt_t volts);

  void SolenoidUp();
  void SolenoidDown();
  void DisplayShooterInfo();

private:
  CANSparkMax *shooterneo_top;
  CANSparkMax *shooterneo_bottom;
  frc::DoubleSolenoid *shooter_solenoid;

  rev::SparkMaxPIDController *m_pidController_top;
  rev::SparkMaxPIDController *m_pidController_bottom;

  rev::SparkMaxRelativeEncoder *shooterneo_top_encoder;
  rev::SparkMaxRelativeEncoder *shooterneo_bottom_encoder;
  frc::SimpleMotorFeedforward<units::radians> *bottom_ff;
  frc::SimpleMotorFeedforward<units::radians> *top_ff;
  // frc::Timer *timer_shooter;
  // double last_time = 0;
};

#endif