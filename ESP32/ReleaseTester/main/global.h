#ifndef __GLOBAL_H
    #define __GLOBAL_H

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"
    #include <esp_task.h>
    #include "esp_log.h"

    #include "nvs_flash.h"
    #include "nvs.h"

    #define FIRMVERSION "0.91"
    #define SN "wyq18261maqkh1" //Serial Number de 14 digitos (numeros e letras sem acento)


    #define N_LEITURAS 780
    #define COMPRIMENTO_TESTE 350 //em mm
    #define CEL_CARGA 5000 //Peso máximo suportado pela céçula de carga instalada em gramas.    



    extern TaskHandle_t nxNotify;               //Fila de notificações de eventos para Nextion atrelado a UART1
    extern TaskHandle_t motorNotify;            //Fila de notiricações de eventos para Motor de passos atrelado a UART2

    int totalTestTimeMs(float vel);             //Calcula tempo total que para completar o teste baseado na VEL e COMPRIMENTO_TESTE
    int cicleReadingTimeMs(float vel);          //Calcula o tempo do ciclo de leitura para celula de carga (limite minimo de 13ms)
    float minValueOnArray(float array[]);       //Calcula valor minimo guardado em um array float
    float maxValueOnArray(float array[]);       //Calcula valor maximo guardado em um array float
    float meanValueOnArray(float array[]);      //Calcula média de valores guardados em um array float
    float varianceOnArray(float array[]);       //Calcula variancia dos valores guardados em um array float
    float stdDevOnArray(float array[]);         //Calcula desvio padrão dos valores guardados em array float
    float meanWorkTestResult(float array[]);    //Calcula trabalho médio
    void storeReadedData(float val, int cycle); //Armazena valores lidos no array realizando escalonamento
    void summarize();                           //Realiza calculos de sumário de dados
    void saveValue(const char *key,int value);                       //Salva valores inteiros na FLASH
    int getValue(const char *key);                                 //Recupera valores inteiros da FLASH
    void saveLongValue(const char *key, long value);                  //Salva valores long na FLASH
    long getLongValue(const char *key);                             //Recupera valores long da FLASH
    void saveCfgValues();
    void loadCfgValues();


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


    //Estatistiticas
    float MAX , 
          MIN, 
          MED, 
          DESV, 
          VARI, 
          TRAB,
          LEITURAS_PESO[N_LEITURAS+1];
    
#endif