#include "Particle.h"
#include "NFCClient.h"

#define SS A2

NFCClient::NFCClient(NFCClientReadMessageHandler callback_):
    _pn532_spi(SPI, SS),
    _nfcAdapter(_pn532_spi),
    callback(std::move(callback_)),
    _isReadingNFCMessage(0) {

    _timer = new Timer(1000, &NFCClient::_timerCallback, *this, false);
}

int NFCClient::start() {
    if (_isRunning()) {
        return NFCClientState::ERROR_ALREADY_STARTED;
    }

    _nfcAdapter.begin();
    _timer->reset();

    return NFCClientState::START_SUCCEED;
}

int NFCClient::stop() {
    _timer->stop();
    return NFCClientState::STOPPED;
}

bool NFCClient::_readMessage() {
    _isReadingNFCMessage = true;

    if (_nfcAdapter.tagPresent()) {
        NfcTag tag = _nfcAdapter.read();
        if (tag.getUidLength() > 0) {
            _lastTag = tag;
            _isReadingNFCMessage = false;
            return true;
        }
    }

    _isReadingNFCMessage = false;
    return false;
}

bool NFCClient::_isRunning() {
    return _timer->isActive();
}

void NFCClient::_timerCallback() {
    if (_isReadingNFCMessage) {
        return;
    }

    // Check if we can find a tag.
    if (!_readMessage()) {
        if (_currentTag.getUidLength() > 0) {
            _currentTag = NfcTag();
        }
        return;
    }

    // Check if this is the same tag.
    if (_lastTag == _currentTag) {
        return;
    }

    if (callback) {
        _currentTag = _lastTag;
        callback(_currentTag);
    }
}
