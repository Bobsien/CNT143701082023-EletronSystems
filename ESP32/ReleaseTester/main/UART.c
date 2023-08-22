#include "UART.h"



void uart1Task(void * params)
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
          memset(UART1_BUF, 0, sizeof(UART1_BUF)); //Limpa buffer Nextion (utilizado para tratamento de dados)
          snprintf(UART1_BUF, sizeof(UART1_BUFHW), UART1_BUFHW);
          memset(UART1_BUFHW, 0, sizeof(UART1_BUFHW)); //Limpa buffer de hardware após transferido para buffer de tratamento
          xTaskNotify(nxNotify,0b00001,eSetBits);             
        }
    }
}


void uart2Task(void * params)
{
    memset(UART2_BUFHW, 0, sizeof(UART2_BUFHW));

    uart_config_t uart2Conf = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_2,&uart2Conf);
    uart_set_pin(UART_NUM_2,TXD2_PIN,RXD2_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2,RX_BUX_SIZE,0,0,NULL,0);

    while(1){
        if (uart_read_bytes(UART_NUM_2,(uint8_t *) UART1_BUFHW,RX_BUX_SIZE,pdMS_TO_TICKS(20)) > 0 ){
          memset(UART2_BUF, 0, sizeof(UART2_BUF)); //Limpa buffer Nextion (utilizado para tratamento de dados)
          snprintf(UART2_BUF, sizeof(UART2_BUFHW), UART2_BUFHW);
          memset(UART2_BUFHW, 0, sizeof(UART2_BUFHW)); //Limpa buffer de hardware após transferido para buffer de tratamento
          xTaskNotify(motorNotify,0b00001,eSetBits);             
        }
    }
}           

//Função utilizada para localização de comandos com EOC no Buffer da UART 1 (para Nextion)
bool LocateOnBuffer1_EOC(const char compare[], const char EOC[], unsigned short *addr, unsigned short *dataAddr) {
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



void ClearOnBuffer1(const char term[], int addr) {
  int lenght = strlen(term);

  for ( unsigned int i = (unsigned int)addr; i <= (unsigned int)lenght + (unsigned int)addr; i++) {
    UART1_BUF[i] = (char)' ';
  }
  

}

//Função para verificação de tamanho de array CHAR em tempo de execução
//Utilizada para contornar limitações
int charVarSize(const char var[]){
    size_t variable_size = 0;

    while (var[variable_size] != '\0') {
        variable_size++;
    }

    return variable_size;
}

//Função para verificação de tamanho de array UINT8 em tempo de execução
//Utilizada para contornar limitações
int byteVarSize(const uint8_t var[]){
    size_t variable_size = 0;

    while (var[variable_size] != '\0') {
        variable_size++;
    }

    return variable_size;
}