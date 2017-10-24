#include "OpenOperation.h"

OpenOperation::OpenOperation(uint8_t relayPin) {
    _relayPin = relayPin;
    pinMode(_relayPin, OUTPUT);

    _timer = new Timer(2000, (void (*)())&OpenOperation::_callback, true);
}

void OpenOperation::start() {
    // TODO: Delay 2 seconds after 2 consecutive open operations
    if (!isRunning()) {
        digitalWrite(_relayPin, HIGH);
        _timer->reset();
    }
}

void OpenOperation::stop() {
    digitalWrite(_relayPin, LOW);
    _timer->dispose();
}

bool OpenOperation::isRunning() {
    return _timer->isActive();
}

void OpenOperation::_callback(void) {
    digitalWrite(_relayPin, LOW);
}
