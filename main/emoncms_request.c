#include <string.h>
#include "esp_system.h"
#include "esp_log.h"

void buildEmonCMSRequest (char *buffer,char *node, char *data){
  snprintf(buffer,512,"node=%s&data={%s}&apikey=%s",node,data,CONFIG_EMONCMS_API_KEY);
}
