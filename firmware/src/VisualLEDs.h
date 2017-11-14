#ifndef VISUALLEDS
#define VISUALLEDS

#include "ChainableLED.h"

class VisualLEDs {
    public:
        enum Color: byte {
            disabled = 0, red, green, blue
        };

        VisualLEDs(byte dataPin, byte clockPin);

        void updateNetworkStatus();
        void showNotification(Color color);

    private:
        const static byte LED_NETWORKSTATUS = 0;
        const static byte LED_NOTIFICATION = 0;
        const static byte NUM_LEDS = 1;

        ChainableLED _chainbleLEDs;
        Timer _timer;

        bool _wifiStatus;

        volatile byte _blinkingCounter;
        volatile bool _blinkingFlag;
        volatile Color _color;

        void _showNetworkStatus();
        void _callback(void);
};

#endif
