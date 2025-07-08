/*******************
common.h: includes used across all components
*******************/

#ifndef OTA_H
#define OTA_H
#include "esp_event.h"
#include "esp_http_client.h"


void wifiLogin(const char *ssid, const char *password);
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_config();

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void ota_check();
void ota_task(void *arg);

extern EventGroupHandle_t wifiEventGroup;
#endif //OTA_H