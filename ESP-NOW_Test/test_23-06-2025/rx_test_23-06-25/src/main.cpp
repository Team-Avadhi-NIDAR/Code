#include <Arduino.h>
#include <WiFi.h>
#include "ESPNowW.h"
#include <esp_wifi.h>
#define ledPin 2

uint8_t mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// put function declarations here:

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
  else{
    Serial.println("Recieved Data size mismatch");
  }
  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  delay(1000);

  WiFi.mode(WIFI_STA);

  esp_err_t result = esp_wifi_set_mac(WIFI_IF_STA, mac);

  /*if (result == ESP_OK)
  {
    Serial.println("New Mac Address set successfully");
  }
  else {
    Serial.println("MAC address not set");
  }

  Serial.print("MAC Address: "); Serial.print(WiFi.macAddress());*/

  ESPNow.init();

  ESPNow.reg_recv_cb(OnDataRecv);
  
}

void loop() {
  if (dataRecvd)
  {
    Serial.printf("recieved Latitude: %.2f\n", location.latitude);
    Serial.printf("recieved Longitude: %.2f\n", location.longitude);
    dataRecvd = false;
  }

  delay(1000);
  
}
