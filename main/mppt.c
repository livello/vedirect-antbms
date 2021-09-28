#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "vedirect.h"
#include "https_request.h"
#include "emoncms_request.h"
#include "watchdog.h"

#define RX_BUF_SIZE 1024

static const char *TAG = "MPPT";

char *extract_ve_block(const char *str){
  const char *i1 = strstr(str, "PID");
  if(i1 != NULL){
    const size_t pl1 = strlen("PID");
    const char *i2 = strstr(i1 + pl1, "PID");
    if(i2 != NULL){
      const size_t mlen = (i2-2) - i1;
      char *ret = malloc(mlen + 1);
      if(ret != NULL){
        memcpy(ret, i1, mlen);
        ret[mlen] = '\0';
        return ret;
      }
    }
  }
  return NULL;
}

void vSerial_read_task(void *pvParameters) {
  const uart_config_t uart_config = {
    .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
  };
  bool isMppt1 = (bool)pvParameters;
  char rxString[1024];

  uart_port_t port = isMppt1 ? UART_NUM_1 : UART_NUM_2;
  int rxPin = isMppt1 ? CONFIG_MPPT1_RX_PIN : CONFIG_MPPT2_RX_PIN;
  
  uart_driver_install(port, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(port, &uart_config);
  uart_set_pin(port, UART_PIN_NO_CHANGE, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_flush(port);
  while(1){
    unsigned int readBytes = uart_read_bytes(port,rxString,1024,2*1000/portTICK_RATE_MS);
    if(readBytes>0){
      rxString[readBytes]=0;
    }
    char * veDirectString = extract_ve_block(rxString);
    if(veDirectString != NULL){
      ESP_LOGI(TAG,"FOUND BLOCK");
      ve_direct_block_t *veDirectBlock = ve_direct_parse_block(veDirectString);
      if(veDirectBlock != NULL){
        char *request = malloc(255);
        char *data = malloc(255);
        int volts,current,vpv,ppv,yt,yd,cs,mpd,err;
        ve_direct_get_field_int(&volts, veDirectBlock, "V");
        ve_direct_get_field_int(&current, veDirectBlock, "I");
        ve_direct_get_field_int(&vpv, veDirectBlock, "VPV");
        ve_direct_get_field_int(&ppv, veDirectBlock, "PPV");
        ve_direct_get_field_int(&cs, veDirectBlock, "CS");
        ve_direct_get_field_int(&yt, veDirectBlock, "H19");
        ve_direct_get_field_int(&yd, veDirectBlock, "H20");
        ve_direct_get_field_int(&mpd, veDirectBlock, "H21");
        ve_direct_get_field_int(&err, veDirectBlock, "ERR");
        snprintf(data,255,"\"V\":%d,\"I\":%d,\"VPV\":%d,\"PPV\":%d,\"YT\":%d,\"YD\":%d,\"MPD\":%d,\"CS\":%d,\"ERR\":%d",
                  volts,current,vpv,ppv,yt,yd,mpd,cs,err);
            
        buildEmonCMSRequest(request,isMppt1?"mppt1":"mppt2",data);
        xTaskCreate(https_get_task, isMppt1?"https_mppt1":"https_mppt2", 8192, request, 5, NULL);
        vTaskDelete(NULL);
      } else {
        ESP_LOGE(TAG,"ERROR parsing block");
      }
    } else {
      ESP_LOGE(TAG,"Block not found in %d bytes",strlen(rxString));
    }
  }
}