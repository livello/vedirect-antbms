#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_sleep.h"

bool mppt1=false,mppt2=false,bms=false;
uint8_t counter=0;

void set_sent(uint8_t item){
  switch(item){
    case 1: mppt1=true;
      break;
    case 2: mppt2=true;
      break;
    case 3: bms=true;
      break;
    default:
      break;
  }
}

void vWatchdogTask (void *pvParameters) {
  while(1){
    ESP_LOGD("WDG","mppt1: %d, mppt2: %d, bms: %d",mppt1,mppt2,bms);
    if(mppt1&&mppt2/*&&bms*/){
      ESP_LOGI("WDG","Sleeping...");
      esp_deep_sleep(20*1000*1000);
    } else if(counter > 60){
      ESP_LOGW("WDG","Restarting");
      esp_restart();
    }
    counter++;
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}