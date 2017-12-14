#pragma one

#include "Particle.h"
#include "PN532_SPI.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "NfcTag.h"

#include <functional>

enum NFCClientState {
    ERROR_ALREADY_STARTED = -1,
    START_SUCCEED,
    STOPPED,
};

class NFCClient {
    public:

        typedef std::function<void(NfcTag)> NFCClientReadMessageHandler;

        NFCClient(NFCClientReadMessageHandler callback);

        int start();
        int stop();

    private:
        Timer* _timer;
        NFCClientReadMessageHandler callback;

        PN532_SPI _pn532_spi;
        NfcAdapter _nfcAdapter;

        bool _readMessage();

        NfcTag _lastTag;
        NfcTag _currentTag;

        bool _isRunning();

        void _timerCallback(void);

        volatile bool _isReadingNFCMessage;
};
