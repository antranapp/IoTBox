#ifndef VISUALLEDS
#define VISUALLEDS

#include "ChainableLED.h"

class VisualLEDs {
    public:
        enum Color: byte {
            disabled = 0, red, green, blue
        };

        enum BlinkingDuration: uint8_t {
            duration_short = 2, duration_medium = 4, duration_long = 6, duration_very_long = 10
        };

        enum BlinkingPeriod: unsigned int {
            period_short = 50, period_medium = 100, period_long = 200, period_very_long = 1000
        };

        VisualLEDs(byte dataPin, byte clockPin);

        void updateNetworkStatus();
        void showNotification(Color color, BlinkingDuration duration, BlinkingPeriod);

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
        volatile BlinkingDuration _duration;

        void _showNetworkStatus();
        void _callback(void);
};

#endif
