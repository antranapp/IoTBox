#include "NFCManager.h"
#include "NfcTag.h"
#include <functional>

NFCManager::NFCManager(Display* display, Setting* setting): _display(display), _setting(setting) {
    using namespace std::placeholders;

    _nfcClient = new NFCClient(std::bind(&NFCManager::_nfcClientCallback, this, _1));
}

void NFCManager::setup() {

    _reset();

    _uid = _setting->getNfcTagUid();
    //Serial.print("get uid: ");Serial.println(_uid);

    _nfcClient->start();
}

bool NFCManager::updateAuthenticationStatus(NFCManagerStatus* status) {
    if (_lastStatus == IDLE) {
        return false;
    }

    *status = _lastStatus;

    _reset();

    return true;
}

void NFCManager::updateNfcTagUid(String uid) {
    _setting->setNfcTagUid(uid);
    _uid = uid;
}

void NFCManager::_nfcClientCallback(NfcTag tag) {
    String tagUid = tag.getUidString();
    Serial.print("uid: ");Serial.println(tagUid);
    _display->showNfcTagUid(tagUid);
    if (tagUid == _uid) {
        _lastStatus = AUTHENTICATED;
    } else {
        _lastStatus = NOT_AUTHENTICATED;
    }
}

void NFCManager::_reset() {
    _lastStatus = IDLE;
}
