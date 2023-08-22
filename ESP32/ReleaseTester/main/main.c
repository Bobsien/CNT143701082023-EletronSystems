#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_task.h>
#include "esp_log.h"


#include "hx.h"
#include "UART.h"
#include "nextion.h"
#include "mksServo.h"

TaskHandle_t nxNotify = NULL;
TaskHandle_t motorNotify = NULL;

void controlTask(void * params);


//Pre-configuração do frame de comunicação
mksServoFrame_t cmdFrame = { 
        .head = 0xFA,
        .addr = 1,
};


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


    mksInit();
}

void app_main(void)
{
    setup();

    //xTaskCreate(hxTask,"hxTask",5*1024,NULL,2,NULL);
    xTaskCreate(uart1Task,"uart1Task",5*1024,NULL,2,&nxNotify);
    xTaskCreate(uart2Task,"uart2Task",5*1024,NULL,2,&motorNotify);
    xTaskCreate(nextionTask,"nextionTask",5*1024,NULL,2,&nxNotify);
    xTaskCreate(controlTask,"controlTask",5*1024,NULL,2,&nxNotify);
}


void controlTask(void * params){
    static bool startAck = 0, //Informa quando o comando de start foi processado
                stopAck = 0;
    while(1){
        if(START){
            if(!startAck){
                stopAck = 0;
                startAck = 1;

                //Comando para velocidade constante
                //if(!RAMPA){
                    cmdFrame.Function =0xF6;  //Velocidade constante
               // }
                cmdFrame.dir = 0;
                if(VUNITMM){
                    cmdFrame.speed = VEL; 
                }else{
                    cmdFrame.speed = VEL*100; 
                }                
                mksSendSpeedCommand(cmdFrame);                
            }


        }else{
            if(!stopAck){
                stopAck = 1;
                startAck = 0;
                cmdFrame.Function =0xF6;  //Velocidade constante
                cmdFrame.dir = 0;
                cmdFrame.speed = 0;        
                mksSendSpeedCommand(cmdFrame);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }

}