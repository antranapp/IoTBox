/*
 * Project IoTBox
 * Description: A internet-connected box which can be opened from API
 * Author: Peacemoon
 * Date: 17.09.2017
 */

 PRODUCT_ID(5673);
 PRODUCT_VERSION(1);

 STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
 SYSTEM_MODE(AUTOMATIC)
 SYSTEM_THREAD(ENABLED);

#include "clickButton.h"
#include "ContactSwitch.h"
#include "Display.h"
#include "ConfigurationTime.h"
#include "OpenOperation.h"
#include "Buzzer.h"
#include "PinManager.h"
#include "Setting.h"
#include "VisualLEDs.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

#define BUTTONPIN D2 // Button reserved for further usage
#define BUZZERPIN D3 // Buzzer reserved for further usage
#define RELAYPIN D4 // Relay to drive the electromagnetic lock.
#define SWITCHPIN D5 // Switch for "LOCKED" or "OPEN" status.
#define LED_CLOCKPIN D6
#define LED_DATAPIN D7

unsigned long lastSync = millis();

int switchState = LOW; // HIGH = locked, LOW = open
volatile bool requestDisplayStatus = true;
volatile bool requestDisplayTime = false;
volatile bool requestDisplayConfigurationTime = true;
volatile bool requestDisplayReminderTime = true;
volatile bool requestOpeningOperation = false;

TimeZone timeZone = 0.0;

Display display = Display();
Buzzer buzzer(BUZZERPIN);

Timer clockTimer(1000, updateTime); // timer used to display the clock on the lcd display

volatile int hour;
volatile int minute;
volatile int second;

int configureOpenTime(String time);
void syncTimeWithCloudIfNeeded();

ConfigurationTime openTime;
ConfigurationTime reminderTime;

const char *PUBLISH_EVENT_NAME = "status";

ClickButton button(BUTTONPIN, LOW);
ContactSwitch switchButton(SWITCHPIN);

bool isStarting = true;

Setting setting;

VisualLEDs visualLEDs(LED_CLOCKPIN, LED_DATAPIN);

PinManager pinManager(&display, &setting);

OpenOperation openOperation(RELAYPIN, &visualLEDs);

void setup() {

    // Setup switchButton timers (all in milliseconds / ms)
    switchButton.debounceTime = 1000;   // Debounce timer in ms

    // Setup button timers (all in milliseconds / ms)
    button.debounceTime   = 20;   // Debounce timer in ms
    button.multiclickTime = 250;  // Time limit for multi clicks
    button.longClickTime  = 1000; // time until "held-down clicks" register

    // Do not open the box when system is turned on
    digitalWrite(RELAYPIN, LOW);

    // Setup the lcd
    display.setup();

    // Register the status of the box to Particle Cloud
    switchState = digitalRead(SWITCHPIN);
    Particle.variable("switchState", switchState);

    // Register the configureTimeZone as a cloud function
    Particle.function("timeZone", configureTimeZone);

    // Register the configureOpenTime as a cloud function
    Particle.function("openTime", configureOpenTime);

    // Register the configureReminderTime as a cloud function
    Particle.function("reminderTime", configureReminderTime);

    // Register the startOpenOperation as a cloud function
    Particle.function("open", startOpenOperation);

    // Get the timeZone from EEPROM
    timeZone = setting.getTimeZone();
    Time.zone(timeZone);

    // Get the open time from EEPROM
    openTime = setting.getOpenTime();

    // Get the reminder time from EEPROM
    reminderTime = setting.getRemiderTime();

    Serial.begin(9600);

    clockTimer.start();
}

void loop() {
    syncTimeWithCloudIfNeeded();

    checkButtonState();
    checkSwitchState();

    checkPin();

    visualLEDs.updateNetworkStatus();

    if (requestOpeningOperation) {
        buzzer.startForOpenOperation();
        openOperation.start();
        requestOpeningOperation = false;
    }

    displayInformation();

    isStarting = false;
}

void checkPin() {
    PinManagerStatus status;
    if (pinManager.updateAuthenticationStatus(&status)) {
        switch (status) {
            case ENTERING:
                visualLEDs.showNotification(VisualLEDs::Color::blue, VisualLEDs::BlinkingDuration::duration_short, VisualLEDs::BlinkingPeriod::period_short);
                break;
            case CANCELLED:
                pinManager.stopAuthentication();
                break;
            case AUTHENTICATED:
                startOpenOperation("");
                break;
            case INVALID:
                visualLEDs.showNotification(VisualLEDs::Color::red, VisualLEDs::BlinkingDuration::duration_medium, VisualLEDs::BlinkingPeriod::period_very_long);
                break;
        }
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
        display.turnOn();
    }

    if (function == 2 || function == -2) { // Double (LONG) click
        pinManager.startAuthentication();
    }

    if (function == 3 || function == -3) { // TRIPLE (LONG) click
        startOpenOperation("");
    }
}

void checkSwitchState() {
    // Update button state
    switchButton.Update();

    bool currentState = switchButton.stableState;

    // Only display the status if state is changed
    if (currentState != switchState) {
        switchState = currentState;

        // Cancel entering pin process
        pinManager.stopAuthentication();

        requestDisplayStatus = true;
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

void displayStatus() {
    if (requestDisplayStatus) {

        // Publish the status to Particle Cloud
        publishData();

        if (display.showStatus(switchState)) {

            if (switchState) {
                buzzer.startForOpen();
            } else {
                buzzer.startForClose();
            }

            requestDisplayStatus = false;
        }
    }
}

void updateTime() {

    hour = Time.hour();
    minute = Time.minute();
    second = Time.second();

    display.updateTimerCounter();

    checkOpenCondition();

    checkReminderCondition();

    requestDisplayTime = true;
}

void displayTime() {
    if (requestDisplayTime) {
        if (!pinManager.isAuthenticating()) {
            // Format from C library: https://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-Time-String-Parsing.html
            String timeString = Time.format(Time.now(), "%T");
            if (display.showTime(timeString)) {
                requestDisplayTime = false;
            }
        }
    }
}

void displayReminderTime() {
    if (requestDisplayReminderTime) {

        if (display.showReminderTime(reminderTime)) {
            requestDisplayReminderTime = false;
        }

        buzzer.startForConfiguration();
    }
}

void displayConfigurationTime() {
    if (requestDisplayConfigurationTime) {

        if (display.showOpenTime(openTime)) {
            requestDisplayConfigurationTime = false;
            buzzer.startForConfiguration();
        }
    }
}

void displayInformation() {

    displayTime();

    displayConfigurationTime();

    displayReminderTime();

    if (!openOperation.isRunning()) {
        displayStatus();
    }
}

int startOpenOperation(String command) {

    visualLEDs.showNotification(VisualLEDs::Color::blue, VisualLEDs::BlinkingDuration::duration_very_long, VisualLEDs::BlinkingPeriod::period_short);

    requestOpeningOperation = true;

    return 1;
}

void checkOpenCondition() {
    if ((hour == openTime.hour) && (minute == openTime.minute) && (second == 0)) {
        requestOpeningOperation = true;
    }
}

// Reminder when the box is still open
void checkReminderCondition() {
    if (switchState == LOW) {
        // TODO: Reminder every 5 minutes when the box is still open
        if ((hour == reminderTime.hour) && (minute == reminderTime.minute) && (second == 0)) {
            buzzer.startForReminder();
        }
    }
}

int configureReminderTime(String time) {
    // Declare the variables of the parts of the String
    String hourString, minuteString;
    int hour, minute;
    int delimeterPosition = time.indexOf(",");

    if (delimeterPosition >= 0) {

        hourString = time.substring(0, delimeterPosition);
        minuteString = time.substring(delimeterPosition+1);
        hour = hourString.toInt();
        minute = minuteString.toInt();
        reminderTime = { 0, hour, minute };

        setting.setRemiderTime(reminderTime);

        requestDisplayReminderTime = true;

        visualLEDs.showNotification(VisualLEDs::Color::green, VisualLEDs::BlinkingDuration::duration_short, VisualLEDs::BlinkingPeriod::period_short);

        return 1;
    }
    return -1;
}

int configureOpenTime(String time) {
    // Declare the variables of the parts of the String
    String hourString, minuteString;
    int hour, minute;
    int delimeterPosition = time.indexOf(",");

    if (delimeterPosition >= 0) {

        hourString = time.substring(0, delimeterPosition);
        minuteString = time.substring(delimeterPosition+1);
        hour = hourString.toInt();
        minute = minuteString.toInt();
        openTime = { 0, hour, minute };

        setting.setOpenTime(openTime);

        requestDisplayConfigurationTime = true;

        visualLEDs.showNotification(VisualLEDs::Color::green, VisualLEDs::BlinkingDuration::duration_short, VisualLEDs::BlinkingPeriod::period_short);

        return 1;
    }
    return -1;
}

int configureTimeZone(String timeZoneString) {
    // Declare the variables of the parts of the String
    float timeZone = timeZoneString.toFloat();

    setting.setTimeZone(timeZone);

    Time.zone(timeZone);

    // Force the display to turn on
    display.turnOn();

    // Display the time
    requestDisplayTime = true;

    visualLEDs.showNotification(VisualLEDs::Color::green, VisualLEDs::BlinkingDuration::duration_short, VisualLEDs::BlinkingPeriod::period_short);

    return 1;

}

void publishData() {
	char buf[256];
	sprintf(buf, "{\"status\":%d,\"deviceID\":\"%s\"}", switchState, System.deviceID().c_str());
	Particle.publish(PUBLISH_EVENT_NAME, buf, PRIVATE);
}
