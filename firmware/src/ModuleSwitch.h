#ifndef __MODULE_SWITCH__
#define __MODULE_SWITCH__

#include "Adafruit_MCP23017.h"

class ModuleSwitch {
    public:
        ModuleSwitch();

        void update();
        uint8_t getModuleSetting();
        bool isButtonEnabled();
        bool isKeypadEnabled();
        bool isRemoteEnabled();
        bool isRFIDEnabled();

    private:
        Adafruit_MCP23017 mcp17;

        uint8_t _setting;
};

#endif
