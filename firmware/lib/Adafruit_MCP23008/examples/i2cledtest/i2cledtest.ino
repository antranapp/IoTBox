/* 
 *  ledtest.ino
 *  
 *  Copyright (c) 2016, Evan Simkowitz
 *  Wiring for the chip can be found at:
 *  http://gtbtech.com/?p=875
 *  Wire an led for the GP0 pin using the setup suggested in the wiring
 *  diagram above.
 */
#include "Adafruit_MCP23008/Adafruit_MCP23008.h"

bool ledOn = false;

unsigned long int ledAccumulator = 0;
const unsigned int ledDelay = 1000;

Adafruit_MCP23008 mcp;
void setup() {
    mcp.begin();
    mcp.pinMode(0, OUTPUT);
    mcp.digitalWrite(0, LOW);
}

void loop() {
    if (millis() - ledAccumulator > ledDelay) {
        ledAccumulator = millis();
        if (ledOn) {
            mcp.digitalWrite(0, LOW);
            ledOn = false;
        }
        else {
            mcp.digitalWrite(0, HIGH);
            ledOn = true;
        }
    }
}
