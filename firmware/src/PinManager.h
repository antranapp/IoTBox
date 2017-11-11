#include "Particle.h"
#include "InputKeypad.h"
#include "Display.h"
#include "Setting.h"

enum PinManagerStatus {
    ENTERING,
    CANCELLED,
    AUTHENTICATED,
    INVALID
};

class PinManager {
    public:

        PinManager(Display* display, Setting* setting);

        void startAuthentication();
        bool updateAuthenticationStatus(PinManagerStatus* status);
        void stopAuthentication();
        bool isAuthenticating();

        void savePin(String pin);

        int setPinHandler(String remotePin);

    private:
        InputKeypad _keypad;
        Display* _display;
        Setting* _setting;

        String _currentPin;
        String _finalPin;
        String _pin;

        void _reset();
        bool _checkPin();
        bool _isValidPin(String pin);
        String _readPinFromStorage();
};
