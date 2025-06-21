#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ESPNowW.h"

uint8_t rx_mac[] = {0x10, 0x52, 0x1C, 0x67, 0x78, 0xF4};

typedef struct
{
  float latitude;
  float longitude;
} LocationData;

LocationData location;


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));     //Initializing Random Seed

  Serial.println("EspNow Test");

  WiFi.mode(WIFI_STA);

  WiFi.disconnect();
  ESPNow.init();
  ESPNow.add_peer(rx_mac);

}

void loop() {
  location.latitude = random(-9000, 9001)/100.0;
  location.longitude = random(-18000, 18001)/100.0;

  Serial.printf("Sending Latitude: %.2f\n", location.latitude);
  Serial.printf("Sending Longitude: %.2f\n", location.longitude);

  ESPNow.send_message(rx_mac, (uint8_t*)&location, sizeof(location));

  delay(1000);
  
}

// put function definitions here:
