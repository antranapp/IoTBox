#pragma once

#include "Particle.h"
#include "ConfigurationTime.h"
#include "TimeZone.h"
#include "NfcTagUid.h"

class Setting {
    public:
        Setting();

        TimeZone getTimeZone();
        void setTimeZone(TimeZone timeZone);

        ConfigurationTime getOpenTime();
        void setOpenTime(ConfigurationTime openTime);

        ConfigurationTime getReminderTime();
        void setReminderTime(ConfigurationTime reminderTime);

        String getPin();
        void setPin(String pin);

        String getNfcTagUid();
        void setNfcTagUid(String);
    private:

};
