#include "VisualLEDs.h"
#include "Particle.h"

VisualLEDs::VisualLEDs(byte clockPin, byte dataPin) : _chainbleLEDs(clockPin, dataPin, 1) {
    _chainbleLEDs.init();
    _wifiStatus = Particle.connected();
    _showNetworkStatus();
}

void VisualLEDs::updateNetworkStatus() {

    bool newWifiStatus = Particle.connected();

    if (newWifiStatus != _wifiStatus) {

        _wifiStatus = newWifiStatus;

        Serial.println(_wifiStatus);

        _showNetworkStatus();
    }
}

void VisualLEDs::_showNetworkStatus() {
    if (_wifiStatus) {
        _chainbleLEDs.setColorRGB(0, 0, 255, 0);
    } else {
        _chainbleLEDs.setColorRGB(0, 255, 0, 0);
    }
}
