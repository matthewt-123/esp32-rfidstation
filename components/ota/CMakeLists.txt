idf_component_register(
  SRCS "ota.c"
  INCLUDE_DIRS "include"
  REQUIRES esp_wifi common app_update esp_http_client esp_https_ota
            esp_netif mbedtls esp_psram
)