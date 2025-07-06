#include "common.h"

/* RUNTIME GLOBAL VARIABLES: Users can change during runtime */
char deviceName[MAX_DEVICE_NAME_LENGTH + 1] = {0};
char ssid[WIFI_SSID_MAX + 1] = {0};
char psk[WIFI_PSK_MAX + 1] = {0};
extern char mqttEndpt[MAX_MQTT_ENDPT_LENGTH + 1] = {0};
extern bool wifiEnable = true;
extern bool rfidEnable = false;
extern bool tempEnable = false;
extern bool pressureEnable = false;
extern bool humidityEnable = false;

/* FIXED GLOBAL VARIABLES: Must be set during compile time. 
    Users should not expect to change these */
extern int16_t sampleRate = 10; //Hz, frequency of sample readings for every sensor