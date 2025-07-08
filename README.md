### ESP32 RFID Station (work in progress)

This ESP32 image is designed have basic BLE and WiFi capabilities. The end goal is to assist with home automation and environment monitoring. Interfacing with the [PN532 RFID module from HiLetGo](https://www.amazon.com/gp/product/B01I1J17LC/) and the [BME280 Environment Monitor from HiLetGo](https://www.amazon.com/gp/product/B01N47LZ4P) over SPI, it communicates real time data over BLE and/or WiFi to trigger webhooks or mobile alerts. It allows for configuration via a mobile app over BLE and supports OTA updates.

## Important Configuration Options in sdkconfig.defaults
- CONFIG_LWIP_LOCAL_HOSTNAME: WiFi broadcasting name
- ESP32 Networking Defaults;
  - Enable WiFi for webhook/OTA usage
  - WiFi SSID and Password for default WiFi login. This can be configured via the app. 
  - Maximum WiFi retry 
- ESP32 BLE Defaults: Default Bluetooth name
## OTA Information
- If enabled in settings, the ESP32 will automatically check for updates once per day (or once per minute if DEBUG mode is set to true). If a newer update is available on GitHub releases, it will update the device. 
