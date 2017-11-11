#include "Particle.h"
#include "Keypad_I2C.h"

class InputKeypad {
    public:
        InputKeypad(uint8_t pinLength);

        void startEnteringPin();
        void stopEnteringPin();
        bool isEnteringPin();
        bool getPin(char* currentKey, String* currentPin, String* finalPin);
        void updateKey();

    private:
        Keypad _keypad;

        uint8_t _pinLength;
        bool _isEnteringPin;
        bool _isPinUpdated;

        String _pin;
        char _key;

        void _reset();
        bool _isValidPinKey(char key);
};
