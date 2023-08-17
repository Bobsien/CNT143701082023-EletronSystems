#ifndef __GLOBAL_H
    #define __GLOBAL_H

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include <esp_task.h>
    #include "esp_log.h"


    #define N_LEITURAS 14400
    #define CEL_CARGA 5000 //Peso máximo suportado pela céçula de carga instalada em gramas.
    #define GRAPH_FACTOR (255/CEL_CARGA)

    extern TaskHandle_t nxNotify;

    //Configurações
    int VMAX;
    int gDELAY;
    int CALIB_H;
    int CALIB_L;

    //Tela principal
    bool RAMPA;
    float TARA;
    float PESO;
    float VEL;
    bool VUNITMM;
    bool START;


    //Estatistitivas
    float MAX , 
          MIN, 
          MED, 
          DESV, 
          VARI, 
          TRAB,
          LEITURAS_PESO[N_LEITURAS+1];
    



#endif