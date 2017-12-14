#include "Setting.h"

Setting::Setting() {
}

TimeZone Setting::getTimeZone() {
    TimeZone timeZone;

    EEPROM.get(0, timeZone);
    if (timeZone == 0xFFFFFFFF) {
        timeZone = 0.0;
    }

    return timeZone;
}

void Setting::setTimeZone(TimeZone timeZone) {
    EEPROM.put(0, timeZone);
}

ConfigurationTime Setting::getOpenTime() {
    ConfigurationTime openTime;

    EEPROM.get(sizeof(TimeZone), openTime);
    if (openTime.version != 0) {
        openTime = { 0, 7, 0 };
    }

    return openTime;
}

void Setting::setOpenTime(ConfigurationTime openTime) {
    EEPROM.put(sizeof(TimeZone), openTime);
}

ConfigurationTime Setting::getRemiderTime() {
    ConfigurationTime reminderTime;

    EEPROM.get(sizeof(TimeZone) + sizeof(ConfigurationTime), reminderTime);
    if (reminderTime.version != 0) {
        reminderTime = { 0, 20, 30 };
    }

    return reminderTime;
}

void Setting::setRemiderTime(ConfigurationTime reminderTime) {
    EEPROM.put(sizeof(TimeZone) + sizeof(ConfigurationTime), reminderTime);
}

String Setting::getPin() {
    const int STRING_BUF_SIZE = 5;
    char stringBuf[STRING_BUF_SIZE];
    int address = sizeof(TimeZone) + sizeof(ConfigurationTime) + sizeof(ConfigurationTime);

    EEPROM.get(address, stringBuf);
    stringBuf[sizeof(stringBuf) - 1] = 0; // make sure it's null terminated

    String str(stringBuf);

    return str;
}

void Setting::setPin(String pin) {
    const int STRING_BUF_SIZE = 5; // terminated character at the end
	char stringBuf[STRING_BUF_SIZE];
    int address = sizeof(TimeZone) + sizeof(ConfigurationTime) + sizeof(ConfigurationTime);

    pin.getBytes((unsigned char *)stringBuf, sizeof(stringBuf));
    EEPROM.put(address, stringBuf);
}


String Setting::getNfcTagUid() {
    int address = sizeof(TimeZone) + sizeof(ConfigurationTime) + sizeof(ConfigurationTime) + 5;

    NfcTagUid nfcTagUid;
    EEPROM.get(address, nfcTagUid);
    Serial.print("length: ");Serial.println(nfcTagUid.length);
    if (nfcTagUid.version != 0) {
        Serial.println("no uid found");
        return "";
    }

    nfcTagUid.uid[nfcTagUid.length] = 0;

    String uid(nfcTagUid.uid);

    Serial.print("found uid: ");Serial.println(uid);

    return uid;
}

void Setting::setNfcTagUid(String uid) {
    int address = sizeof(TimeZone) + sizeof(ConfigurationTime) + sizeof(ConfigurationTime) + 5;

    NfcTagUid nfcTagUid;
    uid.getBytes((unsigned char *)nfcTagUid.uid, sizeof(nfcTagUid.uid));
    nfcTagUid.length = uid.length();
    nfcTagUid.version = 0;

    EEPROM.put(address, nfcTagUid);
}
