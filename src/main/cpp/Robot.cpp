#include "Robot.h"
#include "iostream"

//Not our classes?
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <cameraserver/CameraServer.h>
#include <frc/Joystick.h>
#include <frc/Timer.h>
#include <frc/MotorSafety.h>
#include <frc/motorcontrol/Spark.h>
#include <frc/Compressor.h>

#include <frc/Filesystem.h>
#include <frc/trajectory/TrajectoryUtil.h>
#include <wpi/fs.h>
#include <frc/controller/RamseteController.h>
#include <frc/kinematics/DifferentialDriveOdometry.h>
#include <frc/kinematics/DifferentialDriveKinematics.h>
#include <AHRS.h>

//our classes
#include "drivebase.h"
#include "intake.h"
#include "hopper.h"
#include "shooter.h"
#include "colorsensor.h"
#include "ballmanager.h"
#include "elevator.h"

#include "settings.h"

using namespace rev;

DriveBase *drive;
Intake *intake;
Hopper *hopper;
Shooter *shooter;
ColorSensor *color_sensor_top;
ColorSensor *color_sensor_bot;
BallManager *ball_manager;
Elevator *elevator;

//Joysticks
frc::Joystick *joystick_0;
frc::Joystick *joystick_1;
//Drive
CANSparkMax *m_leftLeadMotor;
CANSparkMax *m_rightLeadMotor;
CANSparkMax *m_leftFollowMotor;
CANSparkMax *m_rightFollowMotor;
frc::DifferentialDrive *differential_drive;
ButtonToggle *reverse_drive_toggle;
//Intake
TalonSRX *intake_talon;
DoubleSolenoid *intake_double_solonoid;
//Hopper
TalonSRX *talon_hopper_top;
TalonSRX *talon_hopper_bottom;
bool hopper_init = false;
//Shooter
CANSparkMax *shooterneo_top;
CANSparkMax *shooterneo_bottom;
bool low_goal_mode = false;
//Elevator
TalonFX *elevator_motor;
DigitalInput *limit_switch_top;
DigitalInput *limit_switch_bottom;
DoubleSolenoid *elevator_solenoid_lock;
//Color Sensor
frc::Color *red_target_bot;
frc::Color *blue_target_bot;
frc::Color *red_target_top;
frc::Color *blue_target_top;
ColorSensorV3 *rev_color_sensor_top;
ColorSensorV3 *rev_color_sensor_bot;
ColorMatch *color_match;
//IR BreakBeam
DigitalInput *ir_break_beam;
//Compressor
Compressor *compressor;
//Timers
frc::Timer *m_timer_auto;
frc::Timer *m_timer_intake;
frc::Timer *m_timer_elevator;
//RGB
// Spark *rgb_spark;

ButtonToggle intake_deploy_toggle;
ButtonToggle hopper_manual_toggle;
ButtonToggle shooter_goal_toggle;
ButtonToggle elevator_lock_toggle;
ButtonToggle compressor_toggle;


SparkMaxRelativeEncoder *m_rightLeadMotor_encoder;
SparkMaxRelativeEncoder *m_leftLeadMotor_encoder;

frc::SendableChooser<std::string> m_auto_Chooser;
frc::SendableChooser<std::string> m_team_color_Chooser;
std::string ganyu_auto_selection = "Sleep";
// chris is so cool 
// bryan ganyu simp
// ganyu is pog
// who?tao
// hi guys its bryan welcome back to my channel today im teaching you * minecraft *
// Thank you for listening to my ted talk

//auto
frc::Trajectory *trajectory;
frc::RamseteController *auto_controller;
frc::Timer *auto_timer;
Trajectory::State goal;
ChassisSpeeds adjustedSpeeds;
frc::DifferentialDriveOdometry *m_odometry;
frc::Pose2d pose;
frc::DifferentialDriveKinematics *kinematics;
rev::SparkMaxPIDController *drive_pid_left;
rev::SparkMaxRelativeEncoder *drive_encoder_left;
rev::SparkMaxPIDController *drive_pid_right;
rev::SparkMaxRelativeEncoder *drive_encoder_right;
AHRS *navx;
int auto_state = 0;

units::meter_t track_width = units::meter_t(0.6);

void Robot::RobotInit() {
  m_team_color_Chooser.SetDefaultOption("Blue","Blue");
  m_team_color_Chooser.AddOption("Red","Red");
  frc::Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);

  frc::CameraServer::StartAutomaticCapture();
  cs::CvSink cvSink = frc::CameraServer::GetVideo();
  cs::CvSource outputStream = frc::CameraServer::PutVideo("Driver Cam", 640, 480);

  //auto
  navx = new AHRS(SPI::Port::kMXP);
}
void Robot::RobotPeriodic() {}
void Robot::AutonomousInit() {
  Build();
  auto_controller = new RamseteController();
  auto_timer = new Timer();
  frc::Rotation2d gyroAngle{units::degree_t(-navx->GetAngle()*(3.141592/180.0))};
  m_odometry = new DifferentialDriveOdometry(gyroAngle,Pose2d{9.871497057194068_m, 5.634024121231873_m, 0.7123575980943289_rad}); //need to update
  kinematics = new DifferentialDriveKinematics(track_width);
  drive_pid_left = new SparkMaxPIDController(m_leftFollowMotor->GetPIDController());
  drive_pid_right = new SparkMaxPIDController(m_rightFollowMotor->GetPIDController());
  drive_encoder_left = new SparkMaxRelativeEncoder(m_leftFollowMotor->GetEncoder());
  drive_encoder_right = new SparkMaxRelativeEncoder(m_rightFollowMotor->GetEncoder());

  drive_pid_left->SetP(0);
  drive_pid_left->SetI(0);
  drive_pid_left->SetD(0);
  drive_pid_left->SetFF(0);

  drive_pid_right->SetP(0);
  drive_pid_right->SetI(0);
  drive_pid_right->SetD(0);
  drive_pid_right->SetFF(0);
}
void Robot::AutonomousPeriodic() {
  intake->PistonDown();
  if(auto_state == 0){
    if(ball_manager->Rev(MechanismConst::kside_target_top,MechanismConst::kside_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty()){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      auto_state++;
    } 
  }
  if(auto_state == 1){
    auto_timer->Reset();
    auto_timer->Start();
    fs::path deployDirectory = frc::filesystem::GetDeployDirectory();
    deployDirectory = deployDirectory / "paths" / "Out.wpilib.json";
    trajectory = new Trajectory(frc::TrajectoryUtil::FromPathweaverJson(deployDirectory.string()));
    auto_state++;
  }
  if(auto_state == 2){
    intake->RunIntake(1);
    frc::Rotation2d gyroAngle{units::degree_t(-navx->GetAngle()*(3.141592/180.0))};
    pose = m_odometry->Update(gyroAngle, units::meter_t(drive_encoder_left->GetPosition()), units::meter_t(drive_encoder_right->GetPosition()));
    goal = trajectory->Sample(auto_timer->Get());
    adjustedSpeeds = auto_controller->Calculate(pose, goal);
    auto [left, right] = kinematics->ToWheelSpeeds(adjustedSpeeds);
    drive_pid_left->SetReference(double(left), rev::ControlType::kVelocity);
    drive_pid_right->SetReference(double(right), rev::ControlType::kVelocity);
    if(auto_timer->Get() < trajectory->TotalTime()){
      auto_state++;
    }
  }
  if(auto_state == 3){
    intake->RunIntake(1);
    auto_timer->Reset();
    auto_timer->Start();
    fs::path deployDirectory = frc::filesystem::GetDeployDirectory();
    deployDirectory = deployDirectory / "paths" / "Back.wpilib.json";
    delete trajectory;
    trajectory = new Trajectory(frc::TrajectoryUtil::FromPathweaverJson(deployDirectory.string()));
    if(!ball_manager -> IsEmpty()){
      auto_state++;
    }
  }
  if(auto_state == 3){
    frc::Rotation2d gyroAngle{units::degree_t(-(navx->GetAngle()+180)*(3.141592/180.0))};
    pose = m_odometry->Update(gyroAngle, units::meter_t(drive_encoder_left->GetPosition()), units::meter_t(drive_encoder_right->GetPosition()));
    goal = trajectory->Sample(auto_timer->Get());
    adjustedSpeeds = auto_controller->Calculate(pose, goal);
    auto [left, right] = kinematics->ToWheelSpeeds(adjustedSpeeds);
    drive_pid_left->SetReference(double(-left), rev::ControlType::kVelocity);
    drive_pid_right->SetReference(double(-right), rev::ControlType::kVelocity);
    if(auto_timer->Get() < trajectory->TotalTime()){
      auto_state++;
    }
  }
  if(auto_state == 4){
    if(ball_manager->Rev(MechanismConst::kside_target_top,MechanismConst::kside_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty()){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      auto_state++;
    } 
  }
  if(auto_state == 5){
    //turn off all motors (not implemented yet)
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
}

void Robot::TeleopInit() {
  Build();
  ball_manager->team_color = m_team_color_Chooser.GetSelected();
  m_timer_intake->Start();
  m_timer_elevator->Start();
}
void Robot::TeleopPeriodic() {
  // rgb_spark->Set(-0.57);
  ball_manager->CheckHopperState();
  DisplayShuffle();
  drive ->Drive();
  //compressor
  if (compressor_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kcompressor_toggle_button))){
    compressor->EnableDigital();
    frc::SmartDashboard::PutBoolean("Compressor", true);
  }else{
    compressor->Disable();
    frc::SmartDashboard::PutBoolean("Compressor", false);
  }

  //The toggle for Low Goal
  if(shooter_goal_toggle.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kshooter_goal_toggle_button))){
    low_goal_mode = true;
  }else{
    low_goal_mode = false;
  }

  if(joystick_0->GetRawAxis(Joy0Const::kshoot_wall_trigger)>0.3){
    //shooting
    if(low_goal_mode){
      //low goal
      if(ball_manager->Rev(MechanismConst::klow_target_top,MechanismConst::klow_target_bottom)){
        ball_manager->Shoot();
        ball_manager->CheckHopperState();
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }else{
      //high goal
      if(ball_manager->Rev(MechanismConst::khigh_target_top,MechanismConst::khigh_target_bottom)){
        ball_manager->Shoot();
        ball_manager->CheckHopperState();
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
  }else{
    //if not shooting
    shooter->VelocityControl(0,0);
    if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
      //Reject code
      intake->PistonDown();
      ball_manager->Reject();
    }else if(joystick_1->GetRawButtonReleased(Joy1Const::kreject_ball_button)){
      //When you release shut off motor
      intake->PistonUp();
      hopper->RunHopperMotor(0,0);
    }else{
      //if not rejecting
      if(intake_deploy_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
          intake->PistonDown();
          //If the intake is in the down state allow the intake to run
          if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)>0.3){
            hopper_init = true;
            intake->RunIntake(1);
            m_timer_intake->Start();
            m_timer_intake->Reset();
            ball_manager->CheckHopperState();
          }else{
            intake->RunIntake(0);
          }
      }else{
        intake->PistonUp();
      }
      //if the m_intake_timer is less than 5s then run the hopper
      //hopper in manual or auto will add the run loadhopper automatically later
      if(joystick_1->GetRawButton(Joy1Const::khopper_manual_button)){
        hopper->RunHopperMotor(-joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis), -joystick_1->GetRawAxis(Joy1Const::khopper_manual_axis));
        frc::SmartDashboard::PutBoolean("Manual Hopper", true);
      }else if(!m_timer_intake->HasElapsed(3_s)){
        frc::SmartDashboard::PutBoolean("Manual Hopper", false);
        if (hopper_init){
          ball_manager->CheckHopperState();
          ball_manager->LoadHopper();
        }
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
  }
  //check if its around time to climb //GetMatchTime()
  // if(m_timer_elevator->GetMatchTime()<30_s){
  if(joystick_1->GetRawButton(Joy1Const::kelevator_allow)){
    if(elevator_lock_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kelevator_lock_button))){
      elevator->LockElevator();
    }else{
      elevator->UnlockElevator();
    }
    elevator->ElevatorMove(joystick_1->GetRawAxis(Joy1Const::kelevator_axis));
  }else{
    elevator_motor->Set(ControlMode::PercentOutput, 0);
  }
  // }
}
void Robot::DisplayShuffle() {
  drive->DisplayDriveInfo();
  intake->DisplayIntakeInfo();
  hopper->DiplayHopperInfo();
  shooter->DisplayShooterInfo();
  ball_manager->DisplayBallManagerInfo();
  elevator->DisplayElevatorInfo();
  frc::SmartDashboard::PutBoolean("Low Goal Mode", low_goal_mode);
  frc::SmartDashboard::PutString("Team Color", ball_manager->team_color);
}
void Robot::DisabledInit() {
  //joystick
  delete joystick_0;
  delete joystick_1;
  //drive base
  delete m_leftLeadMotor;
  delete m_rightLeadMotor;
  delete m_leftFollowMotor;
  delete m_rightFollowMotor;
  delete differential_drive;
  delete reverse_drive_toggle;
  delete drive;
  //auto
  delete m_leftLeadMotor_encoder;
  delete m_rightLeadMotor_encoder;
  //intake
  delete intake_talon;
  delete intake_double_solonoid;
  delete intake;
  //hopper
  delete talon_hopper_top;
  delete talon_hopper_bottom;
  delete hopper;
  //shooter
  delete shooterneo_top;
  delete shooterneo_bottom;
  delete shooter;
  //color sensor
  delete red_target_bot;
  delete blue_target_bot;
  delete red_target_top;
  delete blue_target_top;
  delete rev_color_sensor_top;
  delete rev_color_sensor_bot;
  delete color_match;
  delete color_sensor_top;
  delete color_sensor_bot;
  //Ir break beam
  delete ir_break_beam;
  //BallManager
  delete ball_manager;
  //elevator
  delete elevator_motor;
  delete limit_switch_top;
  delete limit_switch_bottom;
  delete elevator_solenoid_lock;
  delete elevator;
  //compressor
  delete compressor; 
  //rgb
  // delete rgb_spark;
  //timer
  delete m_timer_intake;
  delete m_timer_elevator;
  //Delete();
}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}
void Robot::Build(){
  //joysticks
  joystick_0 = new frc::Joystick(0);
  joystick_1 = new frc::Joystick(1);
  //drivebase
  m_leftLeadMotor = new CANSparkMax(DriveConst::kleft_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightLeadMotor = new CANSparkMax(DriveConst::kright_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_leftFollowMotor = new CANSparkMax(DriveConst::kleft_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightFollowMotor = new CANSparkMax(DriveConst::kright_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  differential_drive = new frc::DifferentialDrive(*m_leftLeadMotor,*m_rightLeadMotor);
  // differential_drive->SetSafetyEnabled(false);
  drive = new DriveBase(m_leftLeadMotor,m_rightLeadMotor,m_leftFollowMotor,m_rightFollowMotor,differential_drive,reverse_drive_toggle, joystick_0);
  // xyalign = new XYalign(drive, joystick_0);
  //auto
  m_leftLeadMotor_encoder = new SparkMaxRelativeEncoder(m_leftLeadMotor->GetEncoder());
  m_rightLeadMotor_encoder = new SparkMaxRelativeEncoder(m_rightLeadMotor->GetEncoder());
  //Intake
  intake_talon = new TalonSRX(MechanismConst::kintake_motor);
  intake_double_solonoid = new DoubleSolenoid(13,PneumaticsModuleType::REVPH, MechanismConst::kintake_double_solonoid_port_forward, MechanismConst::kintake_double_solonoid_port_reverse);
  intake = new Intake(intake_talon,intake_double_solonoid);
  //Hopper
  talon_hopper_top = new TalonSRX(MechanismConst::khopper_motor_top_port);
  talon_hopper_bottom = new TalonSRX(MechanismConst::khopper_motor_bottom_port);
  hopper = new Hopper(talon_hopper_top,talon_hopper_bottom);
  //shooter
  shooterneo_top = new CANSparkMax(MechanismConst::shooter_top_port, CANSparkMax::MotorType::kBrushless);
  shooterneo_bottom = new CANSparkMax(MechanismConst::shooter_bottom_port, CANSparkMax::MotorType::kBrushless);
  shooter = new Shooter(shooterneo_top, shooterneo_bottom); 
  //Color Sensor
  red_target_bot = new frc::Color(0.539917, 0.339233, 0.120972);
  blue_target_bot= new frc::Color(0.158325, 0.406372, 0.435181);
  red_target_top= new frc::Color(0.666, 0.333, 0.001);
  blue_target_top= new frc::Color(0.250122, 0.500122, 0.250122);
  rev_color_sensor_bot = new ColorSensorV3(frc::I2C::Port::kOnboard);
  rev_color_sensor_top = new ColorSensorV3(frc::I2C::Port::kMXP);
  color_match = new ColorMatch();
  color_sensor_bot= new ColorSensor(rev_color_sensor_bot,color_match,red_target_bot, blue_target_bot);
  color_sensor_top= new ColorSensor(rev_color_sensor_top,color_match, red_target_top, blue_target_top);
  //Ir Break Beam
  ir_break_beam = new DigitalInput(SensorConst::kir_break_beam_port);
  //BallManager
  ball_manager = new BallManager(intake,hopper,shooter, color_sensor_bot, color_sensor_top);
  //elevator
  elevator_motor = new TalonFX(MechanismConst::kelevator_motor_port);
  limit_switch_top = new DigitalInput(SensorConst::limit_switch_top_port);
  limit_switch_bottom = new DigitalInput(SensorConst::limit_switch_bottom_port);
  elevator_solenoid_lock = new DoubleSolenoid(13, PneumaticsModuleType::REVPH, MechanismConst::kelevator_pnumatic_port_forward, MechanismConst::kelevator_pnumatic_port_reverse);
  elevator = new Elevator(elevator_motor,limit_switch_top,limit_switch_bottom,elevator_solenoid_lock);
  //compressor
  compressor = new Compressor(13,frc::PneumaticsModuleType::REVPH);
  //rgb
  // rgb_spark = new Spark(1);
  //timer
  m_timer_auto = new frc::Timer();
  m_timer_intake = new frc::Timer();
  m_timer_elevator = new frc::Timer();
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
