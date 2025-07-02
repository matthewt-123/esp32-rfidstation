#include "sensors.h"
#include "common.h"


/********
 * int read_sensor(): return a random value
 *  this is meant to simulate reading a sensor value over i2c to allow for ble implementation
*********/
int read_sensor() 
{
    return 58;
}

char *read_rfid()
{
    char *retVal = "6b00db41-b6f2-4aa4-bb4f-ad73b3804fe3";
    return retVal;
}