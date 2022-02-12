#include "shooter.h"
using namespace frc;
using namespace rev;

void Shooter::ShootPercentOutput(int percent1, int percent2) {
  shooterneo_lead.Set(percent1);
  shooterneo_follow.Set(percent2);
}
void Shooter::VelocityControl(int lead_velocity, int follow_velocity) {
  m_pidController.SetReference(lead_velocity, rev::ControlType::kVelocity);
  m_pidController2.SetReference(follow_velocity, rev::ControlType::kVelocity);
}
float Shooter::VelocityOutput(std::string shooter_motor){
  if (shooter_motor == "Top"){
    return shooterneo_lead.GetEncoder().GetVelocity();
  }else if(shooter_motor == "Bottom"){
    return shooterneo_follow.GetEncoder().GetVelocity();
  }
  else{
    return 0;
  }
}
void Shooter::DisplayShooterInfo(){
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Top Amp",shooterneo_lead.GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Top Velocity",shooterneo_lead.GetEncoder().GetVelocity());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Bottom Amp",shooterneo_follow.GetOutputCurrent());
  frc::Shuffleboard::GetTab("Telop").Add("Shooter Bottom Velocity",shooterneo_follow.GetEncoder().GetVelocity());
}
