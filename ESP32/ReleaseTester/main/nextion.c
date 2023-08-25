#include "nextion.h"
#include "UART.h"
#include "esp_log.h"
#include "hx.h"

//Definições de index de estados para cada tela - Não alterar ordem
#define SBOOT 0
#define SSOBRE 1
#define SMAIN 2
#define SGRAPH 3
#define SSUMMARY 4
#define SCFG 5

//Variaveis de controle da máquina de estados
int NxState = 0;
int oldState = 0;

void nextionTxTask(void * params ){
   vTaskDelay(pdMS_TO_TICKS(100));


   NxState = 0;

    while(1){
     float cargaAtual = 0;
     char *temp;
               switch(NxState){
                    case SBOOT:
                          //Envia configurações
                          
                          NxTextSend("fw",FIRMVERSION);
                          NxTextSend("SN",SN);
                          temp = floatToCharArray(gDELAY);
                          NxTextSend("CFG.cb0",temp);   
                          heap_caps_free(temp);
                          temp = floatToCharArray(VMAX);
                          NxTextSend("CFG.cb1",temp);
                          heap_caps_free(temp);
                          temp = floatToCharArray(VUNITMM);
                          NxValueSend("CFG.cb2",temp);
                          heap_caps_free(temp);
                          temp = floatToCharArray(CALIB_L);
                          NxValueSend("CFG.n0",temp);
                          heap_caps_free(temp);
                          temp = floatToCharArray(CALIB_H);
                          NxValueSend("CFG.n1",temp);
                          heap_caps_free(temp);
                          NxPageSend("Boot");

                          if (RAMPA){
                                   NxTextSend("Main.t0","Rampa");
                          }else{
                                   NxTextSend("Main.t0","Linear");
                          }
                          cargaAtual =((PESO/1000)*9.81)-TARA; //Calcula a carga atual em Newtons - OBS: O peso vem em gramas
                          temp = floatToCharArray(cargaAtual*100);
                          NxValueSend("Main.x0",temp);   
                          heap_caps_free(temp);
                          temp = floatToCharArray(VEL);
                          NxValueSend("Main.x1",temp);
                          heap_caps_free(temp);
                          
                          oldState = SBOOT;
                          NxState = SMAIN;                          

                          for (unsigned short intro_count=0; intro_count<250; intro_count++)
                          {
                              vTaskDelay(pdMS_TO_TICKS(20));
                              if(NxState == 1){  //Se NxState mudar para 1 significa que usuário clicou em Sobre
                                   break;
                              }
                          }
                          
                          

                          break;        

                    case SSOBRE:
                         if(oldState != SSOBRE){
                              oldState = SSOBRE;
                              NxPageSend("Sobre");
                         }
                         break;

                    case SMAIN:
                         //Estado 1 - Refente a tela principal na Nextion IDE
                         if(oldState != SMAIN){  //Verifica se veio de outro estado
                              oldState = SMAIN;
                              NxPageSend("Main");
                         }

                          cargaAtual =((PESO/1000)*9.81)-TARA; //Calcula a carga atual em Newtons - OBS: O peso vem em gramas
                          temp = floatToCharArray(cargaAtual*100);
                          NxValueSend("Main.x0",temp); 
                          heap_caps_free(temp);

                         if (RAMPA){
                              NxTextSend("Main.t0","Rampa");
                         }else{
                              NxTextSend("Main.t0","Linear");
                         }


                         if(floatVarSize(LEITURAS_PESO) > nxGraphMain){                          
                              for(nxGraphMain=nxGraphMain; floatVarSize(LEITURAS_PESO) >= nxGraphMain; nxGraphMain++){
                                    temp = floatToCharArray((LEITURAS_PESO[nxGraphMain]*255)/((CEL_CARGA/1000)*9.81));
                                    NxGraphPlot('4','0',temp,1);
                                    heap_caps_free(temp);
                                   // ESP_LOGI("GRAPH1","%d S: %d I:%d",(int)((float)(LEITURAS_PESO[nxGraphMain]*255)/CEL_CARGA), floatVarSize(LEITURAS_PESO),nxGraphMain );
                              }
                              //nxGraphMain++;
                         }


                         break;
                    case SGRAPH:
                         //Estado 2 - Referente a tela Graph na Nextion IDE
                         if(oldState != SGRAPH){  //Verifica se veio de outro estado
                              oldState = SGRAPH;
                              NxPageSend("Graph");
                         }
                         temp = floatToCharArray(MIN*100);
                         NxValueSend("x0",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(MED*100);
                         NxValueSend("x1",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(MAX*100);
                         NxValueSend("x2",temp);
                         heap_caps_free(temp);


                         if(floatVarSize(LEITURAS_PESO) >= nxGraphComplete){
                              for(nxGraphComplete=nxGraphComplete; floatVarSize(LEITURAS_PESO) >= nxGraphComplete; nxGraphComplete++){
                                    temp = floatToCharArray((LEITURAS_PESO[nxGraphComplete]*255)/((CEL_CARGA/1000)*9.81));
                                    NxGraphPlot('5','0',temp,1);
                                    heap_caps_free(temp);
                              }
                         }

                         break;  
                    case SSUMMARY:
                         if(oldState != SSUMMARY){
                              oldState = SSUMMARY;
                              NxPageSend("Summary");
                         }

                         temp = floatToCharArray(MIN*100);
                         NxValueSend("x0",temp);
                         heap_caps_free(temp);
                         temp=floatToCharArray(MED*100);
                         NxValueSend("x1",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(MAX*100);
                         NxValueSend("x2",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(DESV*100);
                         NxValueSend("x3",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(VARI*100);
                         NxValueSend("x4",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(TRAB*100);
                         NxValueSend("x5",temp);
                         heap_caps_free(temp);

                         break;                            
                    case SCFG:
                         if(oldState != SCFG){
                              oldState = SCFG;
                              temp = floatToCharArray(gDELAY);
                              NxTextSend("CFG.cb0",temp);   
                              heap_caps_free(temp);
                              temp = floatToCharArray(VMAX);
                              NxTextSend("CFG.cb1",temp);
                              heap_caps_free(temp);
                              temp = floatToCharArray(VUNITMM);
                              NxValueSend("CFG.cb2",temp);     
                              heap_caps_free(temp);                       
                              NxPageSend("CFG");
                         }
                         temp = floatToCharArray(CALIB_L);
                         NxValueSend("CFG.n0",temp);
                         heap_caps_free(temp);
                         temp = floatToCharArray(CALIB_H);
                         NxValueSend("CFG.n1",temp);    
                         heap_caps_free(temp);                       
                         break;


               }

               //size_t freeHeapSize = esp_get_free_heap_size();
              // printf("Memória livre disponível: %d bytes\n", freeHeapSize);                    
               vTaskDelay(pdMS_TO_TICKS(50));
    }
}


void nextionRxTask(void * params ){
    unsigned int nxnt = 0;
    char *temp;

   vTaskDelay(pdMS_TO_TICKS(2000));

    while(1){
          unsigned short addr = 0, dataaddr = 0;  
          xTaskNotifyWait(0, ULONG_MAX, &nxnt, pdMS_TO_TICKS(20));       

             if (LocateOnBuffer1_EOC("BTNSTART:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SMAIN;             
                START= (UART1_BUF[dataaddr] - 48);   
                ClearOnBuffer1("BTNSTART:",addr);
                ESP_LOGI("NX","STARTING/STOP: %d",START);
             }

             if (LocateOnBuffer1_EOC("BTN:SOBRE","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SSOBRE;                
                ClearOnBuffer1("BTN:SOBRE",addr);
                ESP_LOGI("NX","SOBRE");
             }

             if (LocateOnBuffer1_EOC("BTN:CFG","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SCFG;                
                ClearOnBuffer1("BTN:CFG",addr);
                ESP_LOGI("NX","CFG");
             }

             if (LocateOnBuffer1_EOC("BTN:CFVOLT","###",&addr,&dataaddr)){
                oldState = NxState;                
                NxState = SMAIN;
                ClearOnBuffer1("BTN:CFVOLT",addr);
                ESP_LOGI("NX","MAIN");
             }

           /*  if (LocateOnBuffer1_EOC("BTN:CFVOLT","###",&addr,&dataaddr)){
                NxState = 1;
                ESP_LOGI("NX","CFG");
             }   */   


             if (LocateOnBuffer1_EOC("TARA:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SMAIN;
                TARA =  NxTakeLongValue(dataaddr);
                TARA = TARA/100;

                if (TARA > 0 ){
                    NxSend("x0.bco=","64623");
                    NxSend("t2.bco=","64623");
                }else{
                    NxSend("x0.bco=","65535");
                    NxSend("t2.bco=","65535");
                }

                ESP_LOGI("NX","TARA: %f, %s", TARA, UART1_BUF);                
                ClearOnBuffer1("TARA:",addr);                                
             }    

             if (LocateOnBuffer1_EOC("BTVEL:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SMAIN;
                VEL = NxTakeLongValue(dataaddr);
                //VEL = TVELARA/100;
                temp = floatToCharArray(VEL);
                NxValueSend("x1",temp);
                heap_caps_free(temp);
                ESP_LOGI("NX","VELOCIDADE: %f, %s", VEL, UART1_BUF);                
                ClearOnBuffer1("BTVEL:",addr);                                
             }                

             if (LocateOnBuffer1_EOC("BTRMP:","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SMAIN;
                RAMPA = (UART1_BUF[dataaddr] - 48);
                //ESP_LOGI("NX","Rampa Status: %d, %s", RAMPA, UART1_BUF);                
                ClearOnBuffer1("BTRMP:",addr);                                
             }         


             if (LocateOnBuffer1_EOC("CFG:CAL1","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = NxState;
                CALIB_H = LOADCEL_RAW;                             
                ClearOnBuffer1("CFG:CAL1:",addr);                                
             }   


             if (LocateOnBuffer1_EOC("CFG:CALZ","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = NxState;
                CALIB_L = LOADCEL_RAW;                             
                ClearOnBuffer1("CFG:CALZ",addr);                                
             }                

             if (LocateOnBuffer1_EOC("BTN:APLIC","###",&addr,&dataaddr)){
                    oldState = NxState;
                    NxState = SMAIN;
                    ClearOnBuffer1("BTN:APLIC",addr);

                    ESP_LOGI("NX","CFG APLICADA");

                    //Aplicando configurações, significa que enviou dados que devem ser retirados do buffer e aplicados.
                    
                    if (LocateOnBuffer1_EOC("SDEL:","###",&addr,&dataaddr)){
                         gDELAY = NxTakeLongValue(dataaddr);
                         ESP_LOGI("NX","DELAY: %d, %s", gDELAY, UART1_BUF);                
                         ClearOnBuffer1("SDEL:",addr);                                
                    } 

                    if (LocateOnBuffer1_EOC("SVEL:","###",&addr,&dataaddr)){
                         VMAX = NxTakeLongValue(dataaddr);
                         ESP_LOGI("NX","MAX VELOCIDADE: %d", VMAX);                
                         ClearOnBuffer1("SVEL:",addr);                                
                    }

                    if (LocateOnBuffer1_EOC("BTNMM:","###",&addr,&dataaddr)){
                         VUNITMM = UART1_BUF[dataaddr]-48;
                         ESP_LOGI("NX","VEL EM mm: %d", VUNITMM);                
                         ClearOnBuffer1("BTNMM:",addr);                                
                    }

                    if (LocateOnBuffer1_EOC("SCZER:","###",&addr,&dataaddr)){
                         CALIB_L = NxTakeLongValue(dataaddr);
                         ESP_LOGI("NX","CAL MIN: %d", CALIB_L);                
                         ClearOnBuffer1("SCZER:",addr);                                
                    } 

                    if (LocateOnBuffer1_EOC("SCCAL:","###",&addr,&dataaddr)){                        
                         CALIB_H = NxTakeLongValue(dataaddr);
                         ESP_LOGI("NX","CAL MAX: %d", CALIB_H);                
                         ClearOnBuffer1("SCCAL:",addr);                                
                    }            

                    //Salva as configurações
                    saveCfgValues();
             }   

             if (LocateOnBuffer1_EOC("BTN:GRAPH","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SGRAPH;
                ESP_LOGI("NX","GRAPH");              
                ClearOnBuffer1("BTN:GRAPH:",addr);                                
             }

             if (LocateOnBuffer1_EOC("BTN:SUMM","###",&addr,&dataaddr)){
                oldState = NxState;
                NxState = SSUMMARY;
                ESP_LOGI("NX","SUMMARY");
                ClearOnBuffer1("BTN:SUMM:",addr);                                
             }

             if (LocateOnBuffer1_EOC("BTN:VOLT","###",&addr,&dataaddr)){
                oldState = NxState;
                switch (oldState)
                {
                    case SSOBRE:
                         NxState = SBOOT;
                         break;

                    case SGRAPH:
                         NxState = SMAIN;
                         break;
                    
                    case SSUMMARY:
                         NxState = SGRAPH;              
                         break;
                }
                ClearOnBuffer1("BTN:VOLT:",addr); 
             } 

        }        
}



void NxValueSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 12;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
    //uint8_t data_to_send[] = {0xFF,0xFF,0xFF};
   // uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 3);     
    /*uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/
     snprintf(buff, sizeof(buff), "%c%c%c%s.val=%s%c%c%c",  0xff, 0xff, 0xff,variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
     
 /*   uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 7;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
  /*  uint8_t data_to_send = 0xFF;
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/
     snprintf(buff, sizeof(buff), "%c%c%c%s%s%c%c%c", 0xff,0xff,0xff, variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
     
 /*   uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxTextSend(const char variable[], const char value[]) {

     
     int l = charVarSize(variable) + charVarSize(value) + 14;// (size_t)(sizeof(variable)) +  (size_t)((value)) + 7;
     
     char buff[l];
   /* uint8_t data_to_send = 0xFF;
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/
     snprintf(buff, sizeof(buff), "%c%c%c%s.txt=\"%s\"%c%c%c", 0xff, 0xff, 0xff, variable, value, 0xff, 0xff, 0xff);
     uart_write_bytes(UART_NUM_1,buff,sizeof(buff));
   /* uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
    uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     */
     
}


void NxPageSend(const char page[]) {
     
          int l = charVarSize(page)+12;
          char buff[l];

         // uint8_t data_to_send = 0xFF;

         /* uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
          uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/

          snprintf(buff, sizeof(buff), "%c%c%cpage %s%c%c%c",  0xff, 0xff, 0xff, page, 0xff, 0xff, 0xff);
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
     int l = charVarSize(value) +  15;
    char buff[l];

   //  uint8_t data_to_send = 0xFF;

    /* uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);     
     uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);
     uart_write_bytes(UART_NUM_1, (const char *)&data_to_send, 1);*/

    snprintf(buff, sizeof(buff), "%c%c%cadd %c,%c,%s%c%c%c",  0xff, 0xff, 0xff, graphID, can, value, 0xff, 0xff, 0xff);
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
    int requiredSize = 0;
    char* result;

    requiredSize = snprintf(NULL, 0, "%d", intValue) + 1; // Tamanho necessário, incluindo terminador nulo
    //result = malloc((requiredSize +1) * sizeof(char)); // Aloca memória dinamicamente
    result = heap_caps_malloc((requiredSize +1) * sizeof(char),MALLOC_CAP_8BIT); // Aloca memória dinamicamente

    if (result != NULL) {
        snprintf(result, requiredSize, "%d", intValue); // Formata o valor inteiro na string
        return result;
    }else{
     return "00";     
    }    
}