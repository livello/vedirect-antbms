#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "esp_crt_bundle.h"

#include "watchdog.h"

#define WEB_SERVER CONFIG_EMONCMS_HOSTNAME
#define WEB_PORT CONFIG_EMONCMS_PORT
#define WEB_URL CONFIG_EMONCMS_PATH

static const char *TAG = "REQ";

static const char *REQUEST = "POST https://" WEB_SERVER WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: " CONFIG_ESP_WIFI_HOSTNAME "/1.0 esp32\r\n";

mbedtls_ssl_context ssl;
mbedtls_net_context server_fd;

char currentRequest[512];
char requestQueue[512];
bool pendingQueue = false;

void httpsStart() {
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    ESP_ERROR_CHECK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0));
    ESP_ERROR_CHECK(esp_crt_bundle_attach(&conf));
    ESP_ERROR_CHECK(mbedtls_ssl_set_hostname(&ssl, WEB_SERVER));
    ESP_ERROR_CHECK(mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT));
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    ESP_ERROR_CHECK(mbedtls_ssl_setup(&ssl, &conf));
    mbedtls_net_init(&server_fd);
    ESP_ERROR_CHECK(mbedtls_net_connect(&server_fd, WEB_SERVER,WEB_PORT, MBEDTLS_NET_PROTO_TCP));
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0){
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE){
            return;
        }
    }
    ESP_ERROR_CHECK(mbedtls_ssl_get_verify_result(&ssl));
}

void httpsClose(void){
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_ssl_session_reset(&ssl);
    mbedtls_net_free(&server_fd);
}

bool enqueueRequest(char *newRequest) {
    if(pendingQueue){
        return false;
    } else {
        strcpy(requestQueue,newRequest);
        pendingQueue=true;
    }
    return true;
}

void vHTTPSRequest(void *pvParameters){
    int ret, len;
    char buf[512];
    char finalRequest[1024]="";
    bool done = false;
    for(;;){
        if(pendingQueue){
            strcpy(currentRequest,requestQueue);
            pendingQueue = false;    
            httpsStart();        
            snprintf(finalRequest,1024,
                    "%sContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n%s",
                    REQUEST,strlen(currentRequest),currentRequest);
            
            ESP_LOGI(TAG, "Writing HTTP request...");
            ESP_LOGI(TAG, "%s",finalRequest);
            
            size_t written_bytes = 0;
            do {
                ret = mbedtls_ssl_write(&ssl,
                                        (const unsigned char *)finalRequest + written_bytes,
                                        strlen(finalRequest) - written_bytes);
                if (ret >= 0) {
                    ESP_LOGD(TAG, "%d bytes written", ret);
                    written_bytes += ret;
                } else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
                    break;
                }
            } while(written_bytes < strlen(finalRequest));
            done=false;
            putchar('\n');
            do {
                len = sizeof(buf) - 1;
                bzero(buf, sizeof(buf));
                ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);
                if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE){
                    if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                        ret = 0;
                        done=true;
                    }
                    if(ret < 0) {
                        ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
                        done=true;
                    }
                    if(ret == 0){
                        ESP_LOGD(TAG, "connection closed");
                        done=true;
                    }
                    len = ret;
                    for(int i = 0; i < len; i++) {
                        putchar(buf[i]);
                    }
                }
            } while(!done);
            putchar('\n');
            httpsClose();
            if(strstr(currentRequest, "mppt1") != NULL) {
                set_sent(1);
            }
            if(strstr(currentRequest, "mppt2") != NULL) {
                set_sent(2);
            }
            if(strstr(currentRequest, "bms") != NULL) {
                set_sent(3);
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
