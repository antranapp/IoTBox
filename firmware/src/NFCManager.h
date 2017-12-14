#pragma once

#include "NFCClient.h"
#include "Setting.h"

class NFCManager {
    public:
        NFCManager(Setting* setting);

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
        String _uid;
        NFCManagerStatus _lastStatus;

        void _nfcClientCallback(NfcTag tag);
        void _reset();
};
