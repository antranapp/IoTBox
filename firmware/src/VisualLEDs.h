#ifndef VISUALLEDS
#define VISUALLEDS

#include "ChainableLED.h"

class VisualLEDs {
    public:
        VisualLEDs(byte dataPin, byte clockPin);

        void updateNetworkStatus();

    private:
        ChainableLED _chainbleLEDs;

        bool _wifiStatus;

        void _showNetworkStatus();
};

#endif
