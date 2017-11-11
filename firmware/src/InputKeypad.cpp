#include "InputKeypad.h"

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char* I2CTYPE = "Adafruit_MCP23017";

char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
byte rowPins[ROWS] = {6, 5, 4, 3}; //row pins
byte colPins[COLS] = {2, 1, 0}; //connect to the column pinouts of the keypad

InputKeypad::InputKeypad(uint8_t pinLength) : _keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CTYPE ) {
    _pinLength = pinLength;
    _isEnteringPin = false;
    _isPinUpdated = false;
    _pin = "";
}

void InputKeypad::startEnteringPin() {
    _isEnteringPin = true;
}

bool InputKeypad::isEnteringPin() {
    return _isEnteringPin;
}

void InputKeypad::stopEnteringPin() {
    _isEnteringPin = false;
    _reset();
}

bool InputKeypad::getPin(char* currentKey, String* currentPin, String* finalPin) {
    if (!_isEnteringPin) {
        return false;
    }

    if (_isPinUpdated) {
        _isPinUpdated = false;

        *currentKey = _key;
        *currentPin = _pin;

        if (_pin.length() >= _pinLength) {
            *finalPin = _pin;
            stopEnteringPin();
        } else {
            *finalPin = "";
        }


        return true;
    }

    return false;
}

void InputKeypad::updateKey() {
    if (!_isEnteringPin) {
        return;
    }

    char key = _keypad.getKey();
    if (key) {
        _key = key;
        if (_isValidPinKey(key)) {
            _pin = _pin + key;
        }
        _isPinUpdated = true;
    }
}

void InputKeypad::_reset() {
    _isPinUpdated = false;
    _pin = "";
}

bool InputKeypad::_isValidPinKey(char key) {
    return isDigit(key);
}
