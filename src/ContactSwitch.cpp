
#include "ContactSwitch.h"

ContactSwitch::ContactSwitch(uint8_t buttonPin)
{
  _pin           = buttonPin;
  _activeHigh    = LOW;           // Assume active-low button
  _btnState      = !_activeHigh;  // initial button state in active-high logic
  _lastState     = _btnState;
  _clickCount    = 0;
  depressed      = false;
  _lastBounceTime= 0;
  debounceTime   = 20;            // Debounce timer in ms
  pinMode(_pin, INPUT);
}

void ContactSwitch::Update()
{
  long now = (long)millis();      // get current time
  _btnState = digitalRead(_pin);  // current appearant button state

  // If the switch changed, due to noise or a button press, reset the debounce timer
  if (_btnState != _lastState) _lastBounceTime = now;

  // debounce the button (Check if a stable, changed state has occured)
  if (now - _lastBounceTime > debounceTime)
  {
      stableState = _btnState;
  }

  _lastState = _btnState;
}
