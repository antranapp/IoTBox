#include "OpenOperation.h"
#include "Particle.h"

OpenOperation::OpenOperation(uint8_t relayPin, VisualLEDs* visualLEDs) {
    pinMode(relayPin, OUTPUT);

    _relayPin = relayPin;
    _visualLEDs = visualLEDs;
    _isRunning = false;
}

void OpenOperation::start() {
    if (!_isRunning) {
        _isRunning = true;
        _startTime = millis();
        digitalWrite(_relayPin, HIGH);
        //delay(2000);
    };
}

void OpenOperation::update() {
    if (_isRunning) {
        if (millis() - _startTime > 1000 * 2) { // 2 seconds
            digitalWrite(_relayPin, LOW);
            _isRunning = false;
        }
    };
}

bool OpenOperation::isRunning() {
    return _isRunning;
}
