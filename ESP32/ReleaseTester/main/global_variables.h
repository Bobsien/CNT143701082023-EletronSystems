#ifndef __GLOBAL_H
    #define __GLOBAL_H

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include <esp_task.h>
    #include "esp_log.h"

    extern TaskHandle_t nxNotify;

    int VMAX;
    int gDELAY;
    int CALIB_H;
    int CALIB_L;
    bool RAMPA;
    float TARA;
    float PESO;



#endif