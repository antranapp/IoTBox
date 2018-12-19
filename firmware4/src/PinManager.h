#include "Particle.h"
#include "InputKeypad.h"

enum PinManagerStatus {
    ENTERING,
    CANCELLED,
    AUTHENTICATED_1,
    AUTHENTICATED_2,
    AUTHENTICATED_3,
    AUTHENTICATED_4,
    INVALID
};

class PinManager {
    public:

        PinManager();

        void startAuthentication();
        bool updateAuthenticationStatus(PinManagerStatus* status);
        void stopAuthentication();
        bool isAuthenticating();

    private:
        InputKeypad _keypad;

        String _currentPin;
        String _finalPin;

        void _reset();
        bool _checkPin();
        bool _isValidPin(String pin);
};
