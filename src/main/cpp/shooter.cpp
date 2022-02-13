#include "shooter.h"
using namespace frc;
using namespace rev;

void Shooter::ShootPercentOutput(double percent1, double percent2) {
  shooterneo_top->Set(percent1);
  shooterneo_bottom->Set(percent2);
}
void Shooter::VelocityControl(double top_velocity, double bottom_velocity) {
  m_pidController->SetReference(top_velocity, rev::ControlType::kVelocity);
  m_pidController->SetReference(bottom_velocity, rev::ControlType::kVelocity);
}
float Shooter::VelocityOutput(std::string shooter_motor){
  if (shooter_motor == "Top"){
    return shooterneo_top_encoder->GetVelocity();
  }else if(shooter_motor == "Bottom"){
    return shooterneo_bottom_encoder->GetVelocity();
  }
  else{
    return 0;
  }
}
void Shooter::DisplayShooterInfo(){
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Top Amp",shooterneo_top->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Top Velocity",shooterneo_top_encoder->GetVelocity());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Bottom Amp",shooterneo_bottom->GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Bottom Velocity",shooterneo_bottom_encoder->GetVelocity());
}
