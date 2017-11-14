#include "Particle.h"
#include "VisualLEDs.h"

class OpenOperation {

    public:
        OpenOperation(byte relayPin, VisualLEDs* visualLEDs);

        void start();

        bool isRunning();

    private:
        uint8_t _relayPin;
        VisualLEDs* _visualLEDs;

        volatile bool _isRunning;
};
