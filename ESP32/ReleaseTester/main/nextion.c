#include "nextion.h"
#include "UART.h"
#include "esp_log.h"

int NxState = 0;
int oldState = 0;

void nextionTask(void * params ){
    unsigned int nxnt = 0;

   vTaskDelay(pdMS_TO_TICKS(100));

   //Envia configurações
   NxTextSend("CFG.cb0",floatToCharArray(gDELAY));   
   NxTextSend("CFG.cb1",floatToCharArray(VMAX));
   NxValueSend("CFG.n0",floatToCharArray(CALIB_L));
   NxValueSend("CFG.n1",floatToCharArray(CALIB_H));
   NxPageSend("Boot");
   vTaskDelay(pdMS_TO_TICKS(5000));
   NxPageSend("Main");
   NxState = 1;

    while(1){
         unsigned short addr = 0, dataaddr = 0;         

        xTaskNotifyWait(0, ULONG_MAX, &nxnt, pdMS_TO_TICKS(20));

        if (nxnt == 0) {
             
               //  NxValueSend("x0","37");
               switch(NxState){
                    case 1:
                         if(oldState != 1){
                              oldState = 1;
                              NxPageSend("Main");
                         }

                         NxValueSend("x0",floatToCharArray( (PESO-TARA)*100));
                         if (RAMPA){
                              NxTextSend("Main.t0","Rampa");
                         }else{
                              NxTextSend("Main.t0","Linear");
                         }
                         
                         break;
                    case 2:
                         if(oldState != 2){
                              oldState = 2;
                              NxPageSend("Graph");
                         }
                         NxValueSend("x0",floatToCharArray(MIN));
                         NxValueSend("x1",floatToCharArray(MED));
                         NxValueSend("x2",floatToCharArray(MAX));
                         if(START)
                         {

                              NxGraphPlot('5','0',floatToCharArray(PESO*GRAPH_FACTOR),1);

                         }
                         break;  
                    case 3:
                         if(oldState != 3){
                              oldState = 3;
                              NxPageSend("Summary");
                         }


                         break;                            
                    case 4:
                         if(oldState != 4){
                              oldState = 4;
                              NxTextSend("CFG.cb0",floatToCharArray(gDELAY));   
                              NxTextSend("CFG.cb1",floatToCharArray(VMAX));
                              NxValueSend("CFG.cb2",floatToCharArray(VUNITMM));
                              NxValueSend("CFG.n0",floatToCharArray(CALIB_L));
                              NxValueSend("CFG.n1",floatToCharArray(CALIB_H));                              
                              NxPageSend("CFG");
                         }
                         break;
                    case 5:
                         if(oldState != 5){
                              oldState = 5;
                              NxPageSend("Sobre");
                         }
                         break;

               }
                    
               vTaskDelay(pdMS_TO_TICKS(50));
    

        } else {
             //  xTaskNotifyWait(nxnt,0,&nxnt,portMAX_DELAY);
             if (LocateOnBuffer1_EOC("BTN:CFG","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 4;                
                ClearOnBuffer1("BTN:CFG",addr);
                ESP_LOGI("NX","CFG");
             }


             if (LocateOnBuffer1_EOC("BTN:CFVOLT","###",&addr,&dataaddr)){
                oldState = NxState;                
                NxState = 1;
                ClearOnBuffer1("BTN:CFVOLT",addr);
                ESP_LOGI("NX","MAIN");
             }

           /*  if (LocateOnBuffer1_EOC("BTN:CFVOLT","###",&addr,&dataaddr)){
                NxState = 1;
                ESP_LOGI("NX","CFG");
             }   */   


             if (LocateOnBuffer1_EOC("TARA:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 1;
                TARA =  NxTakeLongValue(dataaddr);
                TARA = TARA/100;

                if (TARA > 0 ){
                    NxSend("x0.bco=","64623");
                }else{
                    NxSend("x0.bco=","65535");
                }

                ESP_LOGI("NX","TARA: %f, %s", TARA, UART1_BUF);                
                ClearOnBuffer1("TARA:",addr);                                
             }    

             if (LocateOnBuffer1_EOC("BTVEL:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 1;
                VEL = NxTakeLongValue(dataaddr);
                //VEL = TVELARA/100;
                NxValueSend("x1",floatToCharArray(VEL));
                ESP_LOGI("NX","VELOCIDADE: %f, %s", VEL, UART1_BUF);                
                ClearOnBuffer1("BTVEL:",addr);                                
             }                

             if (LocateOnBuffer1_EOC("BTRMP:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 1;
                RAMPA = (UART1_BUF[dataaddr] - 48);
                //ESP_LOGI("NX","Rampa Status: %d, %s", RAMPA, UART1_BUF);                
                ClearOnBuffer1("BTRMP:",addr);                                
             }         

             if (LocateOnBuffer1_EOC("BTN:APLIC","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 1;
                ClearOnBuffer1("BTN:APLIC",addr);

               ESP_LOGI("NX","CFG APLICADA");

               //Aplicando configurações, significa que enviou dados que devem ser retirados do buffer e aplicados.
               if (LocateOnBuffer1_EOC("SDEL:","###",&addr,&dataaddr)){
                   // oldState = NxState;                    
                    gDELAY = NxTakeLongValue(dataaddr);
                    //VEL = TVELARA/100;
                    //NxValueSend("x1",floatToCharArray(VEL));
                    ESP_LOGI("NX","DELAY: %d, %s", gDELAY, UART1_BUF);                
                    ClearOnBuffer1("SDEL:",addr);                                
               } 

               if (LocateOnBuffer1_EOC("SVEL:","###",&addr,&dataaddr)){
                   // oldState = NxState;                    
                    VMAX = NxTakeLongValue(dataaddr);
                    //VEL = TVELARA/100;
                    //NxValueSend("x1",floatToCharArray(VEL));
                    ESP_LOGI("NX","MAX VELOCIDADE: %d, %s", VMAX, UART1_BUF);                
                    ClearOnBuffer1("SVEL:",addr);                                
               }

               if (LocateOnBuffer1_EOC("BTNMM:","###",&addr,&dataaddr)){
                   // oldState = NxState;                    
                    VUNITMM = UART1_BUF[dataaddr]-48;
                    //VEL = TVELARA/100;
                    //NxValueSend("x1",floatToCharArray(VEL));
                    ESP_LOGI("NX","VEL EM mm: %d, %s", VUNITMM, UART1_BUF);                
                    ClearOnBuffer1("BTNMM:",addr);                                
               }

               if (LocateOnBuffer1_EOC("SCZER:","###",&addr,&dataaddr)){
                   // oldState = NxState;                    
                    CALIB_L = NxTakeLongValue(dataaddr);
                    //VEL = TVELARA/100;
                    //NxValueSend("x1",floatToCharArray(VEL));
                    ESP_LOGI("NX","CAL MIN: %d, %s", CALIB_L, UART1_BUF);                
                    ClearOnBuffer1("SCZER:",addr);                                
               } 

               if (LocateOnBuffer1_EOC("SCCAL:","###",&addr,&dataaddr)){
                   // oldState = NxState;                    
                    CALIB_H = NxTakeLongValue(dataaddr);
                    //VEL = TVELARA/100;
                    //NxValueSend("x1",floatToCharArray(VEL));
                    ESP_LOGI("NX","CAL MAX: %d, %s", CALIB_H, UART1_BUF);                
                    ClearOnBuffer1("SCCAL:",addr);                                
               }                 
             }   

             if (LocateOnBuffer1_EOC("BTN:GRAPH","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 2;
                ESP_LOGI("NX","GRAPH");              
                ClearOnBuffer1("BTN:GRAPH:",addr);                                
             }

             if (LocateOnBuffer1_EOC("BTN:SUMM","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = 3;
                ESP_LOGI("NX","SUMMARY");
                ClearOnBuffer1("BTN:SUMM:",addr);                                
             }

             if (LocateOnBuffer1_EOC("BTN:VOLT","###",&addr,&dataaddr)){
                oldState = NxState;
                switch (oldState)
                {
                case 2:
                    NxState = 1;
                    break;
                
                case 3:
                    NxState = 2;              
                    break;
                }
                ClearOnBuffer1("BTN:VOLT:",addr); 
             } 

        }    
    }
}


void NxValueSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 11;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
    uint8_t data_to_send = 0xFF;
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
     snprintf(buff, sizeof(buff), "%s.val=%s%c%c%c", variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
     
 /*   uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 6;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
    uint8_t data_to_send = 0xFF;
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
     snprintf(buff, sizeof(buff), "%s%s%c%c%c", variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
     
 /*   uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxTextSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 11;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
    uint8_t data_to_send = 0xFF;
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
     snprintf(buff, sizeof(buff), "%s.txt=\"%s\"%c%c%c", variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
   /* uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxPageSend(const char page[]) {
     
          int l = charVarSize(page)+9;
          char buff[l];

          uint8_t data_to_send = 0xFF;

          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);

          snprintf(buff, sizeof(buff), "page %s%c%c%c", page, 0xff, 0xff, 0xff);
          uart_write_bytes(UART_NUM_1,buff,sizeof(buff));

        /*  uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/

          ets_delay_us(2000);
     
}

void NxGraphPlot(const char graphID, const char can, const char value[], int hscale) {
  short i = 0;
  for (i = 0; i < hscale; i++) {
    ets_delay_us(2000);
     int l = 1 + 1 + charVarSize(value) +  12;
    char buff[l];
    snprintf(buff, sizeof(buff), "add %c,%c,%s%c%c%c\n\r", graphID, can, value, 0xff, 0xff, 0xff);
    uart_write_bytes(UART_NUM_1,buff,sizeof(buff));

   /* uart_write_bytes(UART_NUM_1,"add ",sizeof("add "));
    uart_write_bytes(UART_NUM_1,graphID,sizeof(graphID));
    uart_write_bytes(UART_NUM_1,',',sizeof(','));
    uart_write_bytes(UART_NUM_1,can,sizeof(can));
    uart_write_bytes(UART_NUM_1,',',sizeof(','));
    uart_write_bytes(UART_NUM_1,value,sizeof(value));
    uart_write_bytes(UART_NUM_1,(char)(0xff),1);
    uart_write_bytes(UART_NUM_1,(char)(0xff),1);
    uart_write_bytes(UART_NUM_1,(char)(0xff),1);*/
  }
}

long NxTakeLongValue(int dataaddr){

     return (UART1_BUF[dataaddr] - 48)*1000000000 + (UART1_BUF[dataaddr+1] - 48)*100000000 + \
                (UART1_BUF[dataaddr+2] - 48)*10000000 + (UART1_BUF[dataaddr+3] - 48)*1000000 + (UART1_BUF[dataaddr+4] - 48)*100000 + \
                 + (UART1_BUF[dataaddr+5] - 48)*10000 +  + (UART1_BUF[dataaddr+6] - 48)*1000 +  + (UART1_BUF[dataaddr+7] - 48)*100 \
                  + (UART1_BUF[dataaddr+8] - 48)*10 +  + (UART1_BUF[dataaddr+9] - 48);
}



char* floatToCharArray(float value) {
     int intValue = (int)value; // Converte o float para int

    int requiredSize = snprintf(NULL, 0, "%d", intValue) + 1; // Tamanho necessário, incluindo terminador nulo
    char* result = (char*)malloc(requiredSize * sizeof(char)); // Aloca memória dinamicamente
    if (result == NULL) {
        perror("Erro na alocação de memória");
        exit(EXIT_FAILURE);
    }

    snprintf(result, requiredSize, "%d", intValue); // Formata o valor inteiro na string

    return result;
}