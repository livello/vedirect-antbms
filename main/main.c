#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "https_request.h"
#include "emoncms_request.h"
#include "mppt.h"
#include "watchdog.h"
#include "bluetooth.h"


void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    wifi_init();
    
    xTaskCreate(vWatchdogTask, "watchdog_task", 1024*2, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(vSerial_read_task, "mppt1_task", 8192, (void *)true, tskIDLE_PRIORITY, NULL);
    vTaskDelay(200/portTICK_PERIOD_MS);
    xTaskCreate(vSerial_read_task, "mppt2_task", 8192, (void *)false, tskIDLE_PRIORITY, NULL);
    vTaskDelay(200/portTICK_PERIOD_MS);
    bluetoothInit();
}
