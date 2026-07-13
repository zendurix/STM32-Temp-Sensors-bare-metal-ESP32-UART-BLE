idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_uart
                       REQUIRES esp_driver_gpio
                       REQUIRES bt nvs_flash)
