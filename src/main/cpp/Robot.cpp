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
int AutoState = 0;
// chris is so cool 
// bryan ganyu simp
// ganyu is pog
// who?tao
// hi guys its bryan welcome back to my channel today im teaching you * minecraft *
// Thank you for listening to my ted talk

void Robot::RobotInit() {
  
  // m_position_Chooser = new frc::SendableChooser<std::string>();
  // m_team_color_Chooser = new frc::SendableChooser<std::string>;
  // std::string team_color[2] = {"Red","Blue"};
  // SmartDashboard::PutStringArray("Team Color", team_color);
  // m_position_Chooser.SetDefaultOption()
  m_auto_Chooser.SetDefaultOption("Ganyu Wall","Wall");
  m_auto_Chooser.AddOption("Ganyu Wall2","Wall2Ball");
  m_auto_Chooser.AddOption("Ganyu Side2","Side2Ball");
  m_auto_Chooser.AddOption("Ganyu Side","Side");
  m_auto_Chooser.AddOption("Ganyu Taxi","Taxi");
  m_auto_Chooser.AddOption("Ganyu Wait Taxi","Wait Taxi");
  m_auto_Chooser.AddOption("Ganyu Sleep","Nothing");
  frc::Shuffleboard::GetTab("Pre").Add("Auto Chooser", m_auto_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);
  m_team_color_Chooser.SetDefaultOption("Blue","Blue");
  m_team_color_Chooser.AddOption("Red","Red");
  frc::Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).WithWidget(frc::BuiltInWidgets::kComboBoxChooser);

  // frc::SmartDashboard::PutData("Team Color Chooser", &m_team_color_Chooser);
  // SmartDashboard::UpdateValues();

  frc::CameraServer::StartAutomaticCapture();
  cs::CvSink cvSink = frc::CameraServer::GetVideo();
  cs::CvSource outputStream = frc::CameraServer::PutVideo("Driver Cam", 640, 480);
}
void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() {
  Build();
  AutoState = 0;
    // //Gets the values from the Shuffleboard
    // std::string auto_selection = Shuffleboard::GetTab("Pre").Add("Robot Position", m_auto_Chooser).GetEntry().GetString("NA");
    // //The team color it defaults to Red jic you forget to set color (aka hope to win 50-50)
    // ball_manager->team_color = Shuffleboard::GetTab("Pre").Add("Team Color", m_team_color_Chooser).GetEntry().GetString("Uh");
  m_leftLeadMotor_encoder->SetPosition(0);
  m_rightLeadMotor_encoder->SetPosition(0);
  ganyu_auto_selection = m_auto_Chooser.GetSelected();
  ball_manager->team_color = m_team_color_Chooser.GetSelected();
}
void Robot::AutonomousPeriodic() {
  if(ganyu_auto_selection == "Wall"){
    ball_manager->CheckHopperState();
    if ((AutoState == 0) && ball_manager->Rev(MechanismConst:: khigh_target_top,MechanismConst:: khigh_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 1){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 26 && m_leftLeadMotor_encoder->GetPosition() >= 26 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    }
  }else if(ganyu_auto_selection == "Side"){
    ball_manager->CheckHopperState();
    if ((AutoState == 0) && ball_manager->Rev(MechanismConst::kside_target_top,MechanismConst::kside_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 1){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 10 && m_leftLeadMotor_encoder->GetPosition() >= 10 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    } 
  }else if(ganyu_auto_selection == "Wall2Ball"){
    ball_manager->LoadHopper();
    intake->PistonDown();
    ball_manager->CheckHopperState();
    if ((AutoState == 0) && ball_manager->Rev(MechanismConst:: khigh_target_top,MechanismConst:: khigh_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 1){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 22 && m_leftLeadMotor_encoder->GetPosition() >= 22 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      intake->RunIntake(1);
      AutoState++;
    } 
    if(AutoState==2 && !ball_manager -> IsEmpty()){
      intake->RunIntake(0);
      AutoState++;
    } if (AutoState == 3){
      m_rightLeadMotor->Set(-0.3);
      m_leftLeadMotor->Set(-0.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() <= 5 && m_leftLeadMotor_encoder->GetPosition() <= 5 && AutoState == 3){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      intake->RunIntake(1);
      AutoState++;
    } 
    if (AutoState == 4){
      intake->StopIntake();
      AutoState++;
    }
    if(AutoState == 5 && ball_manager->Rev(MechanismConst:: khigh_target_top,MechanismConst:: khigh_target_bottom)){
      ball_manager -> Shoot();
    }
    if (m_timer_auto->GetMatchTime()<5_s){     
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
    }
  }else if(ganyu_auto_selection == "Side2Ball"){
    ball_manager->LoadHopper();
    intake->PistonDown();
    ball_manager->CheckHopperState();
    if ((AutoState == 0) && ball_manager->Rev(MechanismConst:: kside_target_top,MechanismConst:: kside_target_bottom)){
      ball_manager -> Shoot();
    }
    if (ball_manager -> IsEmpty() && AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 1){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 10 && m_leftLeadMotor_encoder->GetPosition() >= 10 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      intake->RunIntake(1);
      AutoState++;
    } 
    if(AutoState==2 && !ball_manager -> IsEmpty()){
      intake->RunIntake(0);
      AutoState++;
    } if (AutoState == 3){
      m_rightLeadMotor->Set(-0.3);
      m_leftLeadMotor->Set(-0.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() <= 3 && m_leftLeadMotor_encoder->GetPosition() <= 3 && AutoState == 3){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      intake->RunIntake(1);
      AutoState++;
    } 
    if (AutoState == 4){
      intake->StopIntake();
      AutoState++;
    }
    if(AutoState == 5 && ball_manager->Rev(MechanismConst:: khigh_target_top,MechanismConst:: khigh_target_bottom)){
      ball_manager -> Shoot();
    }
    if (m_timer_auto->GetMatchTime()<5_s){     
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
    }
  }
  if(ganyu_auto_selection == "Taxi"){
    ball_manager->CheckHopperState();
    if ( AutoState == 0){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 1){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 10 && m_leftLeadMotor_encoder->GetPosition() >= 10 && AutoState == 1){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    } 
  }
  if(ganyu_auto_selection == "Wait Taxi"){
    ball_manager->CheckHopperState();
    if (m_timer_auto->GetMatchTime()<5_s && AutoState == 0){
      AutoState++;
    }
    if ( AutoState == 1){
      shooter->ShootPercentOutput(0,0);
      hopper->RunHopperMotor(0,0);
      AutoState++;
    } 
    if (AutoState == 2){
      m_rightLeadMotor->Set(.3);
      m_leftLeadMotor->Set(.3);
    } 
    if (m_rightLeadMotor_encoder->GetPosition() >= 10 && m_leftLeadMotor_encoder->GetPosition() >= 10 && AutoState == 2){
      m_rightLeadMotor->Set(0);
      m_leftLeadMotor->Set(0);
      AutoState++;
    } 
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
  DisplayShuffle();
  ball_manager->CheckHopperState();
  drive ->Drive();
  //compressor
  if (compressor_toggle.GetToggleNoDebounce(joystick_1->GetRawButton(Joy1Const::kcompressor_toggle_button))){
    compressor->EnableDigital();
    frc::SmartDashboard::PutBoolean("Compressor", true);
  }else{
    compressor->Disable();
    frc::SmartDashboard::PutBoolean("Compressor", false);
  }
  // camera_result = camera.GetLatestResult();
  // limelight_result = limecamera.GetLatestResult();

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
            intake->RunIntake(1);
            hopper_init = true;
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
  if(m_timer_elevator->Get()<60_s){
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
