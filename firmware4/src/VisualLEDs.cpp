#include "VisualLEDs.h"
#include "Particle.h"

VisualLEDs::VisualLEDs(byte clockPin, byte dataPin) : _chainbleLEDs(clockPin, dataPin, VisualLEDs::NUM_LEDS), _timer(100, &VisualLEDs::_callback, *this, false) {
    _chainbleLEDs.init();

    // Show network status LED
    // _wifiStatus = Particle.connected();
    // _showNetworkStatus();

    _blinkingFlag = false;
    _blinkingCounter = 0;
    _blinkingColor = disabled;
    _lastColor = disabled;

    _timer.stop();
}

// void VisualLEDs::updateNetworkStatus() {
//
//     bool newWifiStatus = Particle.connected();
//
//     if (newWifiStatus != _wifiStatus) {
//
//         _wifiStatus = newWifiStatus;
//
//         _showNetworkStatus();
//     }
// }

void VisualLEDs::showNotification(Color color, BlinkingDuration duration, BlinkingPeriod period) {
    _blinkingCounter = 0;
    _blinkingFlag = false;

    _blinkingColor = color;
    _duration = duration;

    _timer.stop();
    _timer.changePeriod(period);
    _timer.reset();
}

void VisualLEDs::setColor(Color color) {
    _lastColor = color;
    _setColor(color);
}

// void VisualLEDs::_showNetworkStatus() {
//     if (_wifiStatus) {
//         _chainbleLEDs.setColorRGB(VisualLEDs::LED_NETWORKSTATUS, 0, 255, 0);
//     } else {
//         _chainbleLEDs.setColorRGB(VisualLEDs::LED_NETWORKSTATUS, 255, 0, 0);
//     }
// }

void VisualLEDs::_callback(void) {
    if (_blinkingColor == disabled) {
        return;
    }

    if (_blinkingFlag) {
        _setColor(_blinkingColor);
    } else {
        _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 0, 0, 0);
    }

    _blinkingFlag = !_blinkingFlag;

    _blinkingCounter++;

    if (_blinkingCounter > _duration) {
        _blinkingColor = disabled;
        _blinkingCounter = 0;
        _blinkingFlag = false;
        _timer.stop();

        _setColor(_lastColor);
        // Restore the network status LED
        // _showNetworkStatus();
    }
}

void VisualLEDs::_setColor(Color color) {
    switch (color) {
        case disabled:
            _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 0, 0, 0);
            break;
        case red:
            _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 255, 0, 0);
            break;
        case green:
            _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 0, 255, 0);
            break;
        case blue:
            _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 0, 0, 255);
            break;
    }
}
