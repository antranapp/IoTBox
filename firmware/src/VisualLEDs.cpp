#include "VisualLEDs.h"
#include "Particle.h"

VisualLEDs::VisualLEDs(byte clockPin, byte dataPin) : _chainbleLEDs(clockPin, dataPin, VisualLEDs::NUM_LEDS), _timer(50, &VisualLEDs::_callback, *this, false) {
    _chainbleLEDs.init();

    // Show network status LED
    //_wifiStatus = Particle.connected();
    //_showNetworkStatus();

    _blinkingFlag = false;
    _blinkingCounter = 0;
    _color = disabled;
    _timer.stop();
}

void VisualLEDs::updateNetworkStatus() {

    bool newWifiStatus = Particle.connected();

    if (newWifiStatus != _wifiStatus) {

        _wifiStatus = newWifiStatus;

        _showNetworkStatus();
    }
}

void VisualLEDs::showNotification(Color color) {
    _blinkingCounter = 0;
    _blinkingFlag = false;
    _color = color;
    _timer.reset();
}

void VisualLEDs::_showNetworkStatus() {
    if (_wifiStatus) {
        _chainbleLEDs.setColorRGB(VisualLEDs::LED_NETWORKSTATUS, 0, 255, 0);
    } else {
        _chainbleLEDs.setColorRGB(VisualLEDs::LED_NETWORKSTATUS, 255, 0, 0);
    }
}

void VisualLEDs::_callback(void) {
    if (_color == disabled) {
        return;
    }

    if (_blinkingFlag) {
        switch (_color) {
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
    } else {
        _chainbleLEDs.setColorRGB(VisualLEDs::LED_NOTIFICATION, 0, 0, 0);
    }

    _blinkingFlag = !_blinkingFlag;

    _blinkingCounter++;

    if (_blinkingCounter > 6) {
        _color = disabled;
        _blinkingCounter = 0;
        _blinkingFlag = false;
        _timer.stop();
    }
}
