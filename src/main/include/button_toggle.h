#ifndef BUTTONTOGGLE
#define BUTTONTOGGLE

#include <frc/Timer.h>

class ButtonToggle {
public:
  bool GetToggleNoDebounce(bool button);
  void SetDebouncePeriod(float period);
  bool GetToggle(bool button);
  void SetToggleState(bool state);
  bool GetToggleState();
  units::second_t debounce_period = units::second_t(.1);
private:
  void UpdateToggle(bool button);
  bool Debounce(bool button);

  bool toggle_pressed = 0;
  bool toggle_on = 0;

  units::second_t latest = units::second_t(0);
};
#endif