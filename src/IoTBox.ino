/*
 * Project IoTBox
 * Description:
 * Author: Peacemoon
 * Date: 17.09.2017
 */

 STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
 SYSTEM_MODE(AUTOMATIC)
 SYSTEM_THREAD(ENABLED);

#include "Grove_LCD_RGB_Backlight.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)

#define BUTTONPIN D2 // Button reserved for further usage
#define BUZZERPIN D3 // Buzzer reserved for further usage
#define RELAYPIN D4 // Relay to drive the electromagnetic lock.
#define SWITCHPIN D6 // Switch for "LOCKED" or "OPEN" status.
#define LEDPIN D7 // Status LED.

/*************************************************
 * Public Constants
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

unsigned long lastSync = millis();

rgb_lcd lcd;

int switchState = LOW; // HIGH = locked, LOW = open
volatile bool requestDisplayStatus = true;
volatile bool requestDisplayTime = false;
volatile bool requestDisplayConfigurationTime = true;
volatile bool requestOpeningOperation = false;
volatile bool isLCDBusy = false;


int melodyOpen[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,
};
int noteDurationsOpen[] = {
    4, 4, 8, 4, 4,
    4, 4, 8, 4, 4,
    4, 4, 8, 4, 4,
    4, 4, 12, 4,
};

int melodyClose[] = {
    NOTE_G3, NOTE_C3, NOTE_DS3, NOTE_F3,
    NOTE_G3 ,NOTE_C3, NOTE_DS3, NOTE_F3,
    NOTE_D3
};
int noteDurationsClose[] = {
    12, 12, 4, 4,
    8, 8, 4, 4,
    12,
};

int melodyConfiguration[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0,  0,
    NOTE_G6, 0, 0, 0,
    NOTE_C7,
};
int noteDurationsConfiguration[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12,
};

class OpenOperation {
  public:

    OpenOperation() {
        _timer = new Timer(2000, (void (*)())&OpenOperation::callback, true);
    }

    void start() {
        // TODO: Delay 2 seconds after 2 consecutive open operations
        if (!isRunning()) {
            // Only open the box when it is closed
            if (switchState == HIGH) {
                digitalWrite(RELAYPIN, HIGH);
                _timer->reset();
            }
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

Timer clockTimer(1000, updateTime); // timer used to display the clock on the lcd display

OpenOperation openOperation;

volatile int hour;
volatile int minute;
volatile int second;

int configureTime(String time);
void syncTimeWithCloudIfNeeded();

struct ConfigurationTime {
    uint8_t version;
    int hour;
    int minute;
};

ConfigurationTime configurationTime;

const char *PUBLISH_EVENT_NAME = "status";

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
    switchState = digitalRead(SWITCHPIN);
    Particle.variable("switchState", switchState);

    // Register the configureTime as a cloud function
    Particle.function("time", configureTime);

    // Register the startOpenOperation as a cloud function
    Particle.function("open", startOpenOperation);

    // Get the configured time from EEPROM, set it to default if not saved
    EEPROM.get(0, configurationTime);
    if (configurationTime.version != 0) {
        // EEPROM was empty -> initialize myObj
        configurationTime = { 0, 7, 0 };
    }

    Serial.begin(9600);

    clockTimer.start();
}

void loop() {
    syncTimeWithCloudIfNeeded();

    if (requestOpeningOperation) {
        digitalWrite(LEDPIN, HIGH);
        startBuzzerForOpenOperation();
        openOperation.start();
        requestOpeningOperation = false;
    } else {
        digitalWrite(LEDPIN, LOW);
    }

    displayInformation();
}

void syncTimeWithCloudIfNeeded() {
    if (millis() - lastSync > ONE_DAY_MILLIS) {
      // Request time synchronization from the Particle Cloud
      Particle.syncTime();
      waitUntil(Particle.syncTimeDone);
      lastSync = millis();
    }
}

void startBuzzerForOpenOperation() {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000/noteDurationsOpen[thisNote];
        tone(BUZZERPIN, melodyOpen[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZERPIN);
    }
}

void startBuzzerForClose() {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000/noteDurationsClose[thisNote];
        tone(BUZZERPIN, melodyClose[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZERPIN);
    }
}

void startBuzzerForConfiguration() {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        int noteDuration = 1000/noteDurationsConfiguration[thisNote];
        tone(BUZZERPIN, melodyConfiguration[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZERPIN);
    }
}

void displayStatus() {
    if (requestDisplayStatus) {

        // Publish the status to Particle Cloud
        publishData();

        if (!isLCDBusy) {
            isLCDBusy = true;
            if (switchState == HIGH) {
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print("CLOSED");

                startBuzzerForClose();
            } else {
                lcd.setCursor(0,1);
                lcd.print("               ");
                lcd.setCursor(0,1);
                lcd.print("OPEN");
            }
            isLCDBusy = false;
            requestDisplayStatus = false;
        }
    }
}

void updateTime() {

    hour = Time.hour();
    minute = Time.minute();
    second = Time.second();

    //requestDisplayTime = true;

    checkOpenCondition();
}

void displayTime() {
    if (!requestDisplayConfigurationTime) {
        if (requestDisplayTime) {
            if (!isLCDBusy) {
                isLCDBusy = true;
                lcd.setCursor(0,0);
                lcd.print("          ");
                lcd.setCursor(0,0);
                // Format from C library: https://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-Time-String-Parsing.html
                lcd.print(Time.format(Time.now(), "%T"));
                isLCDBusy = false;

                requestDisplayTime = false;
            }
        }
    }
}

void displayConfigurationTime() {
    if (requestDisplayConfigurationTime) {

        startBuzzerForConfiguration();

        if (!isLCDBusy) {
            isLCDBusy = true;
            lcd.setCursor(11,0);
            lcd.print(configurationTime.hour);

            lcd.setCursor(13,0);
            lcd.print(":");

            lcd.setCursor(14,0);
            lcd.print(configurationTime.minute);
            isLCDBusy = false;

            requestDisplayConfigurationTime = false;
        }
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
    startOpenOperation("");
}

int startOpenOperation(String command) {
    requestOpeningOperation = true;
    return 1;
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

void publishData() {
	char buf[256];
	sprintf(buf, "{\"status\":%d,\"deviceID\":\"%s\"}", switchState, System.deviceID().c_str());
	Particle.publish(PUBLISH_EVENT_NAME, buf, PRIVATE);
}
