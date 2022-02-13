#include "colorsensor.h"

// Returns the closest matching out of the stored colors (No clue what the
// confidence does [currently 1.0 needs to be double])
std::string ColorSensor::ClosestColor() {
  nearest_color =
      color_match->MatchClosestColor(rev_color_sensor->GetColor(), confidence);
  if (nearest_color == red_target) {
    return "Red";
  } else if (nearest_color == blue_target) {
    return "Blue";
  } else if (nearest_color == green_target) {
    return "Green";
  } else if (nearest_color == yellow_target) {
    return "Yellow";
  } else {
    return "What happend (None)";
  }
}
// Just returns a frc::Color value
frc::Color ColorSensor::GetColor() { return rev_color_sensor->GetColor(); }
bool ColorSensor::CheckForBall() {
  // GetProximity return a int from 0(far) to 2047(close)
  return (rev_color_sensor->GetProximity() > SensorConst::kvalue_for_ball);
}