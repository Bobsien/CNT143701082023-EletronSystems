#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_task.h>
#include "esp_log.h"

#include "global.h"  
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
    // Inicialização da NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
        
    VMAX = 50;
    gDELAY = 0;
    CALIB_H = 12345212;
    CALIB_L = 212; 
    RAMPA = 0;  
    TARA = 0; 
    PESO = 0;
    VEL = 10;
    VUNITMM = 0;
    LOADCEL_RAW = 0;

    loadCfgValues();

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
    
   
    xTaskCreate(nextionTxTask,"nextionTxTask",32*1024,NULL,2,&nxNotify);
    xTaskCreate(nextionRxTask,"nextionRxTask",5*1024,NULL,2,&nxNotify);
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

                memset(LEITURAS_PESO,'\0',N_LEITURAS*sizeof(float));
               /* for (int i=0; i<N_LEITURAS; i++){
                    LEITURAS_PESO[i]='\0';
                }*/
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
                if(nCyclesMax > N_LEITURAS){
                    nCyclesMax = N_LEITURAS;
                }
                nCycles = 0;

                ESP_LOGI("MAIN","VEL: %f mm/min TTIME: %d CYCLE: %d ms n: %d ",VEL,totalTestTimeMs(VEL), cicleReadingTimeMs(VEL), nCyclesMax);
            }

            if(nCyclesMax < nCycles){
                START = 0;
                stopAck = 0;      
                NxValueSend("Main.bt0","0");   //Concluiu teste em modo automatico, altera estado do botão na tela principal
                summarize();
                ESP_LOGI("SUMMARY","MAX: %f MIN: %f MED: %f DESV: %f VARI: %f TRAB: %f", MAX, MIN, MED, DESV, VARI, TRAB);
            }else{
                //LEITURAS_PESO[nCycles]= rand() % 5001; //PESO; 
                PESO = rand() % 5001;
                storeReadedData((PESO/1000)*9.81,nCycles);
                //ESP_LOGI("TESTE","%f", LEITURAS_PESO[nCycles]);
                if(nCycles > 5){
                    summarize();    //Realiza calculos de sumario de valores
                }
                nCycles++;
            }

            vTaskDelay(pdMS_TO_TICKS(cicleReadingTimeMs(VEL)));
           // 
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


