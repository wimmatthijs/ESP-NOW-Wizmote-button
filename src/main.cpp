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

#define DELAY_MS 50000

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

WizMoteClass WizMote;

void on_data_sent(uint8_t *mac_addr, uint8_t sendStatus) {

  // Turn the WizMote off once the packet has been sent
  //WizMote.powerOff();
  Serial.println("Data was sent");
}

void setup() {

  // Initialize Serial communication
  Serial.begin(BAUD_RATE);

  // Initialize the WizMote
  WizMote.begin();

  // Initialize ESP-NOW
  WizMote.initializeEspNow(WIFI_CHANNEL);

  // Register send callback
  WizMote.registerSendCallback(on_data_sent);


  // NO need to turn it on, the preset already does that for you

  // Prepare  message
  // WizMoteMessage.program=129;
  // WizMoteMessage.seq[0] = WizMote.nextSequenceNumber();
  // WizMoteMessage.byte6=32;
  // WizMoteMessage.button=WIZMOTE_BUTTON_ON;
  // WizMoteMessage.byte8=1;
  // WizMoteMessage.byte9=100;
  // WizMoteMessage.byte10 = (uint32_t)0;
  // for (int i=0;i<5;i++){
  //   WizMote.broadcast((uint8_t *)&WizMoteMessage, sizeof(wizstructure));
  //   delay(100);
  // }
  
  // Prepare message
  //The sequence number never did work that well for me, but WLED accepts any number that is not the same as previous
  //the sequence numnber needs to be byte inverted for some reason, an attempt to this happens in the .nextSequenceNumber function
  WizMoteMessage.program=129;
  WizMoteMessage.seq = WizMote.nextSequenceNumber();
  WizMoteMessage.byte6=32;
  WizMoteMessage.button=WIZMOTE_BUTTON_ONE;
  WizMoteMessage.byte8=1;
  WizMoteMessage.byte9=100;
  WizMoteMessage.byte10 = (uint32_t)0;

  //sending it five times to guarantee more success, i found this was necessary when you also have an SSID set in WLED.
  //With an SSID set in WLED it seems to do some scans or something which makes ESP NOW packets be lost
  //For wearables my config was ONLY AP Mode Always on channel 1 (see /include/configuration.h)
  Serial.println((uint32_t)WizMoteMessage.seq);
  for (int i=0;i<5;i++){
    WizMote.broadcast((uint8_t *)&WizMoteMessage, sizeof(wizstructure));
    delay(100);
  }
  delay(DELAY_MS);

  // Prepare Last message
  WizMoteMessage.program=129;
  WizMoteMessage.seq = WizMote.nextSequenceNumber();
  WizMoteMessage.byte6=32;
  WizMoteMessage.button=WIZMOTE_BUTTON_OFF;
  WizMoteMessage.byte8=1;
  WizMoteMessage.byte9=100;
  WizMoteMessage.byte10 = (uint32_t)0;
  for (int i=0;i<5;i++){
    WizMote.broadcast((uint8_t *)&WizMoteMessage, sizeof(wizstructure));
    delay(100);
  }
}

//I am using only the reset button for one dedicated function so i put everything to sleep to save battery
void loop() {
  ESP.deepSleep(ESP.deepSleepMax());
}