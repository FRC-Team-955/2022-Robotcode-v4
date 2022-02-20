#include "shooter.h"

void Shooter::ShootPercentOutput(double top_percent, double bottom_percent) {
  shooterneo_top->Set(top_percent);
  shooterneo_bottom->Set(-bottom_percent);
}
void Shooter::VelocityControl(double top_velocity, double bottom_velocity) {
  m_pidController_top->SetReference(top_velocity, rev::ControlType::kVelocity);
  m_pidController_bottom->SetReference(-bottom_velocity, rev::ControlType::kVelocity);
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
