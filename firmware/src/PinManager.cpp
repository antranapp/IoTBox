#include "PinManager.h"

PinManager::PinManager(Display* display, Setting* setting) : _keypad(4) {
    _display = display;
    _setting = setting;

    _pin = _setting->getPin();
    if (!_isValidPin(_pin)) {
        _pin = "0000"; // TODO: make this a constant
    }

    Particle.function("setPin", &PinManager::setPinHandler, this);

    _reset();
}

void PinManager::startAuthentication() {
    _reset();
    _keypad.startEnteringPin();
    _display->showPin(_currentPin);
}

bool PinManager::isAuthenticating() {
    return _keypad.isEnteringPin();
}

bool PinManager::updateAuthenticationStatus(PinManagerStatus* status) {
    if (!_keypad.isEnteringPin()) {
        return false;
    }

    char key;

    bool result = _keypad.getPin(&key, &_currentPin, &_finalPin);

    if (result) {
        String keyString(key);

        _display->showPin(_currentPin);

        if (_finalPin == "") {
            if (keyString == "*") {
                *status = CANCELLED;
            } else {
                *status = ENTERING;
            }
        } else {
            if (_checkPin()) {
                *status = AUTHENTICATED;
            } else {
                *status = INVALID;

                _reset();

                // Show empty pin
                _display->showPin(_currentPin);
            }
        }

        return true;
    }

    _keypad.updateKey();

    return false;
}

void PinManager::stopAuthentication() {
    _keypad.stopEnteringPin();
    _reset();
}

bool PinManager::_checkPin() {
    return _finalPin == _pin;
}

int PinManager::setPinHandler(String remotePin) {
    if (!_isValidPin(remotePin)) {
        return -1;
    }

    _pin = remotePin;

    _setting->setPin(_pin);

    return 1;
}

void PinManager::_reset() {
    _currentPin = "";
    _finalPin = "";
}

bool PinManager::_isValidPin(String pin) {
    if (pin == "") {
        return false;
    }

    for (byte i = 0; i < pin.length(); i++) {
        if (!isDigit(pin.charAt(i))) {
            return false;
        }
    }
    return true;
}
