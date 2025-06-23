#include <Arduino.h>
#include <WiFi.h>
#include "ESPNowW.h"
// put function declarations here:

#define ledPin 2

uint8_t rx_mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

typedef struct
{
  float latitude;
  float longitude;
} LocationData;

LocationData location;



void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  randomSeed(analogRead(0));

  WiFi.mode(WIFI_MODE_STA);

  WiFi.disconnect();

  ESPNow.init();
  ESPNow.add_peer(rx_mac);

}

void loop() {
  // put your main code here, to run repeatedly:
  location.latitude = random(-9000, 9001)/100;
  location.longitude = random(-18000, 18001)/100;

  Serial.printf("Latitude: %.2f\n", location.latitude);
  Serial.printf("Longitude: %.2f\n", location.longitude);

  ESPNow.send_message(rx_mac, (uint8_t *)&location, sizeof(location));
  digitalWrite(ledPin, 1);
  delay(1000);
}

