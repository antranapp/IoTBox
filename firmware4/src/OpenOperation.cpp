#include "OpenOperation.h"
#include "Particle.h"

OpenOperation::OpenOperation(uint8_t relayPin_1, uint8_t relayPin_2, uint8_t relayPin_3, byte relayPin_4) {
    _relayPin_1 = relayPin_1;
    _relayPin_2 = relayPin_2;
    _relayPin_3 = relayPin_3;
    _relayPin_4 = relayPin_4;

    pinMode(_relayPin_1, OUTPUT);
    pinMode(_relayPin_2, OUTPUT);
    pinMode(_relayPin_3, OUTPUT);
    pinMode(_relayPin_4, OUTPUT);

    _isRunning = false;
}

void OpenOperation::start(uint8_t index) {
    if (!_isRunning) {
        _isRunning = true;
        _startTime = millis();
        switch (index) {
            case 1:
                _relayPin = _relayPin_1;
                break;
            case 2:
                _relayPin = _relayPin_2;
                break;
            case 3:
                _relayPin = _relayPin_3;
                break;
            case 4:
                _relayPin = _relayPin_4;
                break;
            default:
                return;
        }
        digitalWrite(_relayPin, HIGH);
    };
}

bool OpenOperation::update() {
    if (_isRunning) {
        if (millis() - _startTime > 1000 * 2) { // 2 seconds
            digitalWrite(_relayPin, LOW);
            _isRunning = false;

            return true;
        }
    };

    return false;
}

bool OpenOperation::isRunning() {
    return _isRunning;
}
