
#include <stdio.h>
#include <inttypes.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "nvs_flash.h"
#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

// GAP generic acces profile - advertising, name, connection
// GATT generic attribute profile - data exchange



#define UART_PORT UART_NUM_2
#define UART_TX_PIN GPIO_NUM_16 
#define UART_RX_PIN GPIO_NUM_17 

#define UART_BUFFER_SIZE 1024

// UART: RX: D17    TX: D16


static const char* TAG = "BLE_TEST";
static const char* DEVICE_NAME = "SUPER_TERMOMETR";

static char temp_from_stm32[20] = "NULL";
static uint16_t temp_value_handle;

static uint16_t connection_handle = BLE_HS_CONN_HANDLE_NONE;
static uint8_t addr_type;

static const ble_uuid128_t temp_service_uuid = BLE_UUID128_INIT (
    0x12, 0x34, 0xAB, 0xFF,
    0x00, 0x11, 0x69, 0x12,
    0x21, 0x03, 0x99, 0x02,
    0xdd, 0xdc, 0xee, 0xa2
);
static const ble_uuid128_t temp_characteristic_uuid = BLE_UUID128_INIT (
    0x16, 0xf4, 0xA2, 0xdF,
    0xa0, 0x11, 0xa3, 0xD0,
    0xd1, 0x03, 0x90, 0xA2,
    0xfd, 0xdc, 0xee, 0xB2
);
static int ble_gap_event(struct ble_gap_event* event, void* arg) ;

static void start_advertising(void) {
    int rc;
    struct ble_gap_adv_params advertisement_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND, // anyone can connect
        .disc_mode = BLE_GAP_DISC_MODE_GEN, // generaly discoverable        
    };

    struct ble_hs_adv_fields adv_fields;
    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.name = (uint8_t*)DEVICE_NAME;
    adv_fields.name_len = strlen(DEVICE_NAME);
    adv_fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&adv_fields);

    rc = ble_gap_adv_start(addr_type, NULL, BLE_HS_FOREVER, &advertisement_params, ble_gap_event, NULL);

    if (rc == 0) {
      ESP_LOGI(TAG, "advertising started! \n");
    } else {        
      ESP_LOGI(TAG, "advertising failed! \n");
    }
}

static int ble_gap_event(struct ble_gap_event* event, void* arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            if (event->connect.status == 0) {
                connection_handle = event->connect.conn_handle;
                ESP_LOGI(TAG, "connected \n");
            }
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT: {
            connection_handle = BLE_HS_CONN_HANDLE_NONE;
            ESP_LOGI(TAG, "DISconnected \n");
            start_advertising();
            break;
        }
    }
    return 0;
}

static int temp_read_callback(uint16_t  connection_handle, uint16_t attribute_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    os_mbuf_append(ctxt->om, temp_from_stm32, strlen(temp_from_stm32));

    return 0;
}

static const struct ble_gatt_svc_def gatt_services[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &temp_service_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
             {
                .uuid = &temp_characteristic_uuid.u,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .access_cb = temp_read_callback,
                .val_handle = &temp_value_handle
             },
             {
                0
             }
        }
    }, 
    {
        0
    }
};



void UART_initialize() {
    uart_config_t uart_config = {
        .baud_rate =115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, 0));
}

void ble_host_task(void* param) {

    ESP_LOGI(TAG, "ble host task started \n");
    nimble_port_run();
}



static void ble_app_on_sync(void) {
    int rc;

    rc = ble_hs_id_infer_auto(0, &addr_type);
    if (rc != 0) {
      ESP_LOGI(TAG, "addres id infer auto failed!\n");
      return;
    }
    start_advertising();
}


void app_main(void)
{
    ESP_LOGI(TAG, "Hello world!\n");


    UART_initialize();

    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    nimble_port_init();


    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_services);
    ble_gatts_add_svcs(gatt_services);
    ESP_LOGI(TAG, "HTEMP handle: %u\n", temp_value_handle);


    ble_svc_gap_device_name_set(DEVICE_NAME);




    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(ble_host_task);


    //uint8_t rx_buffer[128];


    while (true) {
        int len = uart_read_bytes(UART_PORT, temp_from_stm32, 20, pdMS_TO_TICKS(1000));
        if (len > 0) {
            printf("received %d bytes \n", len);
            for (int i = 0; i < len; i++) {
                printf("%c", temp_from_stm32[i]);
            }
            printf("\n");
            if (connection_handle != BLE_HS_CONN_HANDLE_NONE) {
                ble_gatts_notify(connection_handle, temp_value_handle); 
            }

        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

   
    esp_restart();
}
