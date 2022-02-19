#pragma once
#include <frc/shuffleboard/Shuffleboard.h>

namespace Joy0Const {
constexpr int kquick_turn_button = 2;
constexpr int kdrive_speed_axis = 1;
constexpr int kdrive_curvature_axis = 4;
constexpr int kreverse_drive =  1;
constexpr int kball_aimbot_button = 5;
constexpr int kshoot_trigger = 5;
} // namespace Joy0Const

namespace Joy1Const {
//intake
constexpr int kintake_toggle_button = 6;
constexpr int kintake_motor_run = 6;
//shooter
constexpr int kreject_ball_button = 5;
constexpr int kshoot_wall_trigger = 5;
//elevator
constexpr int kelevator_axis = 2;
constexpr int kelevator_lock_button = 3;
}

namespace SensorConst {
constexpr int limit_switch_top_port = 1;
constexpr int limit_switch_bottom_port = 2;
constexpr int kvalue_for_ball = 2000; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
constexpr int kultrasonic_port = 0;
} // namespace SensorConst

namespace DriveConst {
constexpr int kleft_lead_neo_number = 4; 
constexpr int kright_lead_neo_number = 2;
constexpr int kleft_follow_neo_number = 5;
constexpr int kright_follow_neo_number = 3;
constexpr float kturn_div = 20; //For Ball Align
constexpr float Kp = -0.1, KpDistance = -0.1, min_command = -0.05;//For Limelight Align
} // namespace DriveConst

namespace MechanismConst {
//intake
constexpr int kintake_motor = 10; //g

constexpr int kintake_double_solonoid_port_left_forward = 7; //g
constexpr int kintake_double_solonoid_port_left_reverse = 6; //g

constexpr int kintake_double_solonoid_port_right_forward = 5; //g
constexpr int kintake_double_solonoid_port_right_reverse = 9; //g

//hopper
constexpr int khopper_motor_top_port = 9; //g
constexpr int khopper_motor_bottom_port = 11; //g
//shooter
constexpr double bottom_kP = 0.0003, bottom_kI = 0, bottom_kD = 0, bottom_kIz = 0, bottom_kFF = 0.00024, bottom_kMaxOutput = 1,
                 bottom_kMinOutput = -1;
constexpr double top_kP = 0.00026, top_kI = 0, top_kD = 0, top_kIz = 0, top_kFF = 0.00018, top_kMaxOutput = 1,
                 top_kMinOutput = -1;
constexpr int shooter_top_port = 8; //g
constexpr int shooter_bottom_port = 7; //g
constexpr int krange_target = 100;
constexpr int khigh_range= 100;
constexpr int khigh_target = 1000;
constexpr int kreject_range= 100;
constexpr int kreject_target = 1000;
//elevator
constexpr int kelevator_motor_port = 12; //g
constexpr int kelevator_pnumatic_port = 0;
constexpr int kintake_reversal_amps = 10;
constexpr int climb_amperage = 10;
constexpr int elevator_slow_multiplier = 0.5;
//compressor
constexpr float kcompressor_warning_pressure = 10;
} // namespace MechanismConst

namespace AutoConst {
}
