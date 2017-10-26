#include "OpenOperation.h"

OpenOperation::OpenOperation(uint8_t relayPin, uint8_t ledPin) {
    _relayPin = relayPin;
    _ledPin = ledPin;
    _isRunning = false;
}

void OpenOperation::start() {
    if (!_isRunning) {
        _isRunning = true;
        digitalWrite(_ledPin, HIGH);
        digitalWrite(_relayPin, HIGH);
        delay(2000);
        digitalWrite(_relayPin, LOW);
        digitalWrite(_ledPin, LOW);
        _isRunning = false;
    };
}

bool OpenOperation::isRunning() {
    return _isRunning;
}
