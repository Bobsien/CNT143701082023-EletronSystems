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
int totalTestTimeMs(float vel);
int cicleReadingTimeMs(float vel);



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
    VEL = 10;
    VUNITMM = 1;

    START = 0;

    //Estatistitivas
    MAX = 0; 
    MIN = 0;
    MED = 0;
    DESV = 0;
    VARI = 0;
    TRAB = 0;
    memset(LEITURAS_PESO, 0, sizeof(LEITURAS_PESO));

    nxGraphMain = 0;
    nxGraphComplete = 0;

    xTaskCreate(uart1Task,"uart1Task",5*1024,NULL,2,&nxNotify);
    xTaskCreate(uart2Task,"uart2Task",5*1024,NULL,2,&motorNotify);
    mksInit();
}

void app_main(void)
{
    setup();

    //xTaskCreate(hxTask,"hxTask",5*1024,NULL,2,NULL);
    
   
    xTaskCreate(nextionTask,"nextionTask",5*1024,NULL,2,&nxNotify);
    xTaskCreate(controlTask,"controlTask",5*1024,NULL,2,NULL);
}


void controlTask(void * params){
    static bool startAck = 0, //Informa quando o comando de start foi processado
                stopAck = 0;
    int     nCycles = 0,   //Contador de ciclos de aquisição
            nCyclesMax = 0;//Numero de ciclos de aquisição para garantir a janela de tempo do teste
    vTaskDelay(pdMS_TO_TICKS(5000));
    while(1){
        
        if(START){            
            if(!startAck){
                ESP_LOGI("MAIN","STARTING");
                
                stopAck = 0;
                startAck = 1;

                memset(LEITURAS_PESO,'\0',N_LEITURAS);
                nxGraphMain = 0;
                nxGraphComplete = 0;                 

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
                nCyclesMax = totalTestTimeMs(VEL)/cicleReadingTimeMs(VEL);
                nCycles = 0;

            }

            if(nCyclesMax < nCycles){
                START = 0;
                stopAck = 0;                              
            }else{
                LEITURAS_PESO[nCycles]= rand() % 5001; //PESO;                
                nCycles++;
            }

            vTaskDelay(pdMS_TO_TICKS(cicleReadingTimeMs(VEL)));
            ESP_LOGI("MAIN","VEL: %f mm/min TTOTAL: %d CYCLE: %d n: %d PESO: %f",VEL,totalTestTimeMs(VEL), cicleReadingTimeMs(VEL), nCyclesMax,LEITURAS_PESO[nCycles-1]);
        }else{
            if(!stopAck){
                ESP_LOGI("MAIN","STOPED");
                stopAck = 1;
                startAck = 0;
             /*   cmdFrame.Function =0xF6;  //Velocidade constante
                cmdFrame.dir = 0;
                cmdFrame.speed = 0;       
                mksSendSpeedCommand(cmdFrame);      */          
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        
    }

}


//Esta função retorna apenas o tempo total que o teste vai levar para ser concluido em ms
int totalTestTimeMs(float vel){
    long mm_seg = 0;
    float factor = 0,
          time_ms = 0;          
    
    //Primeiro, ajustamos a unidade de medida para mm/min
    if (VUNITMM){
       mm_seg = vel; 
    }else{
        //Caso esteja em m/min
       mm_seg = vel*100;
    }
    //Agora, verificamos quanto tempo proporcional a 1 minuto irá gastar para concluir o teste
    //onde "COMPRIMENTO_TESTE" é o comprimento de fita a ser testado 
    factor = COMPRIMENTO_TESTE/mm_seg; 
    //Aplicando o fator (% de tempo de 1 minuto) sobre 60 segundos e convertendo para ms
    time_ms = factor*60*1000; //Como resultado, temos o tempo (em ms) que levará para concluir o teste
    return (int)time_ms;
}

int cicleReadingTimeMs(float vel){    
    float time_ms = 0,
          cicle = 0;

    time_ms = totalTestTimeMs(vel);
    cicle = time_ms/N_LEITURAS;

    if(cicle < 13){
        return 13;
    }else{
        return (int)cicle;
    }
}