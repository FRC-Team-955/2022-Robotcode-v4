#ifndef BUTTONTOGGLE
#define BUTTONTOGGLE

#include <frc/Timer.h>
#include <iostream>


class ButtonToggle {
public:
  ButtonToggle() { debounce_period = units::second_t(.1); }
  ButtonToggle(float period) { debounce_period = units::second_t(period); }

  bool GetToggleNoDebounce(bool button);
  void SetDebouncePeriod(float period);
  bool GetToggle(bool button);

private:
  void UpdateToggle(bool button);
  bool Debounce(bool button);

  bool toggle_pressed = 0;
  bool toggle_on = 0;

  units::second_t latest = units::second_t(0);
  units::second_t debounce_period;
};
#endif