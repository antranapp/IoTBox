#include "Setting.h"

Setting::Setting() {
}

TimeZone Setting::getTimeZone() {
    TimeZone timeZone;

    EEPROM.get(0, timeZone);
    if (timeZone == 0xFFFFFFFF) {
        // EEPROM was empty -> initialize value
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
        // EEPROM was empty -> initialize default value
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
        // EEPROM was empty -> initialize default value
        reminderTime = { 0, 20, 30 };
    }

    return reminderTime;
}

void Setting::setRemiderTime(ConfigurationTime reminderTime) {
    EEPROM.put(sizeof(TimeZone) + sizeof(ConfigurationTime), reminderTime);
}
