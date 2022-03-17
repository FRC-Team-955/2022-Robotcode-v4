#include <auto.h>
/**
 * A 2nd constructor that must be run in auto init
 */
void Auto::Initilize(rev::CANSparkMax *left_spark, rev::CANSparkMax *right_spark) {
  auto_controller = new frc::RamseteController();
  auto_timer = new frc::Timer();
  m_odometry = new frc::DifferentialDriveOdometry(navx->GetRotation2d());
  kinematics = new frc::DifferentialDriveKinematics(AutoConst::ktrack_width);
  m_leftLeadMotor = left_spark;
  m_rightLeadMotor = right_spark;
  drive_pid_left = new rev::SparkMaxPIDController(m_leftLeadMotor->GetPIDController());
  drive_pid_right = new rev::SparkMaxPIDController(m_rightLeadMotor->GetPIDController());
  drive_encoder_left = new rev::SparkMaxRelativeEncoder(m_leftLeadMotor->GetEncoder());
  drive_encoder_right = new rev::SparkMaxRelativeEncoder(m_rightLeadMotor->GetEncoder());

  drive_encoder_left->SetPosition(0);
  drive_encoder_right->SetPosition(0);

  drive_pid_left->SetP(0.000014053);//0.00008
  drive_pid_left->SetI(0);
  drive_pid_left->SetD(0);
  drive_pid_left->SetFF(0.00025594);

  drive_pid_right->SetP(0.000014053);
  drive_pid_right->SetI(0);
  drive_pid_right->SetD(0);
  drive_pid_right->SetFF(0.00025594);
}

/**
 * Loads a given trajectory and starts the timer
 */
void Auto::LoadTrajectory(std::string name) {
  auto_timer->Reset();
  auto_timer->Start();

  // loads trajectory from deploy directory
  fs::path deployDirectory = frc::filesystem::GetDeployDirectory();
  deployDirectory = deployDirectory / "paths" / name;
  trajectory = new frc::Trajectory(
      frc::TrajectoryUtil::FromPathweaverJson(deployDirectory.string()));

  // sets robot position to start of trajectory, to deal with inconsistent
  // placement
  // navx->Reset(double(trajectory->InitialPose().Rotation().Degrees()));
  drive_encoder_left->SetPosition(0);
  drive_encoder_right->SetPosition(0);
  m_odometry->ResetPosition(trajectory->InitialPose(), navx->GetRotation2d());
}

double Auto::ConvertToRPM(units::velocity::meters_per_second_t value) {
  return double(value /3.141592 * AutoConst::kwheel_diameter_meters*AutoConst::kgear_ratio*60*30);
}

units::meter_t Auto::ConvertToMeters(double value) {
  // there are 42 tics per rotation in neo motors
  return units::meter_t(value*0.04475);
}

/**
 * Drives the robot through the loaded trajectory
 * @return true: if trajectory is complete
 */
bool Auto::FollowTrajectory(bool is_inverted) {
  // updates postionon and returns current pose
  pose = m_odometry->Update(
      navx->GetRotation2d(), ConvertToMeters(drive_encoder_left->GetPosition()),
      ConvertToMeters(drive_encoder_right->GetPosition()));
  // calculates optimal pose
  goal = trajectory->Sample(auto_timer->Get());

  // calculates wheel speeds for each side of drivebase
  adjustedSpeeds = auto_controller->Calculate(pose, goal);
  auto [left, right] = kinematics->ToWheelSpeeds(adjustedSpeeds);
  // if(is_inverted){left*=-1; right*=-1;}

  // sets wheels speeds
  drive_pid_left->SetReference(ConvertToRPM(left), rev::ControlType::kVelocity);
  drive_pid_right->SetReference(ConvertToRPM(right), rev::ControlType::kVelocity);

  std::cout<<"Time: "<<double(auto_timer->Get())<<std::endl;
  std::cout<<"Total Time: "<<double(trajectory->TotalTime())<<std::endl;
  // std::cout<<"R: "<<double(right)<<std::endl;
  // std::cout<<"L: "<<double(left)<<std::endl;
  // std::cout<<"L_enc: "<<double(drive_encoder_left->GetPosition())<<std::endl;
  std::cout<<"Navx: "<<double(navx->GetRotation2d().Radians())<<std::endl;
  std::cout<<"Navx: "<<double(goal.pose.Rotation().Radians())<<std::endl;
    std::cout<<double(trajectory->InitialPose().X())<<std::endl;

  if (auto_timer->Get() > trajectory->TotalTime()) {
    delete trajectory;
    return true;
  }
  return false;
}

void Auto::ResetEncoder(){
  drive_encoder_left->SetPosition(0);
  drive_encoder_right->SetPosition(0);
}

bool Auto::Move(double pose){
  if(drive_encoder_left->GetPosition()>pose){
    m_leftLeadMotor->Set(0);
    m_rightLeadMotor->Set(0);
    return true;
  }else{
    m_leftLeadMotor->Set(.1);
    m_rightLeadMotor->Set(.1);
  }
  return false;
}