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
<<<<<<< Updated upstream

namespace Joy1Const {
constexpr int kreject_ball_button = 5;
constexpr int kshoot_wall_trigger = 5;

constexpr int kelevator_axis = 2;
constexpr int kelevator_lock_button = 3;
}
=======
namespace Joy1Const {}
>>>>>>> Stashed changes

namespace SensorConst {
constexpr int kbreak_beam_port = 0;
constexpr int limit_switch_top_port = 1;
constexpr int limit_switch_bottom_port = 2;
constexpr int kvalue_for_ball = 2000; //value at which color sensor proximity will return there is a ball 0(far away) - 2047(close as possible)
constexpr int kultrasonic_port = 0;
} // namespace SensorConst

namespace DriveConst {
constexpr int kleft_lead_neo_number = 3;
constexpr int kright_lead_neo_number = 5;
constexpr int kleft_follow_neo_number = 2;
constexpr int kright_follow_neo_number = 6;
constexpr float kturn_div = 20; //For Ball Align
constexpr float Kp = -0.1, KpDistance = -0.1, min_command = -0.05;
} // namespace DriveConst

namespace MechanismConst {
//shooter
constexpr float kP = 1.0, kI = 0.0, kD = 0.0, kIz = 0.0, kFF = 1.0, kMaxOutput = 1.0, kMinOutput = 1.0;
constexpr int shooter_top_port = 1;
constexpr int shooter_bottom_port = 2;
constexpr int krange_target = 100;
constexpr int krange_reject = 100;
constexpr int kreject_target = 1000;
//elevator
constexpr int kelevator_motor_port = 6;
constexpr int kelevator_pnumatic_port = 0;
constexpr int kintake_reversal_amps = 10;
constexpr int climb_amperage = 10;
constexpr int elevator_slow_multiplier = 0.5;
<<<<<<< Updated upstream
//intake
constexpr int kintake_motor = 10;
constexpr int kintake_solonoid_port_left = 10;
constexpr int kintake_solonoid_port_right= 10;
//shooter
constexpr int krange_target = 100;

constexpr int khigh_range= 100;
constexpr int khigh_target = 1000;

constexpr int kreject_range= 100;
constexpr int kreject_target = 1000;

=======
//hopper
constexpr int khopper_motor_top_port = 1;
constexpr int khopper_motor_bottom_port = 2;
//compressor
constexpr float kcompressor_warning_pressure = 10;
>>>>>>> Stashed changes
} // namespace MechanismConst

namespace AutoConst {
}
