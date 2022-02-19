#ifndef COLORSENSOR
#define COLORSENSOR

#include "rev/ColorMatch.h"
#include "rev/ColorSensorV3.h"
#include <frc/util/Color.h>

#include "settings.h"

using namespace rev;

class ColorSensor {
public:
  ColorSensor(ColorSensorV3 *rev_color_sensor):rev_color_sensor(rev_color_sensor) {
    color_match = new ColorMatch();

    // Adds the target colors to  colorToMatch
    color_match->AddColorMatch(blue_target);
    color_match->AddColorMatch(green_target);
    color_match->AddColorMatch(red_target);
    color_match->AddColorMatch(yellow_target);
  }
  std::string ClosestColor();
  frc::Color GetColor();

  bool CheckForBall();

private:
  ColorSensorV3 *rev_color_sensor;
  ColorMatch *color_match;
  // The values for the target colors
  static constexpr frc::Color blue_target = frc::Color(0.143, 0.427, 0.429);
  static constexpr frc::Color green_target = frc::Color(0.197, 0.561, 0.240);
  static constexpr frc::Color red_target = frc::Color(0.561, 0.232, 0.114);
  static constexpr frc::Color yellow_target = frc::Color(0.361, 0.524, 0.113);

  frc::Color nearest_color;
  double confidence = 1.0;
};

#endif