#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#define TAG "ESP_NOW_LR_RX"

// Custom MAC address for the receiver (valid unicast)
uint8_t custom_mac[] = {0xA8, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; // Changed 0xAA to 0xA8

typedef struct 
{
    float latitude;
    float longitude;
} LocationData;

LocationData location;

volatile bool dataRecvd = false;

void esp_now_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    if (data_len == sizeof(LocationData)) {
        memcpy(&location, data, data_len);
        dataRecvd = true;
        ESP_LOGI(TAG, "Received from MAC %02X:%02X:%02X:%02X:%02X:%02X",
                 recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
                 recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
    } else {
        ESP_LOGE(TAG, "Data length mismatch: expected %d, got %d", sizeof(LocationData), data_len);
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

    // Set custom MAC address
    ret = esp_wifi_set_mac(ESP_IF_WIFI_STA, custom_mac);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Set custom MAC address: %02X:%02X:%02X:%02X:%02X:%02X",
                 custom_mac[0], custom_mac[1], custom_mac[2],
                 custom_mac[3], custom_mac[4], custom_mac[5]);
    } else {
        ESP_LOGE(TAG, "Failed to set custom MAC address: %s", esp_err_to_name(ret));
    }

    // Verify MAC address
    uint8_t mac_verify[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, mac_verify));
    ESP_LOGI(TAG, "Current MAC address: %02X:%02X:%02X:%02X:%02X:%02X",
             mac_verify[0], mac_verify[1], mac_verify[2],
             mac_verify[3], mac_verify[4], mac_verify[5]);

    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));

    wifi_config_t wifi_config = {
        .sta = {
            .channel = 1,
            .threshold.authmode = WIFI_AUTH_OPEN,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(84));

    ESP_LOGI(TAG, "WiFi Initialized in LR Mode");
}

void init_esp_now() {
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW init failed: %s", esp_err_to_name(ret));
        return;
    }
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_recv_cb));
    ESP_LOGI(TAG, "ESP-NOW Initialized successfully");
}

void app_main(void) {
    wifi_init();
    init_esp_now();

    while (1) {
        if (dataRecvd) {
            ESP_LOGI(TAG, "Received Location: Latitude: %.2f, Longitude: %.2f", location.latitude, location.longitude);
            dataRecvd = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}