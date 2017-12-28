#include "Particle.h"
#include "VisualLEDs.h"

class OpenOperation {

    public:
        OpenOperation(byte relayPin, VisualLEDs* visualLEDs);

        void start();
        void update();

        bool isRunning();

    private:
        uint8_t _relayPin;
        VisualLEDs* _visualLEDs;
        unsigned long _startTime;

        volatile bool _isRunning;
};
