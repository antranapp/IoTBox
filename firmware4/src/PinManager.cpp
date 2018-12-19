#include "PinManager.h"

PinManager::PinManager() : _keypad(4) {
    _reset();
}

void PinManager::startAuthentication() {
    _reset();
    _keypad.startEnteringPin();
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

        if (_finalPin == "") {
            if (keyString == "*") {
                *status = CANCELLED;
            } else {
                *status = ENTERING;
            }
        } else {
            if (_finalPin == "1111") {
                *status = AUTHENTICATED_1;
            } else if (_finalPin == "2222") {
                *status = AUTHENTICATED_2;
            } else if (_finalPin == "3333") {
                *status = AUTHENTICATED_3;
            } else if (_finalPin == "4444") {
                *status = AUTHENTICATED_4;
            } else {
                *status = INVALID;
                _reset();
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
