#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "ESPNowW.h"

//uint8_t mac_addr[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

typedef struct
{
  float latitude;
  float longitude;
} LocationData;

LocationData location;
bool dataRecvd = false;


void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  if (data_len == sizeof(LocationData))
  {
    memcpy(&location, data, sizeof(LocationData));
    dataRecvd = true;
  }
  else
  {
    Serial.println("Recieved data size mismatch");
  }
  
  
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
  {
    //Wait for serial to initialize
  }
  
  WiFi.mode(WIFI_STA);

  //esp_wifi_set_mac(WIFI_IF_STA, mac_addr);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  ESPNow.init();

  ESPNow.reg_recv_cb(OnDataRecv);

}

void loop() {
  if (dataRecvd)
  {
    Serial.print("Recieved Latitude: "); Serial.print(location.latitude, 2);
    Serial.println();
    Serial.print("Recieved Longitude: "); Serial.print(location.longitude, 2);
    Serial.println();
    dataRecvd = false;
  }

  delay(1000);
  
}
