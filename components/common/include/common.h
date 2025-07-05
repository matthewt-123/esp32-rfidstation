/*******************
common.h: includes used across all components
*******************/

#ifndef COMMON_H
#define COMMON_H



/* Includes */
/* STD APIs */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* ESP APIs */
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

/* FreeRTOS APIs */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* Interrupt Flag*/
extern bool notify_state;
extern TaskHandle_t xHandle;
#endif //COMMON_H