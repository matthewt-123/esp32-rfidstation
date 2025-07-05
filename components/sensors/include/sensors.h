/*******************
ble.h: providing API to communicate with BLE 
*******************/

#ifndef SENSORS_H
#define SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

int read_sensor();
char *read_rfid();

#ifdef __cplusplus
}
#endif

#endif