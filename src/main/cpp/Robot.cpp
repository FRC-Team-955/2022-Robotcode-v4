#include "Robot.h"
#include "iostream"

//Not our classes?
#include <frc/smartdashboard/SendableChooser.h>
#include <cameraserver/CameraServer.h>
#include <frc/Joystick.h>
#include <frc/Timer.h>
#include <frc/MotorSafety.h>
#include <frc/motorcontrol/Spark.h>
//del maybe
#include <frc/Compressor.h>

//our classes
#include "xy_align.h"
#include "drivebase.h"
#include "intake.h"
#include "hopper.h"
#include "shooter.h"
#include "colorsensor.h"
#include "ballmanager.h"
#include "elevator.h"

#include "settings.h"

using namespace rev;

XYalign *xyalign;
DriveBase *drive;
Intake *intake;
Hopper *hopper;
Shooter *shooter;
ColorSensor *color_sensor_top;
ColorSensor *color_sensor_bot;
BallManager *ball_manager;
Elevator *elevator;
Spark *rgb_spark;

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
ColorSensorV3 *rev_color_sensor_top;
ColorSensorV3 *rev_color_sensor_bot;
ColorMatch *color_match;
//IR BreakBeam
DigitalInput *ir_break_beam;
// Compressor compressor{13, frc::PneumaticsModuleType::REVPH};
Compressor *compressor;
//Timers
frc::Timer *m_timer_intake;
frc::Timer *m_timer_elevator;

photonlib::PhotonCamera camera{"BallDetect"};
photonlib::PhotonCamera limecamera{"gloworm"};
photonlib::PhotonPipelineResult camera_result;
photonlib::PhotonPipelineResult limelight_result;

ButtonToggle intake_deploy_toggle;
ButtonToggle hopper_manual_toggle;
ButtonToggle shooter_goal_toggle;
ButtonToggle elevator_lock_toggle;

SparkMaxRelativeEncoder *m_rightLeadMotor_encoder;
SparkMaxRelativeEncoder *m_leftLeadMotor_encoder;

frc::SendableChooser<std::string> m_position_Chooser;
frc::SendableChooser<std::string> m_team_color_Chooser;

// chris is so cool 
// bryan ganyu simp
// ganyu is pog
// who?tao
// hi guys its bryan welcome back to my channel today im teaching you * minecraft *
// Thank you for listening to my ted talk

void Robot::RobotInit() {
  // frc::CameraServer::StartAutomaticCapture();
  // cs::CvSink cvSink = frc::CameraServer::GetVideo();
  // cs::CvSource outputStream = frc::CameraServer::PutVideo("Driver Cam", 640, 480);

  // frc::ShuffleboardTab telop = frc::Shuffleboard::GetTab("Auto");
  // frc::Shuffleboard::GetTab("Telop");
  // frc::Shuffleboard::GetTab("End Game");

  //Add the options to the Choosers
  m_position_Chooser.AddOption("Left","Left");
  m_position_Chooser.AddOption("Right","Right");
  m_position_Chooser.AddOption("Middle","Middle");
  Shuffleboard::GetTab("Pre").Add("Robot Position", m_position_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
  m_team_color_Chooser.AddOption("Red", "Blue");
  m_team_color_Chooser.AddOption("Red", "Red");
  Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
  Shuffleboard::SelectTab("Pre");
}
void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() {
    // //Gets the values from the Shuffleboard
    // std::string auto_selection = Shuffleboard::GetTab("Pre").Add("Robot Position", "NA").GetEntry().GetString("NA");
    // //The team color it defaults to Red jic you forget to set color (aka hope to win 50-50)
    // ball_manager.team_color = Shuffleboard::GetTab("Pre").Add("Robot Position", "NA").GetEntry().GetString("Red");

    m_rightLeadMotor_encoder = new rev::SparkMaxRelativeEncoder(m_rightLeadMotor->GetEncoder());
    m_leftLeadMotor_encoder = new rev::SparkMaxRelativeEncoder(m_leftLeadMotor->GetEncoder());
}
void Robot::AutonomousPeriodic() {
    if (ball_manager->Rev(2000, 2000) == true){
    ball_manager -> Shoot();

  if (ball_manager -> IsEmpty() == true){
    m_rightLeadMotor->Set(.5);
    m_leftLeadMotor->Set(.5);
  }

  if (m_rightLeadMotor_encoder->GetPosition() >= 5000 && m_leftLeadMotor_encoder->GetPosition() >= 5000){
    m_rightLeadMotor->Set(0);
    m_leftLeadMotor->Set(0);
    }
  }
}

void Robot::TeleopInit() {
  // //joysticks
  joystick_0 = new frc::Joystick(0);
  joystick_1 = new frc::Joystick(1);
  // //drivebase
  m_leftLeadMotor = new CANSparkMax(DriveConst::kleft_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightLeadMotor = new CANSparkMax(DriveConst::kright_lead_neo_number, CANSparkMax::MotorType::kBrushless);
  m_leftFollowMotor = new CANSparkMax(DriveConst::kleft_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  m_rightFollowMotor = new CANSparkMax(DriveConst::kright_follow_neo_number, CANSparkMax::MotorType::kBrushless);
  differential_drive = new frc::DifferentialDrive(*m_leftLeadMotor,*m_rightLeadMotor);
  // differential_drive->SetSafetyEnabled(false);
  drive = new DriveBase(m_leftLeadMotor,m_rightLeadMotor,m_leftFollowMotor,m_rightFollowMotor,differential_drive,reverse_drive_toggle, joystick_0);
  // xyalign = new XYalign(drive, joystick_0);
  // //Intake
  intake_talon = new TalonSRX(MechanismConst::kintake_motor);
  intake_double_solonoid = new DoubleSolenoid(13,PneumaticsModuleType::REVPH, MechanismConst::kintake_double_solonoid_port_forward, MechanismConst::kintake_double_solonoid_port_reverse);
  intake = new Intake(intake_talon,intake_double_solonoid);
  // //Hopper
  talon_hopper_top = new TalonSRX(MechanismConst::khopper_motor_top_port);
  talon_hopper_bottom = new TalonSRX(MechanismConst::khopper_motor_bottom_port);
  hopper = new Hopper(talon_hopper_top,talon_hopper_bottom);
  // //shooter
  shooterneo_top = new CANSparkMax(MechanismConst::shooter_top_port, CANSparkMax::MotorType::kBrushless);
  shooterneo_bottom = new CANSparkMax(MechanismConst::shooter_bottom_port, CANSparkMax::MotorType::kBrushless);
  shooter = new Shooter(shooterneo_top, shooterneo_bottom); 
  // //Color Sensor
  rev_color_sensor_bot = new ColorSensorV3(frc::I2C::Port::kOnboard);
  rev_color_sensor_top = new ColorSensorV3(frc::I2C::Port::kMXP);
  color_match = new ColorMatch();
  color_sensor_bot= new ColorSensor(rev_color_sensor_bot,color_match);
  color_sensor_top= new ColorSensor(rev_color_sensor_top,color_match);
  // //Ir Break Beam
  ir_break_beam = new DigitalInput(SensorConst::kir_break_beam_port);
  // //BallManager
  ball_manager = new BallManager(intake,hopper,shooter, color_sensor_bot, color_sensor_top);
  // //elevator
  elevator_motor = new TalonFX(MechanismConst::kelevator_motor_port);
  limit_switch_top = new DigitalInput(SensorConst::limit_switch_top_port);
  limit_switch_bottom = new DigitalInput(SensorConst::limit_switch_bottom_port);
  elevator_solenoid_lock = new DoubleSolenoid(13, PneumaticsModuleType::REVPH, MechanismConst::kelevator_pnumatic_port_forward, MechanismConst::kelevator_pnumatic_port_reverse);
  elevator = new Elevator(elevator_motor,limit_switch_top,limit_switch_bottom,elevator_solenoid_lock);
  // //compressor
  compressor = new Compressor(13,frc::PneumaticsModuleType::REVPH);
  //rgb
  rgb_spark = new Spark(0);
  //timer
  m_timer_intake = new frc::Timer();
  m_timer_elevator = new frc::Timer();
  m_timer_intake->Start();
  m_timer_elevator->Start();
}
void Robot::TeleopPeriodic() {
  // frc::Shuffleboard::GetTab("Telop").Add("Top Color", "None").GetEntry().SetString(color_sensor_top->ClosestColor());
  // frc::Shuffleboard::GetTab("Telop").Add("Bottom Color", "None").GetEntry().SetString(color_sensor_bot->ClosestColor());
  // std::cout<<"Top: "<<color_sensor_top->ClosestColor()<<std::endl;
  // std::cout<<"Bottom: "<<color_sensor_bot->ClosestColor()<<std::endl;
  // camera_result = camera.GetLatestResult();
  // limelight_result = limecamera.GetLatestResult();
  // rgb_spark->Set(-0.99);
  drive ->Drive();
  // frc::Shuffleboard::Update();
  ball_manager->CheckHopperState();
  if(shooter_goal_toggle.GetToggleNoDebounce(joystick_0->GetRawButton(Joy0Const::kshooter_goal_toggle_button))){
    low_goal_mode = true;
  }else{
    low_goal_mode = false;
  }
  if(joystick_0->GetRawAxis(Joy0Const::kshoot_wall_trigger)>0.3){
    //shooting
    if(low_goal_mode){
      //low goal
      // frc::Shuffleboard::GetTab("Telop").Add("Intake Deploy Down", "Low Goal").GetEntry().SetString("Low Goal");
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
    // //hopper in manual or auto will add the run loadhopper automatically later
    // if(hopper_manual_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::khopper_manual_toggle_button))){
    //   hopper->RunHopperMotor(joystick_1->GetRawAxis(Joy1Const::khopper_top_axis), joystick_1->GetRawAxis(Joy1Const::khopper_bottom_axis));
    // }else{
    //   ball_manager->LoadHopper();
    // }
    
    if(joystick_1->GetRawButton(Joy1Const::kreject_ball_button)){
      ball_manager->Reject();
    }else{
      //if not rejecting
      if(intake_deploy_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kintake_toggle_button))){
          // frc::Shuffleboard::GetTab("Telop").Add("Intake Deploy Down", true).WithWidget(frc::BuiltInWidgets::kBooleanBox).GetEntry().SetBoolean(true);
          intake->PistonDown();
          //If the intake is in the down state allow the intake to run
          if(joystick_1->GetRawAxis(Joy1Const::kintake_motor_run_axis)>0.3){
            intake->RunIntake(1);
            // m_timer_intake->Start();
            m_timer_intake->Reset();
            ball_manager->CheckHopperState();
          }else{
            intake->RunIntake(0);
          }
      }else{
        // frc::Shuffleboard::GetTab("Telop").Add("Intake Deploy Down", false).WithWidget(frc::BuiltInWidgets::kBooleanBox).GetEntry().SetBoolean(false);
        intake->PistonUp();
      }
      //if the m_intake_timer is less than 5s then run the hopper
      std::cout<<(int)m_timer_elevator->Get()<<std::endl;
      if(!m_timer_intake->HasElapsed(3_s)){
        std::cout<<"loadhopper"<<std::endl;
        ball_manager->LoadHopper();
      }else{
        hopper->RunHopperMotor(0,0);
      }
    }
  }
  //check if its around time to climb //GetMatchTime()
  if(m_timer_elevator->Get()>5_s){
    if(elevator_lock_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kelevator_lock_button))){
      elevator->LockElevator();
    }else{
      elevator->UnlockElevator();
    }
  elevator->ElevatorMove(joystick_1->GetRawAxis(Joy1Const::kelevator_axis));
  }  
}
void Robot::DisplayShuffle() {
  drive->DisplayDriveInfo();
  intake->DisplayIntakeInfo();
  hopper->DiplayHopperInfo();
  shooter->DisplayShooterInfo();
  ball_manager->DisplayBallManagerInfo();
  elevator->DisplayElevatorInfo();
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
  delete xyalign;
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
  //timer
  delete m_timer_intake;
  delete m_timer_elevator;
}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
