#include <auto.h>
/**
 * A 2nd constructor that must be run in auto init
 */
void Auto::Initilize(rev::CANSparkMax *left_spark,
                     rev::CANSparkMax *right_spark) {
  auto_controller = new frc::RamseteController();
  auto_timer = new frc::Timer();
  m_odometry = new frc::DifferentialDriveOdometry(navx->GetRotation2d());
  kinematics = new frc::DifferentialDriveKinematics(AutoConst::ktrack_width);
  drive_pid_left =
      new rev::SparkMaxPIDController(left_spark->GetPIDController());
  drive_pid_right =
      new rev::SparkMaxPIDController(right_spark->GetPIDController());
  drive_encoder_left =
      new rev::SparkMaxRelativeEncoder(left_spark->GetEncoder());
  drive_encoder_right =
      new rev::SparkMaxRelativeEncoder(right_spark->GetEncoder());

  drive_pid_left->SetP(0);
  drive_pid_left->SetI(0);
  drive_pid_left->SetD(0);
  drive_pid_left->SetFF(0);

  drive_pid_right->SetP(0);
  drive_pid_right->SetI(0);
  drive_pid_right->SetD(0);
  drive_pid_right->SetFF(0);
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
  m_odometry->ResetPosition(trajectory->InitialPose(), navx->GetRotation2d());
}

double Auto::ConvertToRPM(units::velocity::meters_per_second_t value) {
  return value * ((60 * AutoConst::kgear_ratio) /
                  (3.141592 * AutoConst::kwheel_diameter_meters));
}

units::meter_t Auto::ConvertToMeters(double value) {
  // there are 42 tics per rotation in neo motors
  return value * ((42 * 3.141592 * AutoConst::kwheel_diameter_meters) /
                  AutoConst::kgear_ratio);
}

/**
 * Drives the robot through the loaded trajectory
 * @return true: if trajectory is complete
 */
bool Auto::FollowTrajectory() {
  // updates postionon and returns current pose
  pose = m_odometry->Update(
      navx->GetRotation2d(), ConvertToMeters(drive_encoder_left->GetPosition()),
      ConvertToMeters(drive_encoder_right->GetPosition()));
  // calculates optimal pose
  goal = trajectory->Sample(auto_timer->Get());

  // calculates wheel speeds for each side of drivebase
  adjustedSpeeds = auto_controller->Calculate(pose, goal);
  auto [left, right] = kinematics->ToWheelSpeeds(adjustedSpeeds);

  // sets wheels speeds
  drive_pid_left->SetReference(ConvertToRPM(left), rev::ControlType::kVelocity);
  drive_pid_right->SetReference(ConvertToRPM(left),
                                rev::ControlType::kVelocity);

  if (auto_timer->Get() > trajectory->TotalTime()) {
    delete trajectory;
    return true;
  }
  return false;
}