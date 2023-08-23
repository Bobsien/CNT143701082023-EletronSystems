
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <esp_task.h>
#include "esp_log.h"
#include "hx711.h"
#include "global_variables.h"

#include "hx.h"

void hxTask(void *params){
    hx711_t dev ={
        .dout = DOUT_GPIO,
        .pd_sck = PD_SCK_GPIO,
        .gain = HX711_GAIN_A_128
    };
    
    //Inicializando o dev
    while(1){
        esp_err_t error = hx711_init(&dev);
        if(error == ESP_OK){
            break;
        }
        ESP_LOGE("HX711", "ERRO DE INICIALIZACAO");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    //leitura do dev
    while(1){
        esp_err_t error = hx711_wait(&dev, 500);
        if (error != ESP_OK){
            ESP_LOGE("HX711", "Dispositivo nao foi encontrado");
        }

        int32_t data;
        error = hx711_read_data(&dev, &data);
        if (error != ESP_OK){
            ESP_LOGE("HX711", "Erro de leitura");
        }else{
            //printf("Valor RAW: %d, Peso: %f \n", data, hx711ConvVal(data));
            PESO=hx711ConvVal(data);
        }

        vTaskDelay(pdMS_TO_TICKS(13));
    }
}

float hx711ConvVal(long RAW){
    float value = 0,
          unit = 0;

          unit = (float)CALBALP01/(float)(CALBALV01 - CALBALV00);
          value = (float)(RAW - CALBALV00) * unit;

          return value;
}