#ifndef COLORSENSOR
#define COLORSENSOR

#include "rev/ColorMatch.h"
#include "rev/ColorSensorV3.h"
#include <frc/util/Color.h>

#include "settings.h"

using namespace rev;

class ColorSensor {
public:
  ColorSensor(ColorSensorV3 *rev_color_sensor, ColorMatch *color_match, frc::Color *red_target,frc::Color *blue_target):rev_color_sensor(rev_color_sensor),color_match(color_match),red_target(red_target),blue_target(blue_target) {
    // Adds the target colors to colorToMatch
    color_match->AddColorMatch(*blue_target);
    // color_match->AddColorMatch(green_target);
    color_match->AddColorMatch(*red_target);
    // color_match->AddColorMatch(yellow_target);
  }
  std::string ClosestColor();
  frc::Color GetColor();

  bool CheckForBall(int kvalue_for_ball);

private:
  ColorSensorV3 *rev_color_sensor;
  ColorMatch *color_match;
  // The values for the target colors
  //= frc::Color(0.561, 0.232, 0.114);
  // = frc::Color(0.143, 0.427, 0.429);
  frc::Color *red_target;
  frc::Color *blue_target;

  frc::Color nearest_color;
  double confidence = 1.0;
};

#endif