#pragma once

struct ConfigurationTime {
    uint8_t version;
    uint8_t hour;
    uint8_t minute;

    String toString() {
        return String(hour) + ":" + String(minute);
    }

    static bool fromString(String timeString, ConfigurationTime* time) {

        String hourString, minuteString;
        int hour, minute;

        int delimeterPosition = timeString.indexOf(":");

        if (delimeterPosition >= 0) {

            hourString = timeString.substring(0, delimeterPosition);
            minuteString = timeString.substring(delimeterPosition + 1);
            hour = hourString.toInt();
            minute = minuteString.toInt();

            if (hour < 0 && hour > 23) {
                return false;
            }

            if (minute < 0 && minute > 59) {
                return false;
            }

            *time = { 0, hour, minute };

            return true;
        }

        return false;
    }
};
