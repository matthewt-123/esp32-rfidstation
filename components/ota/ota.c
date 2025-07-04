/* Includes */
#include "esp_wifi.h"
#include "common.h"
#include "esp_system.h"
#include "ota.h"

/* Defines */
#define WIFI_ENABLE
#define TAG "esp_wifi"

/* Global Variables */
int retry_num = 0;


#ifdef WIFI_ENABLE
void wifi_config()
{
    /* Initialize network stack */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    /* Initialize WiFi and pin to core 0*/
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.wifi_task_core_id = 0;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    /* Add callbacks for WiFi events and on obtaining IP */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id
                                                    ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip
                                                    ));
    
    /* Connect to WiFi */
    const char *ssid = CONFIG_ESP_WIFI_SSID;
    const char *password = CONFIG_ESP_WIFI_PASSWORD;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifiLogin(ssid, password);
    ESP_ERROR_CHECK(esp_wifi_start());
}

/* Logging and retry for WiFi task*/
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    // connect to wifi on start
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) esp_wifi_connect();

    //retry connections if connection fails
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_num < CONFIG_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(TAG, "retrying connection to AP");
        } else {
            ESP_LOGE(TAG, "failed to connect to AP");
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_num = 0;
    }
}

void wifiLogin(const char *ssid, const char *password)
{

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK, //defaults to WPA2
        },
    };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';
    
    /* Set config */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
}

void wifi_connect_task(void *pvParameters)
{
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect: %s", esp_err_to_name(err));
    }
    vTaskDelete(NULL);
}

#endif //WIFI_ENABLE