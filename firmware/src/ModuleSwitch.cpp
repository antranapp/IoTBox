#include "ModuleSwitch.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

ModuleSwitch::ModuleSwitch() {
    _setting = 0x00;
}

uint8_t ModuleSwitch::getModuleSetting() {
    return _setting;
}

void ModuleSwitch::update() {
    _setting = mcp17.readGPIO(1);
}

bool ModuleSwitch::isButtonEnabled() {
    return CHECK_BIT(_setting, 0);
}

bool ModuleSwitch::isKeypadEnabled() {
    return CHECK_BIT(_setting, 1);
}

bool ModuleSwitch::isRemoteEnabled() {
    return CHECK_BIT(_setting, 2);
}

bool ModuleSwitch::isRFIDEnabled() {
    return CHECK_BIT(_setting, 3);
}
