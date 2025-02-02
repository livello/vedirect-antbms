#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "antbms.h"
#include "https_request.h"
#include "emoncms_request.h"

esp_bd_addr_t bd_addr;
static esp_bt_pin_code_t pin_code = CONFIG_BMS_BT_PIN;
static uint8_t magic_word[8] = {0xDB, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x0A};
uint8_t message[140];
uint8_t message_pointer = 0;

void composeRequest(void) {
	read_values((uint8_t *) message);
	char *bmsData = malloc(1024);
	char *request = malloc(1024);
	getDataString(bmsData);
	printf("%s", bmsData);
/*  buildEmonCMSRequest(request,"bms",bmsData,1024);
  while(!enqueueRequest(request)){
    vTaskDelay(200/portTICK_PERIOD_MS);
  }*/
}

static const char *TAG = "BLE";

void gapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
	switch(event) {
		case ESP_BT_GAP_PIN_REQ_EVT:
			esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
			ESP_LOGD(TAG, "ESP_BT_GAP_PIN_REQ_EVT");
			break;
		case ESP_BT_GAP_MODE_CHG_EVT:
			printf("\n write GAP \n");
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
			esp_spp_write(param->mode_chg.bda, 8, magic_word);
			break;
		default:
			printf("\ngapCallback %i \n", event);
			break;
	}
}

void sppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
	switch(event) {
		case ESP_SPP_INIT_EVT:
			esp_bt_dev_set_device_name("SOLAR_MONITOR");
			break;
		case ESP_SPP_DISCOVERY_COMP_EVT:
			if (param->disc_comp.status == ESP_SPP_SUCCESS) {
				esp_spp_connect(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, param->disc_comp.scn[0], bd_addr);
			}
			break;
		case ESP_SPP_OPEN_EVT:
			esp_spp_write(param->open.handle, 8, magic_word);
			break;
		case ESP_SPP_DATA_IND_EVT:
			memcpy(message + message_pointer, param->data_ind.data, param->data_ind.len);
			message_pointer += param->data_ind.len;
			if (message_pointer == 140) {
//                esp_spp_disconnect(param->data_ind.handle);
				composeRequest();
				printf("\n write again \n");
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				esp_spp_write(param->data_ind.handle, 8, magic_word);
				printf("\n written \n");
			}

			break;
/*  	case ESP_SPP_CONG_EVT:
  		printf("\nESP_SPP_CONG_EVT\n");
  		break;
  	case ESP_SPP_WRITE_EVT:
	    printf("\nESP_SPP_WRITE_EVT cong=%i\n",param->cong.cong);
  		break;*/
		case ESP_SPP_WRITE_EVT:
			if (!param->write.cong) {
				/* The lower layer is not congested, you can send the next data packet now. */
//			  printf("\n write 1\n");
//			  esp_spp_write(param->write.handle, 8, magic_word);
			} else {
				/*
				 * The lower layer is congested now, don't send the next data packet until receiving the
				 * ESP_SPP_CONG_EVT with param->cong.cong == 0.
				 */
				;
			}

			/*
			 * If you don't want to manage this complicated process, we also provide the SPP VFS mode that hides the
			 * implementation details. However, it is less efficient and will block the caller until all data has been sent.
			 */
			break;
		case ESP_SPP_CONG_EVT:
/*#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
		  ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT cong:%d", param->cong.cong);
#endif*/
			if (param->cong.cong == 0) {
				/* Send the privous (partial) data packet or the next data packet. */
				printf("\n write 2\n");
				esp_spp_write(param->write.handle, 8, magic_word);
			}
			break;

		default:
			printf("\ndefault %i \n", event);
			break;
	}
}

void bluetoothInit(void) {
	printf("bluetoothinit");
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
	esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IN;

	sscanf(CONFIG_BMS_BT_ADDRESS,
	       "%x:%x:%x:%x:%x:%x",
	       (unsigned int *) &bd_addr[0],
	       (unsigned int *) &bd_addr[1],
	       (unsigned int *) &bd_addr[2],
	       (unsigned int *) &bd_addr[3],
	       (unsigned int *) &bd_addr[4],
	       (unsigned int *) &bd_addr[5]);

	ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
	ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
	ESP_ERROR_CHECK(esp_bluedroid_init());
	ESP_ERROR_CHECK(esp_bluedroid_enable());
	ESP_ERROR_CHECK(esp_bt_gap_register_callback(gapCallback));
	ESP_ERROR_CHECK(esp_spp_register_callback(sppCallback));
	ESP_ERROR_CHECK(esp_spp_init(ESP_SPP_MODE_CB));
	ESP_ERROR_CHECK(esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t)));
	ESP_ERROR_CHECK(esp_spp_start_discovery(bd_addr));
	//assert(iocap == ESP_BT_IO_CAP_IN || iocap == ESP_BT_IO_CAP_IO,true);
}