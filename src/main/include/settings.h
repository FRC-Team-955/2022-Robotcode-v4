#pragma once
#include <iostream>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/angle.h>
#include <units/acceleration.h>
#include <units/velocity.h>
#include <units/voltage.h>
#include <units/velocity.h>
#include <units/voltage.h>

namespace Joy0Const {
constexpr int kdrive_speed_axis = 1;
constexpr int kdrive_curvature_axis = 4;
//shooter
constexpr int kshoot_wall_trigger = 3;
constexpr int kshoot_limelight_trigger = 2;
constexpr int kshoot_launchpad_button = 6;
constexpr int kshooter_goal_toggle_button = 7;
constexpr int kpid_only_toggle_button = 8;
// constexpr int kball_aimbot_button = 5;
} // namespace Joy0Const

namespace Joy1Const {
//intake
constexpr int kintake_toggle_button = 5;
constexpr int kintake_motor_run_axis = 2;
//hopper
constexpr int khopper_manual_button = 1;
constexpr int khopper_manual_axis = 5;
//shooter
constexpr int kreject_ball_button = 6;
//elevator
constexpr int kelevator_lock_button = 8;
constexpr int kelevator_allow = 3;
constexpr int kelevator_axis = 1;
//compressor
constexpr int kcompressor_toggle_button = 7;
}
namespace SensorConst {
constexpr int limit_switch_top_port = 4; //g
constexpr int limit_switch_bottom_port = 1; //g
//1000
constexpr unsigned int kvalue_for_ball_top = 1700; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
//190
constexpr unsigned int kvalue_for_ball_bottom = 200; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
constexpr int kir_break_beam_port = 2;
} // namespace SensorConst

namespace DriveConst {
constexpr int kleft_lead_neo_number = 6; //g
constexpr int kleft_follow_neo_number = 5; //g
constexpr int kright_lead_neo_number = 2; //g
constexpr int kright_follow_neo_number = 3; //g
constexpr float Kp = -0.1, KpDistance = -0.1, min_command = -0.05; //For Limelight Align
} // namespace DriveConst

namespace MechanismConst {
//intake
constexpr int kintake_motor = 10; //g
constexpr int kintake_double_solonoid_port_forward = 1;
constexpr int kintake_double_solonoid_port_reverse = 2; 
//hopper
constexpr int khopper_motor_top_port = 119; //this has been change ori 9
constexpr int khopper_motor_bottom_port = 11; //g
//shooter
constexpr double top_kP = 0.00008, top_kI = 0, top_kD = 0, top_kFF =0.000187, top_kMinOutput=0,top_kMaxOutput = 1;
constexpr double bottom_kP = 0.00008, bottom_kI = 0, bottom_kD = 0, bottom_kFF = 0.000187, bottom_kMinOutput=-1, bottom_kMaxOutput = 0;
constexpr double kMinOutput = -1, kMaxOutput = 1;
constexpr int shooter_top_port = 8; //g
constexpr int shooter_bottom_port = 7; //g
constexpr int kshooter_pnumatic_port_forward = 3;
constexpr int kshooter_pnumatic_port_reverse = 4;
//range
constexpr int ktarget_range = 100; //g?
constexpr int ktarget_switch_control_mode = 250; //g?
//low goal
constexpr int ktarget_low_bottom = 1300; //g 
constexpr int ktarget_low_top = 2000; //g
//high goal
constexpr int ktarget_high_top = 2300; //2300
constexpr int ktarget_high_bottom = 2000; //2000
 //g// constexpr int ktarget_high_bottom = 2300; //g
// constexpr int ktarget_high_top = 3300; //g
//Min bottom speed
constexpr double ktarget_limelight_bottom_min = 1700;
//launch pad goal
//2925
constexpr int ktarget_launch_top= 3550;//for range shots
constexpr int ktarget_launch_bottom = 3300; //for range shot
//side goal
constexpr int ktarget_side_bottom = 2200; //g
constexpr int ktarget_side_top = 2600; //g
//elevator
constexpr int kelevator_motor_port = 12; //g
constexpr int kelevator_pnumatic_port_forward = 6;
constexpr int kelevator_pnumatic_port_reverse = 5;
} // namespace MechanismConst

namespace AutoConst {
    constexpr units::meter_t ktrack_width = 0.56_m;
    constexpr units::meter_t kwheel_diameter_meters = 0.15_m;
    constexpr double kgear_ratio = 10.75;

    constexpr units::meter_t camera_height = 36.8_in;
    constexpr units::meter_t target_height = 104_in;
    constexpr units::radian_t camera_pitch = 49_deg;
    constexpr double shooter_m_top = 31.9;
    constexpr double shooter_b_top = 2027;
    constexpr double shooter_m_bottom = -16.1;
    constexpr double shooter_b_bottom = 1977;

    constexpr auto kS = 0.10023_V;
    constexpr auto kV = 1.2794_V * 1_s/1_rad;
    constexpr auto kA = 1.2494_V*1_s*1_s/1_rad;
}