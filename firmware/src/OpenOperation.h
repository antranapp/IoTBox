#include "Particle.h"

class OpenOperation {

    public:
        OpenOperation(uint8_t buttonPin);

        void start();

        void stop();

        bool isRunning();

    private:
        uint8_t _relayPin;

        Timer* _timer;

        void _callback(void);
};
