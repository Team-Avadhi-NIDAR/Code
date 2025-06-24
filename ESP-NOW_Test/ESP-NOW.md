# ESP-NOW 

ESP-NOW is a fast, low-power, peer-to-peer wireless communication protocol by Espressif for ESP32 and ESP8266 devices, allowing direct data exchange without Wi-Fi or Bluetooth connections.

## Flow 

```mermaid
graph TD
    A[Initialize ESP-NOW] --> B[Register Devices]
    B --> C[Set Role: Controller/Responder]
    C --> D[Add Peer]
    D --> E[Send Data]
    E --> F{Data Received?}
    F -->|Yes| G[Process Data]
    F -->|No| H[Retry/Handle Error]
    G --> I[Optional: Send Response]
    H --> I
    I --> J[Continue/End]
```

## ESP-NOW Testing Code

1. [21/06/2025 & 22/06/2025](./test_21-06-2025/21-06-25.md)
2. [Long Range Testing](./ESP-NOW_LR/ESP-NOW_LR.md)
