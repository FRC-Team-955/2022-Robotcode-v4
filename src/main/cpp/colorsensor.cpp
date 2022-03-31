#include "colorsensor.h"

// (No clue what the confidence does [currently 1.0 needs to be double])
/**
 * @returns The closest matching out of the stored colors
 * @note Return blue if unsure
 */
std::string ColorSensor::ClosestColor() {
  nearest_color = color_match->MatchClosestColor(rev_color_sensor->GetColor(), confidence);
  if (nearest_color == *red_target) {
    return "Red";
  } else if (nearest_color == *blue_target) {
    return "Blue";
  } else {
    return "Blue";
  }
}
/// @returns the frc::Color value
frc::Color ColorSensor::GetColor() { return rev_color_sensor->GetColor(); }
/**
 * @param int the value needed to detect ball from 0(far) to 2047(close)
 * @returns bool If their is a ball
 */
bool ColorSensor::CheckForBall(int kvalue_for_ball) {
  // GetProximity return a int from 0(far) to 2047(close)
  return (rev_color_sensor->GetProximity() > kvalue_for_ball);
}