idf_component_register(
    SRCS "blink.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_driver_gpio
        esp_driver_i2s
        freertos
        esp_wifi
        esp_event
        nvs_flash
        esp_netif
        esp_http_client
        log
)