#include "shooter.h"

void Shooter::ShootPercentOutput(double top_percent, double bottom_percent) {
  shooterneo_top->Set(top_percent);
  shooterneo_bottom->Set(-bottom_percent);
}
void Shooter::VelocityControl(double top_velocity, double bottom_velocity) {
  m_pidController_top->SetReference(top_velocity, rev::ControlType::kVelocity);
  m_pidController_bottom->SetReference(-bottom_velocity, rev::ControlType::kVelocity);
  
  // std::cout<<double(bottom_ff->Calculate(units::revolutions_per_minute_t(shooterneo_bottom_encoder->GetVelocity()),units::revolutions_per_minute_t(-bottom_velocity))/(frc::DriverStation::GetBatteryVoltage()*5700))<<std::endl;
  // m_pidController_bottom->SetReference(-bottom_velocity, rev::ControlType::kVelocity,0, double(bottom_ff->Calculate(units::revolutions_per_minute_t(shooterneo_bottom_encoder->GetVelocity()),units::revolutions_per_minute_t(-bottom_velocity))/(frc::DriverStation::GetBatteryVoltage()*5700)));
}
float Shooter::VelocityOutput(std::string shooter_motor){
  if (shooter_motor == "Top"){
    return shooterneo_top_encoder->GetVelocity();
  }else if(shooter_motor == "Bottom"){
    return -shooterneo_bottom_encoder->GetVelocity();
  }
  else{
    return 0;
  }
}
void Shooter::SolenoidUp(){
  shooter_solenoid->Set(frc::DoubleSolenoid::Value::kForward);
}
void Shooter::SolenoidDown(){
  shooter_solenoid->Set(frc::DoubleSolenoid::Value::kReverse);
}
void Shooter::DisplayShooterInfo(){
  frc::SmartDashboard::PutNumber("Shooter Top Amp", shooterneo_top->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Shooter Top Velocity", shooterneo_top_encoder->GetVelocity());
  frc::SmartDashboard::PutNumber("Shooter Bottom Amp", shooterneo_bottom->GetOutputCurrent());
  frc::SmartDashboard::PutNumber("Shooter Bottom Velocity", shooterneo_bottom_encoder->GetVelocity());
}
