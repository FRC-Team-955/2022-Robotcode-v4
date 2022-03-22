#include "shooter.h"

void Shooter::ShootPercentOutput(double top_percent, double bottom_percent) {
  shooterneo_top->Set(top_percent);
  shooterneo_bottom->Set(-bottom_percent);
}

void Shooter::ShootVoltage(units::volt_t top_voltage, units::volt_t bottom_voltage) {
  shooterneo_top->SetVoltage(top_voltage);
  shooterneo_bottom->SetVoltage(-bottom_voltage);
}

double Shooter::GetTargetVelocityFromVoltage(units::volt_t volts){
  return double(volts*500);
}

void Shooter::VelocityControl(double top_velocity, double bottom_velocity) {
  // m_pidController_top->SetReference(top_velocity, rev::ControlType::kVelocity);
  // m_pidController_bottom->SetReference(-bottom_velocity, rev::ControlType::kVelocity);
  
  double top_arbFF = double(top_ff->Calculate(units::radians_per_second_t(top_velocity/60)))/frc::DriverStation::GetBatteryVoltage();
  double bottom_arbFF = double(bottom_ff->Calculate(units::radians_per_second_t(-bottom_velocity/60)))/frc::DriverStation::GetBatteryVoltage();
  // double bottom_arbFF = double(bottom_ff->Calculate(units::radians_per_second_t(shooterneo_bottom_encoder->GetVelocity()/60),units::radians_per_second_t(bottom_velocity/60),timer_shooter->Get()-last_time))/frc::DriverStation::GetBatteryVoltage();
  m_pidController_top->SetReference(top_velocity, rev::ControlType::kVelocity,0,top_arbFF);
  m_pidController_bottom->SetReference(-bottom_velocity, rev::ControlType::kVelocity,0,bottom_arbFF);
  //last_time = timer_shooter->Get();
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
