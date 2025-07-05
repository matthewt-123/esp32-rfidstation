idf_component_register(
  SRCS "ota.c"
  INCLUDE_DIRS "include"
  REQUIRES esp_wifi common app_update
)