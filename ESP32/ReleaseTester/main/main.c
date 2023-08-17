#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_task.h>
#include "esp_log.h"


#include "hx.h"
#include "UART.h"
#include "nextion.h"

TaskHandle_t nxNotify = NULL;


void setup(){
    VMAX = 50;
    gDELAY = 2;
    CALIB_H = 12345212;
    CALIB_L = 212; 
    RAMPA = 0;  
    TARA = 0; 
    PESO = 0.37;
    VEL = 0;
    VUNITMM = 0;

    START = 0;

    //Estatistitivas
    MAX = 0; 
    MIN = 0;
    MED = 0;
    DESV = 0;
    VARI = 0;
    TRAB = 0;
    memset(LEITURAS_PESO, 0, sizeof(LEITURAS_PESO));
}

void app_main(void)
{
    setup();

    //xTaskCreate(hxTask,"hxTask",5*1024,NULL,2,NULL);
    xTaskCreate(uartTask,"uartTask",5*1024,NULL,2,&nxNotify);
    xTaskCreate(nextionTask,"nextionTask",5*1024,NULL,2,&nxNotify);
}

