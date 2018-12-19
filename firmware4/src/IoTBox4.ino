/*
 * Project IoTBox4
 * Description: A internet-connected box which can be opened from API (version 4 trays)
 * Author: Peacemoon
 * Date: 19.12.2018
 */

#include "OpenOperation.h"
#include "PinManager.h"
#include "clickButton.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

#define BUTTONPIN D2
#define RELAYPIN_1 D3
#define RELAYPIN_2 D4
#define RELAYPIN_3 D5
#define RELAYPIN_4 D6

unsigned long lastSync = millis();

volatile bool requestOpeningOperation = false;
volatile uint8_t openingTrayIndex = 0;

ClickButton button(BUTTONPIN, LOW);
OpenOperation openOperation(RELAYPIN_1, RELAYPIN_2, RELAYPIN_3, RELAYPIN_4);
PinManager pinManager;

void setup() {

    // Setup button timers (all in milliseconds / ms)
    button.debounceTime   = 20;   // Debounce timer in ms
    button.multiclickTime = 150;  // Time limit for multi clicks
    button.longClickTime  = 1000; // time until "held-down clicks" register

    // Do not open the box when system is turned on
    digitalWrite(RELAYPIN_1, LOW);
    digitalWrite(RELAYPIN_2, LOW);
    digitalWrite(RELAYPIN_3, LOW);
    digitalWrite(RELAYPIN_4, LOW);

    Serial.begin(9600);
}

void loop() {
    syncTimeWithCloudIfNeeded();

    if (requestOpeningOperation) {
        openOperation.start(openingTrayIndex);
        requestOpeningOperation = false;
        openingTrayIndex = 0;
    } else {
        openOperation.update();

        checkButtonState();
        checkPin();
    }
}

void syncTimeWithCloudIfNeeded() {
    if (millis() - lastSync > ONE_DAY_MILLIS) {
      // Request time synchronization from the Particle Cloud
      Particle.syncTime();
      waitUntil(Particle.syncTimeDone);
      lastSync = millis();
    }
}

void checkButtonState() {
    int function = 0;

    if (requestOpeningOperation) {
        return;
    }

    // Update button state
    button.Update();

    // Save click codes in function, as click codes are reset at next Update()
    if (button.clicks != 0) {
        function = button.clicks;
    }

    if (function == 1 || function == -1) { // (LONG) click
        pinManager.startAuthentication();
    }
}

void checkPin() {
    PinManagerStatus status;
    if (pinManager.updateAuthenticationStatus(&status)) {
        Serial.print("status: ");
        Serial.println(status);
        switch (status) {
            case ENTERING:
                Serial.println("ENTERING");
                break;
            case CANCELLED:
                Serial.println("CANCELLED");
                pinManager.stopAuthentication();
                break;
            case AUTHENTICATED_1:
                Serial.println("AUTHENTICATED_1");
                startOpenOperation(1);
                break;
            case AUTHENTICATED_2:
                Serial.println("AUTHENTICATED_2");
                startOpenOperation(2);
                break;
            case AUTHENTICATED_3:
                Serial.println("AUTHENTICATED_3");
                startOpenOperation(3);
                break;
            case AUTHENTICATED_4:
                Serial.println("AUTHENTICATED_4");
                startOpenOperation(4);
                break;
            case INVALID:
                Serial.println("INVALID");
                break;
        }
    }
}

void startOpenOperation(uint8_t index) {
    Serial.print("Opening: ");
    Serial.println(index);

    requestOpeningOperation = true;
    openingTrayIndex = index;
}
