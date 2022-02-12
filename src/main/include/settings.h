#pragma once
#include <frc/shuffleboard/Shuffleboard.h>

namespace Joy0Const {
constexpr int kquick_turn_button = 2;
constexpr int kdrive_speed_axis = 1;
constexpr int kdrive_curvature_axis = 4;
constexpr int kreverse_drive =  1;
constexpr int kball_aimbot_button = 3;
} // namespace Joy0Const

namespace Joy1Const {}

namespace SensorConst {
constexpr int kbreak_beam_port = 0;
constexpr int limit_switch_top_port = 1;
constexpr int limit_switch_bottom_port = 2;
// value at which color sensor proximity will return there is a ball 0(far away)
// - 2047(close as possible)
constexpr int kvalue_for_ball = 2000;
constexpr int kultrasonic_port = 0;
} // namespace SensorConst

namespace DriveConst {
constexpr int kleft_lead_neo_number = 3;
constexpr int kright_lead_neo_number = 4;
constexpr int kleft_follow_neo_number = 2;
constexpr int kright_follow_neo_number = 5;
constexpr bool kleft_lead_is_inverted = false;
constexpr bool kright_lead_is_inverted = true;
constexpr double kturn_div = 3;
} // namespace DriveConst

namespace MechanismConst {
constexpr double kP = 1, kI = 1, kD = 1, kIz = 1, kFF = 1, kMaxOutput = 1,
                 kMinOutput = 1;
constexpr int shooter_lead_port = 1;
constexpr int shooter_follow_port = 2;
constexpr int khopper_motor_top_port = 1;
constexpr int khopper_motor_bottom_port = 2;
constexpr float kcompressor_warning_pressure = 10;
constexpr int kvelocity_range = 1;
constexpr int kelevator_motor_port = 6;
constexpr int kelevator_pnumatic_port = 0;
constexpr int kintake_reversal_amps = 10;
constexpr int climb_amperage = 10;
constexpr int elevator_slow_multiplier = 0.5;

//shooter
constexpr int krange_target = 100;

constexpr int krange_reject = 100;
constexpr int kreject_target = 1000;

} // namespace MechanismConst

namespace AutoConst {}
