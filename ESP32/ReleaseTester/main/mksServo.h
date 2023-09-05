#ifndef __MKS_SERVO_H
    #define __MKS_SERVO_H

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include <esp_task.h>
    #include "esp_log.h"

    #include "global.h"

    //Definições utilizadas pelo sistema de controle do motor
    #define Mstep  188          //Mstep - O Mstep deve ser ajsutando conforme velcidade maxima esperada para o motor utilizando as equações
    //#define speed  1          //speed
   // #define velocidade  32500 //velocidade    
    #define pi  3.141592    //pi
    #define diametro  48.8  //diamentro da calandra em milimetros

    #define m57Cmodel 1
    #define SERVOADDR 0xE0
    #define DEGREE18M 1


    typedef struct mksServoFrame{        
        uint8_t head;
        uint8_t addr;
        uint8_t Function;
        bool    dir;
        uint8_t rev;
        uint16_t speed;        
        uint8_t acc;        
    } mksServoFrame_t;


    float vRpm;             //RPM

    void mksInit();
    void mksSendCommand(uint8_t commands[]);
    int mksSpeedCalc(long vel);
    uint8_t mksCRC(const uint8_t CMD[]) ;
    void mksSendSpeedCommand(mksServoFrame_t cmd);


#endif