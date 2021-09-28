idf_component_register(SRCS 
                      "emoncms_request.c" 
                      "https_request.c" 
                      "wifi.c" 
                      "mppt.c" 
                      "watchdog.c" 
                      "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES vedirect
                    PRIV_REQUIRES nvs_flash )
