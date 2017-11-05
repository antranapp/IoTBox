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

    private:

};
