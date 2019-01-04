/*
 * Project IoTBox4
 * Description: A internet-connected box which can be opened from API (version 4 trays)
 * Author: Peacemoon
 * Date: 19.12.2018
 */

#include "OpenOperation.h"
#include "PinManager.h"
#include "clickButton.h"
#include "VisualLEDs.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

#define BUTTONPIN D2
#define RELAYPIN_1 D3
#define RELAYPIN_2 D4
#define RELAYPIN_3 D5
#define RELAYPIN_4 D6

#define VISUALLED_CLOCKPIN A0
#define VISUALLED_DATAPIN A1

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
// SEMI_AUTOMATIC loop is still running even without Internet connection.
SYSTEM_MODE(SEMI_AUTOMATIC);
// SYSTEM_THREAD enables parallel operations of user & system codes
SYSTEM_THREAD(ENABLED);

unsigned long lastSync = millis();

volatile bool requestOpeningOperation = false;
volatile uint8_t openingTrayIndex = 0;

bool isStarting = true;

ClickButton button(BUTTONPIN, HIGH);
VisualLEDs visualLEDs(VISUALLED_CLOCKPIN, VISUALLED_DATAPIN);
OpenOperation openOperation(RELAYPIN_1, RELAYPIN_2, RELAYPIN_3, RELAYPIN_4);
PinManager pinManager;

void setup() {

    // Setup button timers (all in milliseconds / ms)
    pinMode(BUTTONPIN, INPUT_PULLDOWN);
    button.debounceTime   = 20;   // Debounce timer in ms
    button.multiclickTime = 250;  // Time limit for multi clicks
    button.longClickTime  = 1000; // time until "held-down clicks" register

    // Do not open the box when system is turned on
    pinMode(RELAYPIN_1, OUTPUT);
    pinMode(RELAYPIN_2, OUTPUT);
    pinMode(RELAYPIN_3, OUTPUT);
    pinMode(RELAYPIN_4, OUTPUT);
    digitalWrite(RELAYPIN_1, LOW);
    digitalWrite(RELAYPIN_2, LOW);
    digitalWrite(RELAYPIN_3, LOW);
    digitalWrite(RELAYPIN_4, LOW);

    visualLEDs.setColor(VisualLEDs::Color::disabled);

    Serial.begin(9600);
}

void loop() {
    syncTimeWithCloudIfNeeded();

    if (requestOpeningOperation) {
        openOperation.start(openingTrayIndex);
    }

    if (openOperation.update()) { // Operation is finished.
        requestOpeningOperation = false;
        openingTrayIndex = 0;
    }

    checkButtonState();
    checkPin();

    isStarting = false;

    if (Particle.connected() == false) {
      Particle.connect();
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

    if (isStarting) {
        return;
    }

    if (requestOpeningOperation) {
        return;
    }

    // Update button state
    button.Update();

    // Save click codes in function, as click codes are reset at next Update()
    if (button.clicks != 0) {
        function = button.clicks;
    }

    if (function == 2 || function == -2) { // Double (LONG) click
        visualLEDs.setColor(VisualLEDs::Color::green);
        pinManager.startAuthentication();
    }
}

void checkPin() {
    PinManagerStatus status;
    if (pinManager.updateAuthenticationStatus(&status)) {
        switch (status) {
            case ENTERING:
                visualLEDs.showNotification(VisualLEDs::Color::blue, VisualLEDs::BlinkingDuration::duration_short, VisualLEDs::BlinkingPeriod::period_short);
                break;
            case CANCELLED:
                visualLEDs.setColor(VisualLEDs::Color::disabled);
                pinManager.stopAuthentication();
                break;
            case AUTHENTICATED_1:
                visualLEDs.setColor(VisualLEDs::Color::disabled);
                startOpenOperation(1);
                break;
            case AUTHENTICATED_2:
                visualLEDs.setColor(VisualLEDs::Color::disabled);
                startOpenOperation(2);
                break;
            case AUTHENTICATED_3:
                visualLEDs.setColor(VisualLEDs::Color::disabled);
                startOpenOperation(3);
                break;
            case AUTHENTICATED_4:
                visualLEDs.setColor(VisualLEDs::Color::disabled);
                startOpenOperation(4);
                break;
            case INVALID:
                visualLEDs.showNotification(VisualLEDs::Color::red, VisualLEDs::BlinkingDuration::duration_medium, VisualLEDs::BlinkingPeriod::period_very_long);
                break;
        }
    }
}

void startOpenOperation(uint8_t index) {
    requestOpeningOperation = true;
    openingTrayIndex = index;
}
