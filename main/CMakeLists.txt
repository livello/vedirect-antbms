idf_component_register(SRCS "https_request.c" "wifi.c" "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash )
