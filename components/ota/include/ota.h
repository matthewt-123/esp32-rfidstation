/*******************
common.h: includes used across all components
*******************/

#ifndef OTA_H
#define OTA_H
#include "esp_event.h"


void wifiLogin(const char *ssid, const char *password);
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_config();

#endif //OTA_H