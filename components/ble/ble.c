#include "ble.h"
#include "sensors.h"
#include "esp_nimble_hci.h"

static const char* LOG_TAG = "BLE";
char name[14] = "Matthew-esp32";

/*********
 * Define UUIDs
 *********/

//!! Service UUID: 0161844a-240c-4dff-b992-6300459a902c
static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2c, 0x90, 0x9a, 0x45, 0x00, 0x63, 0x92, 0xb9, 0xff, 0x4d, 0x0c, 0x24, 0x4a, 0x84, 0x61, 0x01);

//!! Characteristic UUID: 85453fa7-2516-47b1-8ab5-b309cc46ce2b
static const ble_uuid128_t gatt_svr_chr_sensor_uuid =
    BLE_UUID128_INIT(0x2b, 0xce, 0x46, 0xcc, 0x09, 0xb3, 0xb5, 0x8a, 0xb1, 0x47, 0x16, 0x25, 0xa7, 0x3f, 0x45, 0x85);

//!! Characteristic UUID: ee4c2466-c729-4693-a087-d6082f04774a
static const ble_uuid128_t gatt_svr_chr_rfid_uuid =
    BLE_UUID128_INIT(0x4a, 0x77, 0x04, 0x2f, 0x08, 0xd6, 0x87, 0xa0, 0x93, 0x46, 0x29, 0xc7, 0x66, 0x24, 0x4c, 0xee);


char characteristic_value[50] = "";

/*********
 * ble_gatt_svc_def: defines GATT attribute table.
 *  All characteristics must be included in .characteristics
 *********/
int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc = 0;
    const ble_uuid_t *uuid = ctxt->chr->uuid;

    //Identify characteristic by UUID
    if (ble_uuid_cmp(uuid, &gatt_svr_chr_sensor_uuid.u) == 0)
    {
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR); //can only read this attribute
        //respond with sensor value from components/sensors
        int sensorVal = read_sensor();
        snprintf(characteristic_value, 50, "%d", sensorVal); 
        rc = os_mbuf_append(ctxt->om, &characteristic_value, sizeof characteristic_value);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        
    }
    /* Unknown characteristic */
    assert (0);
    return BLE_ATT_ERR_UNLIKELY;
};

/*********
 * ble_gatt_svc_def: defines GATT attribute table.
 *  All characteristics must be included in .characteristics
 *********/
const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                /* Characteristic: Sensor data */
                .uuid = &gatt_svr_chr_sensor_uuid.u,
                .access_cb = gatt_svr_chr_access,
                // .val_handle = &notification_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY
            },
            {
                0
            }
        },
    },
    {
        0
    }
};

 void esp_bt_advertise(void)
{
    struct ble_hs_adv_fields fields;
    struct ble_gap_adv_params adv_params;
    int rc;
    
    /* Set advertisement data */
    memset(&fields, 0, sizeof fields);
    fields.name_is_complete = 1;
    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        ESP_LOGE(LOG_TAG, "Error setting advertisement data: rc = %d", rc);
        return;
    }

    /* Begin advertising */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(BLE_GAP_DISC_MODE_GEN, NULL, BLE_HS_FOREVER, &adv_params, esp_bt_gap_event, NULL);
    if (rc != 0)
    {
        ESP_LOGE(LOG_TAG, "Error starting advertisement: rc = %d", rc);
        return;
    }
    ESP_LOGI(LOG_TAG, "Advertising started");
}

int esp_bt_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(LOG_TAG, "connection %s; status=%d ",
                       event->connect.status == 0 ? "established" : "failed",
                       event->connect.status);
            if (event->connect.status == 0)
            {
                rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
                assert(rc == 0);
                esp_bt_print_conn_desc(&desc);
            }
            if (event->connect.status != 0) {
                esp_bt_advertise(); //go back to advertising on disconnect
            }
            return 0;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(LOG_TAG, "disconnect; reason=%d", event->disconnect.reason);
            esp_bt_advertise();
            return 0;
        default:
            return 1;
    }
    return 2;
};

/**
 * Logs information about a connection
 */
 void esp_bt_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    ESP_LOGI(LOG_TAG, "handle=%d our_ota_addr_type=%d our_ota_addr=",
                desc->conn_handle, desc->our_ota_addr.type);
    ESP_LOGI(LOG_TAG, " our_id_addr_type=%d our_id_addr=",
                desc->our_id_addr.type);
    ESP_LOGI(LOG_TAG, " peer_ota_addr_type=%d peer_ota_addr=",
                desc->peer_ota_addr.type);
    ESP_LOGI(LOG_TAG, " peer_id_addr_type=%d peer_id_addr=",
                desc->peer_id_addr.type);
    ESP_LOGI(LOG_TAG, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                "encrypted=%d authenticated=%d bonded=%d\n",
                desc->conn_itvl, desc->conn_latency,
                desc->supervision_timeout,
                desc->sec_state.encrypted,
                desc->sec_state.authenticated,
                desc->sec_state.bonded);
};


/**************************
 * Configure BLE: 
**************************/
int configure_ble() 
{
    int rc;
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* BLE: Initialize HCI */
    if (nimble_port_init() != ESP_OK) return 1; //if NimBLE port init failed
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);

    /* BLE: Name Device */
    rc = ble_svc_gap_device_name_set("nimble-ble"); //!! Set the name of this device
    assert(rc == 0);

    ESP_LOGI(LOG_TAG, "configured with rc=%d", rc);
    return rc;
    // /* Start advertising */

    // struct ble_gap_adv_params adv_params;

    // /* set adv parameters */
    // memset(&adv_params, 0, sizeof(adv_params));
    // adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    // adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    // return ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER,
    //     &adv_params, adv_event, NULL);
};

void ble_app_on_sync(void)
{
    esp_bt_advertise();  // safe to advertise now
    ESP_LOGI(LOG_TAG, "advertising");
};

void ble_host_task(void *param) {
    nimble_port_run(); // This function never returns
    nimble_port_freertos_deinit();
};
