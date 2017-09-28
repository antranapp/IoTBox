/*
 * Project IoTBox
 * Description:
 * Author: Peacemoon
 * Date: 17.09.2017
 */

#include "Grove_LCD_RGB_Backlight.h"

#define BUTTONPIN D2 // Button reserved for further usage
#define BUZZERPIN D3 // Buzzer reserved for further usage
#define RELAYPIN D4 // Relay to drive the electromagnetic lock.
#define SWITCHPIN D6 // Switch for "LOCKED" or "OPEN" status.
#define LEDPIN D7 // Status LED.

rgb_lcd lcd;

class OpenOperation {
  public:

    OpenOperation() {
        _timer = new Timer(3000, (void (*)())&OpenOperation::callback, true);
    }

    void start() {
        // TODO: Check if box is arelady open
        // TODO: Delay 2 seconds after 2 consecutive open operations
        if (!isRunning()) {
            lcd.setCursor(0,1);
            lcd.print("               ");
            lcd.setCursor(0,1);
            lcd.print("opening....");
            digitalWrite(RELAYPIN, HIGH);
            _timer->reset();
        }
    }

    void stop() {
        digitalWrite(RELAYPIN, LOW);
        _timer->dispose();
    }

    bool isRunning() {
        return _timer->isActive();
    }

  private:
    Timer* _timer;
    void callback(void) {
        digitalWrite(RELAYPIN, LOW);
    }
};

volatile int switchState = LOW;
volatile bool requestDisplayStatus = true;
volatile bool requestDisplayTime = false;
volatile bool requestDisplayConfigurationTime = true;
volatile bool requestOpeningOperation = false;

Timer clockTimer(1000, updateTime); // timer used to display the clock on the lcd display

OpenOperation openOperation;

volatile int hour;
volatile int minute;
volatile int second;

int configureTime(String time);

struct ConfigurationTime {
    uint8_t version;
    int hour;
    int minute;
};

ConfigurationTime configurationTime;

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

void setup() {
    // Ouput pins
    pinMode(LEDPIN, OUTPUT);
    pinMode(RELAYPIN, OUTPUT);

    // Interrupt pin
    pinMode(SWITCHPIN, INPUT);
    attachInterrupt(SWITCHPIN, updateSwitchState, CHANGE);
    pinMode(BUTTONPIN, INPUT_PULLDOWN);
    attachInterrupt(BUTTONPIN, updateButtonState, RISING);

    // Do not open the box when system is turned on
    digitalWrite(RELAYPIN, LOW);

    // Setup the lcd
    lcd.begin(16, 2); // Set up the LCD's number of columns and rows.
    lcd.setRGB(200, 200, 200); // Set up background backlight color.

    // Setup timezone
    Time.zone(+2.00);

    // Register the status of the box to Particle Cloud
    Particle.variable("switchState", (int32_t)switchState);

    // Register the configureTime as a cloud function
    Particle.function("time", configureTime);

    // Get the configured time from EEPROM, set it to default if not saved
    EEPROM.get(0, configurationTime);
    if (configurationTime.version != 0) {
        // EEPROM was empty -> initialize myObj
        configurationTime = { 0, 7, 0 };
    }

    clockTimer.start();
}

void loop() {
    if (requestOpeningOperation) {
        lcd.setCursor(0,1);
        lcd.print("               ");
        lcd.setCursor(0,1);
        lcd.print("will open....");

        digitalWrite(LEDPIN, HIGH);
        openOperation.start();

        requestOpeningOperation = false;
    } else {
        digitalWrite(LEDPIN, LOW);
    }

    displayInformation();
}

void displayStatus() {
    if (requestDisplayStatus) {
        if (switchState == HIGH) {
            lcd.setCursor(0,1);
            lcd.print("               ");
            lcd.setCursor(0,1);
            lcd.print("LOCKED");
        } else {
            lcd.setCursor(0,1);
            lcd.print("               ");
            lcd.setCursor(0,1);
            lcd.print("OPEN");
        }

        requestDisplayStatus = false;
    }
}

void updateTime() {

    hour = Time.hour();
    minute = Time.minute();
    second = Time.second();

    requestDisplayTime = true;

    checkOpenCondition();
}

void displayTime() {
    if (!requestDisplayConfigurationTime) {
        if (requestDisplayTime) {
            lcd.setCursor(0,0);
            lcd.print("          ");
            lcd.setCursor(0,0);
            // Format from C library: https://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-Time-String-Parsing.html
            lcd.print(Time.format(Time.now(), "%T"));

            requestDisplayTime = false;
        }
    }
}

void displayConfigurationTime() {
    if (requestDisplayConfigurationTime) {
        lcd.setCursor(11,0);
        lcd.print(configurationTime.hour);

        lcd.setCursor(13,0);
        lcd.print(":");

        lcd.setCursor(14,0);
        lcd.print(configurationTime.minute);

        requestDisplayConfigurationTime = false;
    }
}

void displayInformation() {
    displayTime();

    displayConfigurationTime();

    if (!openOperation.isRunning()) {
        displayStatus();
    }
}


void updateSwitchState() {
    switchState = digitalRead(SWITCHPIN);
    requestDisplayStatus = true;
}

void updateButtonState() {
    requestOpeningOperation = true;
}

void checkOpenCondition() {
    if ((hour == configurationTime.hour) && (minute == configurationTime.minute) && (second == 0)) {
        requestOpeningOperation = true;
    }
}

int configureTime(String time)
{
    // Declare the variables of the parts of the String
    String hourString, minuteString;
    int hour, minute;
    int delimeterPosition = time.indexOf(",");

    if (delimeterPosition >= 0) {

        hourString = time.substring(0, delimeterPosition);
        minuteString = time.substring(delimeterPosition+1);
        hour = hourString.toInt();
        minute = minuteString.toInt();
        configurationTime = { 0, hour, minute };

        EEPROM.put(0, configurationTime);

        requestDisplayConfigurationTime = true;

        return 1;
    }
    return -1;
}
