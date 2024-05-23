#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WizMote.h>

#include <configuration.h>

#define WIZMOTE_BUTTON_ON          1
#define WIZMOTE_BUTTON_OFF         2
#define WIZMOTE_BUTTON_NIGHT       3
#define WIZMOTE_BUTTON_ONE         16
#define WIZMOTE_BUTTON_TWO         17
#define WIZMOTE_BUTTON_THREE       18
#define WIZMOTE_BUTTON_FOUR        19
#define WIZMOTE_BUTTON_BRIGHT_UP   9
#define WIZMOTE_BUTTON_BRIGHT_DOWN 8

#define DELAY_MS 60000

typedef struct wizstructure {                                             // Data to send by ESP-Now
  byte program;  // Seems to always be set to 145 when power button is pressed and 129
                 // for any other button which suggests it might be used in the programming
                 // sequence for bulbs which make you press the power button.
 
  byte seq;   // Incremental number sent each time a button is pressed. This is done in
              // order to prevent cloning the signal and and making sure each button press
              // is uniquely identified. the 4 bytes need to be reverse and combined to make a long.
 
  byte byte3;
  byte byte4;
  byte byte5;
  byte byte6 = 32;  // Unknown, but this seems to always have a value of 32
  byte button;      // Identifies which button is being pressed
  byte byte8 = 1;   // Unknown, but this seems to always have a value of 1
  byte byte9 = 100; // Unnkown, but this seems to always have a value of 100
  byte byte10;
  byte byte11;
  byte byte12;
  byte byte13;
} wizstructure;


wizstructure WizMoteMessage;

//hardcoded some test jsons
const char JsonGoalMessage[] = "{\"ps\":1}";
const char JsonChillMessage[] = "{\"ps\":3}";
const char JsonOffMessage[] = "{\"on\":false,\"bri\":10}";

WizMoteClass WizMote;

void on_data_sent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Data was sent");
}

void setup() {

  // Initialize Serial communication
  Serial.begin(BAUD_RATE);
  Serial.println();

  // Initialize the WizMote
  WizMote.begin();

  // Initialize ESP-NOW
  WizMote.initializeEspNow(WIFI_CHANNEL);

  // Register send callback
  WizMote.registerSendCallback(on_data_sent);

  Serial.print("Sending: ");
  Serial.println(JsonGoalMessage);
  WizMote.broadcast((uint8_t *)&JsonGoalMessage, 8); //hardcoded the length

  delay(DELAY_MS);

  Serial.print("Sending: ");
  Serial.println(JsonChillMessage);
  WizMote.broadcast((uint8_t *)&JsonChillMessage, 8);

  delay(DELAY_MS);

  Serial.print("Sending: ");
  Serial.println(JsonOffMessage);
  WizMote.broadcast((uint8_t *)&JsonOffMessage, 21);

}

void loop() {
  ESP.deepSleep(ESP.deepSleepMax());
}