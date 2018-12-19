#include "Particle.h"

class OpenOperation {

    public:
        OpenOperation(uint8_t relayPin_1, uint8_t relayPin_2, uint8_t relayPin_3, byte relayPin_4);

        void start(uint8_t relayPin);
        bool update();

        bool isRunning();

    private:
        uint8_t _relayPin_1;
        uint8_t _relayPin_2;
        uint8_t _relayPin_3;
        uint8_t _relayPin_4;

        uint8_t _relayPin;

        unsigned long _startTime;

        volatile bool _isRunning;
};
