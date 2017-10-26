#include "Particle.h"

class OpenOperation {

    public:
        OpenOperation(uint8_t relayPin, uint8_t ledPin);

        void start();

        bool isRunning();

    private:
        uint8_t _relayPin;
        uint8_t _ledPin;

        volatile bool _isRunning;
};
