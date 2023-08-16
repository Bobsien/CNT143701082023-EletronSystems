#include "UART.h"



void uartTask(void * params)
{
    memset(UART1_BUFHW, 0, sizeof(UART1_BUFHW));

    uart_config_t uart1Conf = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1,&uart1Conf);
    uart_set_pin(UART_NUM_1,TXD1_PIN,RXD1_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1,RX_BUX_SIZE,0,0,NULL,0);

    while(1){
        if (uart_read_bytes(UART_NUM_1,(uint8_t *) UART1_BUFHW,RX_BUX_SIZE,pdMS_TO_TICKS(20)) > 0 ){
          snprintf(UART1_BUF, sizeof(UART1_BUFHW), UART1_BUFHW);
          xTaskNotify(nxNotify,0b00001,eSetBits);             
        }
    }
}


           

bool LocateOnBuffer_EOC(const char compare[], const char EOC[], unsigned short *addr, unsigned short *dataAddr) {
  unsigned int datasize = 0, addrLocated = 0; //Add addrLocated no dia 31/10
  unsigned short located = 0;

    datasize = strlen(compare);
    char *position = strstr(UART1_BUF, compare);

    if (position != NULL) {    //Se apos a varredura, located ainda 1
      addrLocated = position - (char *)UART1_BUF;
      located = 1;
     // ESP_LOGI("AQUI","1");
     // break;              //Para o laço principal pois localizou a sequencia de caracteres
    }
  
  
  if (located == 1) {
        
    char *positionEOC = strstr(UART1_BUF, EOC);
    if (positionEOC != NULL){
        located = 1;
       // ESP_LOGI("AQUI","2");
    }else{
      located = 0;
     // ESP_LOGI("AQUI","2.1");
    }

    
  
  }

    //Adicionado dia 31/10 para correção de um BUG. Verificar se demnais rotinas ainda funcionam corretamente
    if (located == 0) {
      //ESP_LOGI("AQUI","3");
      datasize = 0;
    }    
  //delay(1);

  *addr = (unsigned short)addrLocated;
  *dataAddr = (unsigned short)(addrLocated + datasize);
  return located;
}



void ClearOnBuffer(const char term[], int addr) {
  int lenght = strlen(term);

  for ( unsigned int i = (unsigned int)addr; i <= (unsigned int)lenght + (unsigned int)addr; i++) {
    UART1_BUF[i] = (char)' ';
  }
  

}