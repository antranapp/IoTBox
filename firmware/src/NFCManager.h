#pragma once

#include "NFCClient.h"
#include "Setting.h"
#include "Display.h"

class NFCManager {
    public:
        NFCManager(Display* display, Setting* setting);

        enum NFCManagerStatus {
            IDLE = 0,
            AUTHENTICATED,
            NOT_AUTHENTICATED
        };

        void setup();
        bool updateAuthenticationStatus(NFCManagerStatus* status);
        void updateNfcTagUid(String uid);

    private:
        NFCClient* _nfcClient;
        Setting* _setting;
        Display* _display;
        String _uid;
        NFCManagerStatus _lastStatus;

        void _nfcClientCallback(NfcTag tag);
        void _reset();
};
