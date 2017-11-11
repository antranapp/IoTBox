/*
 * button.ino
 * 
 * Copyright (c) 2016, Evan Simkowitz
 * 
 * Wiring for the chip can be found at:
 * http://gtbtech.com/?p=875
 * Wire an led for the GP0 pin using the setup suggested in the wiring.
 * Wire a button to GP1. 
 */
#include "Adafruit_MCP23008/Adafruit_MCP23008.h"

Adafruit_MCP23008 mcp;

void setup() {
    mcp.begin();
    mcp.pinMode(0, OUTPUT);
    mcp.pinMode(1, INPUT);
    mcp.digitalWrite(0, LOW);
}

void loop() {
    // the library has a function pullUp() that pulls the input voltage up, but I haven't adapted it yet for Spark
    bool read = !mcp.digitalRead(1); 
    if (read) {
        mcp.digitalWrite(0, HIGH);
    }
    else {
        mcp.digitalWrite(0, LOW);
    }
}
