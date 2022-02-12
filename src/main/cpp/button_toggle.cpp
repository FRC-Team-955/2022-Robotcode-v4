#include <button_toggle.h>

void ButtonToggle::UpdateToggle(bool button) {
  if (button) {
    if (!toggle_pressed) {
      toggle_on = !toggle_on;
      toggle_pressed = true;
    }
  } else {
    toggle_pressed = false;
  }
}

bool ButtonToggle::GetToggleNoDebounce(bool button) {
  UpdateToggle(button);

  if (toggle_on) {
    return true;
  } else {
    return false;
  }
}

bool ButtonToggle::GetToggle(bool button) {
  UpdateToggle(Debounce(button));

  if (toggle_on) {
    return true;
  } else {
    return false;
  }
}

void ButtonToggle::SetDebouncePeriod(float period) {
  debounce_period = units::second_t(period);
}

bool ButtonToggle::Debounce(bool button) {
  units::second_t now = frc::Timer::GetFPGATimestamp();
  if (button) {
    if ((now - latest) > debounce_period) {
      latest = now;
      return true;
    }
  }
  return false;
}