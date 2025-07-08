/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "common.h"
#include "ble.h"
#include "ota.h"

static const char* LOG_TAG = "MAIN";
void loadNvsIntoRAM();
void _loadStrValue(nvs_handle_t nvsHandle, 
    const char* key, char *value, size_t val_len, char *defaultVal);
void _loadBoolValue(nvs_handle_t nvsHandle, 
    const char* key, bool *value, bool defaultVal);

void app_main(void) 
{
    /* Initialize NVS */
    loadNvsIntoRAM();

    /* Initialize BLE and pin to Core 0 */
    configure_ble();
    nimble_port_freertos_init(ble_host_task);
    xTaskCreate(vTaskSendNotification, "vTaskSendNotification", 4096, NULL, 2, &xHandle);
    
    if (wifiEnable) wifi_config();
    #ifdef CONFIG_OTA_ENABLE
    xTaskCreate(ota_task, "vTaskSendNotification", 4096, NULL, 3, &xHandle);
    #endif
}

void loadNvsIntoRAM()
{
    /* NVS: Initialize */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Open NVS */
    nvs_handle_t nvsHandle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvsHandle));

    /* Read Values */
    _loadStrValue(nvsHandle, "wifi_ssid", ssid, sizeof(ssid), CONFIG_ESP_WIFI_SSID);
    _loadStrValue(nvsHandle, "deviceName", deviceName, MAX_DEVICE_NAME_LENGTH, "Default ESP32 Device");
    _loadStrValue(nvsHandle, "wifi_psk", psk, WIFI_PSK_MAX, CONFIG_ESP_WIFI_PASSWORD);
    _loadStrValue(nvsHandle, "mqttEndpt", mqttEndpt, MAX_MQTT_ENDPT_LENGTH, "");

    _loadBoolValue(nvsHandle, "wifiEnable", &wifiEnable, true);
    _loadBoolValue(nvsHandle, "rfidEnable", &rfidEnable, false);
    _loadBoolValue(nvsHandle, "tempEnable", &tempEnable, false);
    _loadBoolValue(nvsHandle, "pressureEnable", &pressureEnable, false);
    _loadBoolValue(nvsHandle, "humidityEnable", &humidityEnable, false);

    nvs_close(nvsHandle);
}

void _loadStrValue(nvs_handle_t nvsHandle, const char* key, char *value, size_t val_len, char *defaultVal)
{
    esp_err_t err;
    err = nvs_get_str(nvsHandle, key, value, &val_len);
    ESP_LOGI(LOG_TAG, "%s stored: \"%s\"", key, value);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        strlcpy(value, defaultVal, val_len);
    }
}

void _loadBoolValue(nvs_handle_t nvsHandle, const char* key, bool *value, bool defaultVal)
{
    esp_err_t err;
    int8_t output;
    err = nvs_get_i8(nvsHandle, key, &output);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *value = defaultVal;
    }
    else {
        *value = (output != 0);
    }
}