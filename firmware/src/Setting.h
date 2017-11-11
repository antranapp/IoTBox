#ifndef SETTING
#define SETTING

#include "Particle.h"
#include "ConfigurationTime.h"
#include "TimeZone.h"

class Setting {
    public:
        Setting();

        TimeZone getTimeZone();
        void setTimeZone(TimeZone timeZone);

        ConfigurationTime getOpenTime();
        void setOpenTime(ConfigurationTime openTime);

        ConfigurationTime getRemiderTime();
        void setRemiderTime(ConfigurationTime reminderTime);

        String getPin();
        void setPin(String pin);
    private:

};

#endif
