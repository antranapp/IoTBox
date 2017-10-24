
#include "Grove_LCD_RGB_Backlight.h"
#include "ConfigurationTime.h"

class Display {
    public:
        Display();

        void setup();

        void updateTimerCounter();

        void turnOn();
        void turnOff();

        bool showStatus(bool status);
        bool showOpenTime(ConfigurationTime time);
        bool showReminderTime(ConfigurationTime time);

    private:
        rgb_lcd _lcd;
        volatile bool _isLCDBusy;
        volatile int _displayTimerCounter;
        bool _displayIsOn;

        void _showText(String text, uint8_t row, uint8_t col);
        String _addLeadingZero(uint8_t num);
        String _timeToString(ConfigurationTime time);
};
