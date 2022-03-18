#ifndef AUTO
#define AUTO

#include "Navx.h"
#include "rev/CANSparkMax.h"
#include <frc/Filesystem.h>
#include <frc/controller/RamseteController.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/trajectory/TrajectoryUtil.h>
#include <wpi/fs.h>

#include "settings.h"

class Auto {
public:
  Auto() { navx = new Navx(); };
  ~Auto() {
    delete navx;
    delete trajectory;
    delete auto_controller;
    delete auto_timer;
    delete m_odometry;
    delete kinematics;
    delete drive_pid_left;
    delete drive_pid_right;
    delete drive_encoder_left;
    delete drive_encoder_right;
  }
  void Initilize(rev::CANSparkMax *left_spark, rev::CANSparkMax *right_spark, rev::SparkMaxRelativeEncoder *m_leftLeadMotor_encoder, rev::SparkMaxRelativeEncoder *m_rightLeadMotor_encoder);
  void LoadTrajectory(std::string name);
  bool FollowTrajectory();
  double ConvertToRPM(units::velocity::meters_per_second_t value);
  bool Move(double pose);
  void ResetEncoder();
  units::meter_t ConvertToMeters(double value);

private:
  Navx *navx;
  frc::Trajectory *trajectory;
  frc::RamseteController *auto_controller;
  frc::Timer *auto_timer;
  frc::Trajectory::State goal;
  frc::ChassisSpeeds adjustedSpeeds;
  frc::DifferentialDriveOdometry *m_odometry;
  frc::Pose2d pose;
  frc::DifferentialDriveKinematics *kinematics;
  rev::SparkMaxPIDController *drive_pid_left;
  rev::SparkMaxRelativeEncoder *drive_encoder_left;
  rev::SparkMaxPIDController *drive_pid_right;
  rev::SparkMaxRelativeEncoder *drive_encoder_right;
  rev::CANSparkMax *m_leftLeadMotor;
  rev::CANSparkMax *m_rightLeadMotor;
};

#endif