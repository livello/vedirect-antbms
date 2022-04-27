#include <string.h>
#include <stdio.h>
#include "esp_log.h"

#define ANT_TAG "ANTBMS"

int nbr_cell,soc,cell_av,cell_min,cell_max,cell_1,cell_2,cell_3,cell_4,cell_5,cell_6,cell_7,cell_8,
      cell_9,cell_10,cell_11,cell_12,cell_13,cell_14,cell_15,cell_16,cell_17,cell_18,cell_19,cell_20,
      cell_21,cell_22,cell_23,cell_24,t_bal, t_fet, t_1, t_2, t_3, t_4, puiss, HighTemp, LowTemp;
uint8_t chg_status,dischg_status,balance_status;
float volt,amp, Ah_install, Ah_rest, Ah_Total ;

void read_values (uint8_t incomingByte[512]){
    uint32_t tmp = ((((uint8_t)incomingByte[70])<< 24) + (((uint8_t)incomingByte[71])<< 16)+ (((uint8_t)incomingByte[72])<< 8)+ ((uint8_t)incomingByte[73]));
    if (tmp > 2147483648){amp=(-(2*2147483648)+tmp);}
    else{ amp = tmp;}
   uint32_t tmp2 = ((((uint8_t)incomingByte[111])<< 24) + (((uint8_t)incomingByte[112])<< 16)+ (((uint8_t)incomingByte[113])<< 8)+ ((uint8_t)incomingByte[114]));
    if (tmp2 > 2147483648){puiss=(-(2*2147483648)+tmp2);}
    else{ puiss = tmp2;}
    nbr_cell = (uint8_t)incomingByte[123]; 
    soc = (uint8_t)incomingByte[74]; 
    volt = ((((uint8_t)incomingByte[4])<< 8) + (uint8_t)incomingByte[5]); 
    chg_status = (uint8_t)incomingByte[103];
    dischg_status = (uint8_t)incomingByte[104];
    balance_status = (uint8_t)incomingByte[105];
    cell_av = ((((uint8_t)incomingByte[121])<< 8) + (uint8_t)incomingByte[122]); 
    cell_min = ((((uint8_t)incomingByte[119])<< 8) + (uint8_t)incomingByte[120]); 
    cell_max = ((((uint8_t)incomingByte[116])<< 8) + (uint8_t)incomingByte[117]); 
    t_fet = ((((uint8_t)incomingByte[91])<< 8) + (uint8_t)incomingByte[92]); 
    t_bal = ((((uint8_t)incomingByte[93])<< 8) + (uint8_t)incomingByte[94]); 
    t_1 = ((((uint8_t)incomingByte[95])<< 8) + (uint8_t)incomingByte[96]); 
    t_2 = ((((uint8_t)incomingByte[97])<< 8) + (uint8_t)incomingByte[98]); 
    t_3 = ((((uint8_t)incomingByte[99])<< 8) + (uint8_t)incomingByte[100]); 
    t_4 = ((((uint8_t)incomingByte[101])<< 8) + (uint8_t)incomingByte[102]); 
    cell_1 = ((((uint8_t)incomingByte[6])<< 8) + (uint8_t)incomingByte[7]); 
    cell_2 = ((((uint8_t)incomingByte[8])<< 8) + (uint8_t)incomingByte[9]); 
    cell_3 = ((((uint8_t)incomingByte[10])<< 8) + (uint8_t)incomingByte[11]); 
    cell_4 = ((((uint8_t)incomingByte[12])<< 8) + (uint8_t)incomingByte[13]); 
    cell_5 = ((((uint8_t)incomingByte[14])<< 8) + (uint8_t)incomingByte[15]); 
    cell_6 = ((((uint8_t)incomingByte[16])<< 8) + (uint8_t)incomingByte[17]); 
    cell_7 = ((((uint8_t)incomingByte[18])<< 8) + (uint8_t)incomingByte[19]); 
    cell_8 = ((((uint8_t)incomingByte[20])<< 8) + (uint8_t)incomingByte[21]); 
    cell_9 = ((((uint8_t)incomingByte[22])<< 8) + (uint8_t)incomingByte[23]); 
    cell_10 = ((((uint8_t)incomingByte[24])<< 8) + (uint8_t)incomingByte[25]); 
    cell_11 = ((((uint8_t)incomingByte[26])<< 8) + (uint8_t)incomingByte[27]); 
    cell_12 = ((((uint8_t)incomingByte[28])<< 8) + (uint8_t)incomingByte[29]); 
    cell_13 = ((((uint8_t)incomingByte[30])<< 8) + (uint8_t)incomingByte[31]); 
    cell_14 = ((((uint8_t)incomingByte[32])<< 8) + (uint8_t)incomingByte[33]); 
    cell_15 = ((((uint8_t)incomingByte[14])<< 8) + (uint8_t)incomingByte[35]); 
    cell_16 = ((((uint8_t)incomingByte[16])<< 8) + (uint8_t)incomingByte[37]); 
    cell_17 = ((((uint8_t)incomingByte[18])<< 8) + (uint8_t)incomingByte[39]); 
    cell_18 = ((((uint8_t)incomingByte[20])<< 8) + (uint8_t)incomingByte[41]); 
    cell_19 = ((((uint8_t)incomingByte[22])<< 8) + (uint8_t)incomingByte[43]); 
    cell_20 = ((((uint8_t)incomingByte[24])<< 8) + (uint8_t)incomingByte[45]); 
    cell_21 = ((((uint8_t)incomingByte[26])<< 8) + (uint8_t)incomingByte[47]); 
    cell_22 = ((((uint8_t)incomingByte[28])<< 8) + (uint8_t)incomingByte[49]); 
    cell_23 = ((((uint8_t)incomingByte[30])<< 8) + (uint8_t)incomingByte[51]); 
    cell_24 = ((((uint8_t)incomingByte[32])<< 8) + (uint8_t)incomingByte[53]); 
    Ah_install = ((((uint8_t)incomingByte[75])<< 24) + (((uint8_t)incomingByte[76])<< 16)+ (((uint8_t)incomingByte[77])<< 8)+ ((uint8_t)incomingByte[78]));
    Ah_rest = ((((uint8_t)incomingByte[79])<< 24) + (((uint8_t)incomingByte[80])<< 16)+ (((uint8_t)incomingByte[81])<< 8)+ ((uint8_t)incomingByte[82]));
    Ah_Total = ((((uint8_t)incomingByte[83])<< 24) + (((uint8_t)incomingByte[84])<< 16)+ (((uint8_t)incomingByte[85])<< 8)+ ((uint8_t)incomingByte[86]));
}

void print_value(){
    ESP_LOGI(ANT_TAG,"Cells = %d",nbr_cell);
    ESP_LOGI(ANT_TAG,"SOC = %d%%",soc);
    ESP_LOGI(ANT_TAG,"Battery Voltage = %0.1fV",volt/10.0);
    ESP_LOGI(ANT_TAG,"Current = %0.1fA",amp/10.0);
    ESP_LOGI(ANT_TAG,"Power = %dW",puiss);
    ESP_LOGI(ANT_TAG,"Charge Status = %d",chg_status);
    ESP_LOGI(ANT_TAG,"Discharge Staus = %d",dischg_status);
    ESP_LOGI(ANT_TAG,"Balancing Status = %d",balance_status);
    ESP_LOGI(ANT_TAG,"Average cell voltage = %0.3fV",cell_av/1000.0);
    ESP_LOGI(ANT_TAG,"Minimum cell voltage = %0.3fV",cell_min/1000.0);
    ESP_LOGI(ANT_TAG,"Maximum cell voltage = %0.3fV",cell_max/1000.0);
    ESP_LOGI(ANT_TAG,"Difference = %0.3fV",(cell_max-cell_min)/1000.0);
    ESP_LOGI(ANT_TAG,"MOSFET Temperature = %d°C",t_fet);
    ESP_LOGI(ANT_TAG,"Balance Temperature = %d°C",t_bal);
    ESP_LOGI(ANT_TAG,"Sensor 1 Temperature = %d°C",t_1);
    ESP_LOGI(ANT_TAG,"Sensor 2 Temperature = %d°C",t_2);
    ESP_LOGI(ANT_TAG,"Sensor 3 Temperature = %d°C",t_3);
    ESP_LOGI(ANT_TAG,"Sensor 4 Temperature = %d°C",t_4);
    ESP_LOGI(ANT_TAG,"Cell 1 voltage = %0.3fV",cell_1/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 3 voltage = %0.3fV",cell_3/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 2 voltage = %0.3fV",cell_2/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 4 voltage = %0.3fV",cell_4/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 5 voltage = %0.3fV",cell_5/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 6 voltage = %0.3fV",cell_6/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 7 voltage = %0.3fV",cell_7/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 8 voltage = %0.3fV",cell_8/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 9 voltage = %0.3fV",cell_9/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 10 voltage = %0.3fV",cell_10/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 11 voltage = %0.3fV",cell_11/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 12 voltage = %0.3fV",cell_12/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 13 voltage = %0.3fV",cell_13/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 14 voltage = %0.3fV",cell_14/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 15 voltage = %0.3fV",cell_15/1000.0);
    ESP_LOGI(ANT_TAG,"Cell 16 voltage = %0.3fV",cell_16/1000.0);
    ESP_LOGI(ANT_TAG,"Total Capacity = %0.1fAh",Ah_install/1000000.0);
    ESP_LOGI(ANT_TAG,"Remaining Capacity = %0.1fAh",Ah_rest/1000000.0);
    ESP_LOGI(ANT_TAG,"Cycle Capacity = %0.1fAh",Ah_Total/1000000.0);
}

void getDataString (char *url){
    snprintf(url,1024,
        "\"C\":%d,\"SOC\":%d,\"BV\":%.0f,\"BI\":%.0f,\"P\":%d,"
        "\"TM\":%d,\"TB\":%d,\"T1\":%d,\"T2\":%d,\"V1\":%d,\"V2\":%d,\"V3\":%d,"
        "\"V4\":%d,\"V5\":%d,\"V6\":%d,\"V7\":%d,\"V8\":%d,\"V9\":%d,\"V10\":%d,"
        "\"V11\":%d,\"V12\":%d,\"V13\":%d,\"V14\":%d,\"V15\":%d,\"V16\":%d,"
        "\"AT\":%.0f,\"AR\":%.0f,\"AC\":%.0f,\"T3\":%d,\"T4\":%d,\"VMX\":%d,"
        "\"VMN\":%d,\"VMD\":%d,\"CHGS\":%d,\"DCHS\":%d,\"BALS\":%d",
        nbr_cell,soc,volt,amp,puiss,t_fet,t_bal,t_1,t_2,cell_1,cell_2,cell_3,cell_4,
        cell_5,cell_6,cell_7,cell_8,cell_9,cell_10,cell_11,cell_12,cell_13,cell_14,
        cell_15,cell_16,Ah_install,Ah_rest,Ah_Total,t_3,t_4,cell_max,cell_min,cell_av,
        chg_status,dischg_status,balance_status);
}