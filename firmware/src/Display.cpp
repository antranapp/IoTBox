
#include "Display.h"

Display::Display() {
    _displayIsOn = false;
    _isLCDBusy = false;
    _displayTimerCounter = 0;
}

void Display::setup() {
    // Set up background backlight color
    _lcd.setRGB(100, 100, 100);

    _lcd.begin(16, 2);
    _displayIsOn = true;
}

void Display::updateTimerCounter() {
    if (_displayIsOn) {
        _displayTimerCounter++;
        //Serial.println(_displayTimerCounter);
        if (_displayTimerCounter > 20) {
            turnOff();
            _displayTimerCounter = 0;
        }
    }
}

void Display::turnOn() {
    if (!_displayIsOn) {
        _lcd.display();
        _lcd.setColorWhite();
        _displayIsOn = 1;
    }
}

void Display::turnOff() {
    _displayIsOn = 0;
    _lcd.setColorAll();
    _lcd.noDisplay();
}

bool Display::showTime(String timeString) {

    if (!_displayIsOn) {
        return true;
    }

    if (!_isLCDBusy) {

        turnOn();

        _showText(timeString, 0, 0);

        return true;
    }

    return false;
}

bool Display::showOpenTime(ConfigurationTime time) {

    _displayTimerCounter = 0;

    if (!_isLCDBusy) {

        turnOn();

        String text = _timeToString(time);
        _showText(text, 11, 0);

        return true;
    }

    return false;
}

bool Display::showReminderTime(ConfigurationTime time) {

    _displayTimerCounter = 0;

    if (!_isLCDBusy) {

        turnOn();

        String text = _timeToString(time);
        _showText(text, 11, 1);

        return true;
    }

    return false;
}

bool Display::showStatus(bool status) {

    _displayTimerCounter = 0;

    if (!_isLCDBusy) {

        turnOn();

        if (status == HIGH) {
            _showText("CLOSED", 0, 1);
        } else {
            _showText("OPEN", 0, 1);
        }

        return true;
    }

    return false;
}


bool Display::showPin(String pin) {

    _displayTimerCounter = 0;

    if (!_isLCDBusy) {

        turnOn();

        _showText("Pin:" + pin, 0, 0);

        return true;
    }

    return false;
}

void Display::_showText(String text, uint8_t row, uint8_t col) {
    _isLCDBusy = true;

    _lcd.setCursor(row,col);
    _lcd.print("        ");
    _lcd.setCursor(row,col);
    _lcd.print(text);

    _isLCDBusy = false;
}

String Display::_addLeadingZero(uint8_t num) {
    String zero = "0";
    if (num < 10) {
        return zero + String(num);
    } else {
        return String(num);
    }
}

String Display::_timeToString(ConfigurationTime time) {
    String separator = ":";
    String result = _addLeadingZero(time.hour) + separator + _addLeadingZero(time.minute);
    return result;
}
