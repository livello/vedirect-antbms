idf_component_register(SRCS 
                      "emoncms_request.c" 
                      "https_request.c" 
                      "wifi.c" 
                      "serial.c" 
                      "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash )
