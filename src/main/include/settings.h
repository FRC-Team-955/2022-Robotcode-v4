#pragma once
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <units/acceleration.h>
#include <units/velocity.h>
#include <units/angle.h>
#include <units/length.h>

namespace Joy0Const {
constexpr int kdrive_speed_axis = 1;
constexpr int kdrive_curvature_axis = 4;
constexpr int kreverse_drive = 6;
//shooter
constexpr int kshoot_wall_trigger = 3;
constexpr int kshooter_goal_toggle_button = 7;
constexpr int khorizontal_align_button = 4;
constexpr int kshoot_distance_trigger = 2;
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
constexpr unsigned int kvalue_for_ball_top = 1700; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
constexpr unsigned int kvalue_for_ball_bottom = 200; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
constexpr int kir_break_beam_port = 2;
} // namespace SensorConst

namespace DriveConst {
constexpr int kleft_lead_neo_number = 6; //4; //g
constexpr int kleft_follow_neo_number = 5; //g
constexpr int kright_lead_neo_number = 2; //g
constexpr int kright_follow_neo_number = 3; //g
constexpr float kturn_div = 20; //For Ball Align
constexpr float Kp = -0.1, KpDistance = -0.1, min_command = -0.05;//For Limelight Align
constexpr float krate_limit = 3/5;
} // namespace DriveConst

namespace MechanismConst {
//intake
constexpr int kintake_motor = 10; //g
constexpr int kintake_double_solonoid_port_forward = 7;
constexpr int kintake_double_solonoid_port_reverse = 6; 
constexpr int kintake_reversal_amps = 50;
//hopper
constexpr int khopper_motor_top_port = 9; //g
constexpr int khopper_motor_bottom_port = 11; //g
//shooter
constexpr double top_kP = 0.00008, top_kI = 0, top_kD = 0.015035, top_kFF = 0.000186, top_kMinOutput=0,top_kMaxOutput = 1;
constexpr double bottom_kP = 0.00009, bottom_kI = 0, bottom_kD = 0.015035, bottom_kFF = 0.000185, bottom_kMinOutput=-1, bottom_kMaxOutput = 0;
constexpr double kMinOutput = -1, kMaxOutput = 1;
constexpr int shooter_top_port = 8; //g
constexpr int shooter_bottom_port = 7; //g
constexpr int krange_target = 100; //g?
//low goal
constexpr int klow_target_bottom = 1200; //g
constexpr int klow_target_top = 1300; //g
//high goal
// constexpr int khigh_target_bottom = 1050; //g
// constexpr int khigh_target_top = 4100; //g
constexpr int khigh_target_bottom = 2100; //g
constexpr int khigh_target_top = 2000; //g
//Min bottom speed
constexpr double krange_target_min = 1100;
//side goal
constexpr int kside_target_bottom = 2200; //g
constexpr int kside_target_top = 2600; //g
//Vertical shot
constexpr int kvertical_target_bottom = 3000; //g
constexpr int kvertical_target_top = 1950; //g
//reject
constexpr int kreject_target = 1200;
//elevator
constexpr int kelevator_motor_port = 12; //g
constexpr int kelevator_pnumatic_port_forward = 4;
constexpr int kelevator_pnumatic_port_reverse = 5;
constexpr int climb_amperage = 20;
constexpr int elevator_slow_multiplier = 0.5;
//compressor
constexpr float kcompressor_warning_pressure = 10;
} // namespace MechanismConst

namespace AutoConst {
    constexpr units::meter_t ktrack_width = 0.5842_m;
    constexpr units::meter_t kwheel_diameter_meters = 0.1524_m;
    constexpr double kgear_ratio = 10.75;

    constexpr units::meter_t camera_height = 38_in;
    constexpr units::meter_t target_height = 104_in;
    constexpr units::radian_t camera_pitch = 36_deg;
    constexpr double shooter_m_top = 31.9;
    constexpr double shooter_b_top = 2027;
    constexpr double shooter_m_bottom = -16.1;
    constexpr double shooter_b_bottom = 1977;
}
