/*******************
ble.h: providing API to communicate with BLE 
*******************/

#ifndef BLE_H
#define BLE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "common.h"
/* NimBLE stack APIs */
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

int configure_ble();
extern const struct ble_gatt_svc_def gatt_svr_svcs[];
int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
void esp_bt_advertise(void);
int esp_bt_gap_event(struct ble_gap_event *event, void *arg);
void esp_bt_print_conn_desc(struct ble_gap_conn_desc *desc);
int configure_ble();
void ble_app_on_sync(void);
void ble_host_task(void *param);


#ifdef __cplusplus
}
#endif

#endif