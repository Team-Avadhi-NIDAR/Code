#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#define TAG "ESP_NOW_LR_EXAMPLE"

// Receiver’s MAC address (to be updated with a valid unicast MAC)
uint8_t rx_mac[] = {0xA8, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; // Changed 0xAA to 0xA8 for unicast

typedef struct 
{
    float latitude;
    float longitude;
} LocationData;

LocationData location;

void esp_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(TAG, "Message Delivered to %02X:%02X:%02X:%02X:%02X:%02X",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    } else {
        ESP_LOGE(TAG, "Message failed to deliver");
    }
}

void wifi_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));

    wifi_config_t wifi_config = {
        .sta = {
            .channel = 1, // Explicitly set channel
            .threshold.authmode = WIFI_AUTH_OPEN,
            .listen_interval = 0,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(84));

    // Log transmitter’s MAC address
    uint8_t tx_mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, tx_mac));
    ESP_LOGI(TAG, "Transmitter MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
             tx_mac[0], tx_mac[1], tx_mac[2], tx_mac[3], tx_mac[4], tx_mac[5]);

    ESP_LOGI(TAG, "WiFi Initialized in LR Mode");
}

void init_esp_now() {
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW Init failed: %s", esp_err_to_name(ret));
        return;
    }
    ESP_ERROR_CHECK(esp_now_register_send_cb(esp_now_send_cb));

    esp_now_peer_info_t peer_info = {
        .channel = 1,
        .ifidx = ESP_IF_WIFI_STA,
        .encrypt = false,
    };
    memcpy(peer_info.peer_addr, rx_mac, 6);
    esp_err_t add_peer_result = esp_now_add_peer(&peer_info);
    if (add_peer_result == ESP_OK) {
        ESP_LOGI(TAG, "Peer added successfully: %02X:%02X:%02X:%02X:%02X:%02X",
                 rx_mac[0], rx_mac[1], rx_mac[2], rx_mac[3], rx_mac[4], rx_mac[5]);
    } else {
        ESP_LOGE(TAG, "Failed to add peer: %s", esp_err_to_name(add_peer_result));
    }

    ESP_LOGI(TAG, "ESP-NOW Initialized successfully");
}

void app_main(void) {
    wifi_init();
    init_esp_now();
    
    while (1) {
        location.latitude = ((rand() % (9001 - (-9000) + 1)) + (-9000)) / 100.0;
        location.longitude = ((rand() % (18001 - (-18000) + 1)) + (-18000)) / 100.0;

        // Check if peer exists before sending
        esp_now_peer_info_t peer_check;
        esp_err_t peer_status = esp_now_get_peer(rx_mac, &peer_check);
        if (peer_status != ESP_OK) {
            ESP_LOGE(TAG, "Peer not found before sending: %s", esp_err_to_name(peer_status));
        }

        esp_err_t result = esp_now_send(rx_mac, (uint8_t *)&location, sizeof(location));
        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Sent latitude: %.2f, longitude: %.2f", location.latitude, location.longitude);
        } else {
            ESP_LOGE(TAG, "Error sending message: %s", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}