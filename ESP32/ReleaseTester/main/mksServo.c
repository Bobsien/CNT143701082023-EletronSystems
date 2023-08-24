#include "mksServo.h"
#include "global.h"  
#include "UART.h"

//Faz a calibração do encoder
uint8_t cal[]        = {0xE0, 0x80, 0x00, 0x60}; //calibra o encoder

//Seleciona o tipo de motor
uint8_t motType_18[]  = {0xE0, 0x81, 0x01, 0x62}; //calibra o encoder
uint8_t motType_09[]  = {0xE0, 0x81, 0x00, 0x61}; //calibra o encoder

//Seleciona o modo de operação
uint8_t mode_CR_OPEN[] = {0xE0, 0x82, 0x00, 0x62}; //calibra o encoder
uint8_t mode_CR_vFOC[] = {0xE0, 0x82, 0x01, 0x63}; //calibra o encoder
uint8_t mode_CR_UART[] = {0xE0, 0x82, 0x02, 0x64}; //calibra o encoder

//Seteciona a corrente do motor
uint8_t mA_1200[]         = {0xE0, 0x83, 0x06, 0x69}; //(1200ma)

//Seleciona as subdivsões do motor
uint8_t mStep[]      = {0xE0, 0x84, 0x1A, 0x7E}; //calibra o encoder


uint8_t en[]         = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t dir[]        = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t autoSDD[]    = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t protect[]    = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t mPlyer[]     = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t uartBaud[]   = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t uartAddr[]   = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t zeroMode[]   = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t setZero[]    = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t zeroSpeed[]  = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t zeroDir[]    = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t gotoZero[]   = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t acc[]        = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t restore[]    = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder
uint8_t exict[]      = {0xE0, 0xF3, 0x01, 0xD4}; //calibra o encoder

uint8_t enbOn[]  = {0xFA, 0x01, 0xF3, 0x01, 0xEF};
uint8_t enbOff[] = {0xFA, 0x01, 0xF3, 0x00, 0xEE};
uint8_t runMotorHMax[] = {0xE0, 0xF6, 0xFF, 0xD5};
uint8_t runMotorAhMax[] = {0xE0, 0xF6, 0x7F, 0x55};
uint8_t disMotor[] = {0xE0, 0xFF, 0xC8, 0xA7}; //desabilita motor e zera contador
uint8_t stopMotor[] = {0xE0, 0xF7, 0xD7};


//uint16_t runMotorSpeedPul[] = {0xE0, 0xFD, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0x5A}; //Run the motor speed/pulsos
                      //{m57C, addr, Function, data0, data0, data0, data0, data0, crc};//Composição do array - Utilizamos struct mksServoFrame_t
                      //OBS: O campo m57C serve como indicador se está utilizando um 57C ou 42C. Quando em '1', o mksSendCommand não enviará o header
uint8_t runMotorCmd[] = {0   , 0   , 0       , 0    , 0    , 0    , 0    , 0    , 0  }; //Run the motor speed/pulsos
//uint8_t runMotorSpeedPul[] = {0xE0, 0xFD, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x5A}; //Run the motor speed/pulsos

uint8_t runMotorSpeedCos[] = {0x00, 0x00, 0x00, 0x00};


void mksInit(){

  mksSendCommand(enbOff);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mksSendCommand(mode_CR_vFOC);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mksSendCommand(mStep);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mksSendCommand(stopMotor);  //para o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  //calculo a velocidade em m/min
  vRpm = mksSpeedCalc(0);
}

//Envia comando "Speed Mode"
void mksSendSpeedCommand(mksServoFrame_t cmd){ 

   uint8_t retCmd[7+m57Cmodel];

   if(m57Cmodel){
        retCmd[0] = cmd.head;        
   }
   retCmd[0+m57Cmodel] = cmd.addr;
   retCmd[1+m57Cmodel] = cmd.Function;
   retCmd[2+m57Cmodel] = ((cmd.dir&0b1) << 8) + ((cmd.rev & 0b00000111)<<4) + ((cmd.speed >> 8)&0b00001111);
   retCmd[3+m57Cmodel] = cmd.speed&0b0000000011111111;
   retCmd[4+m57Cmodel] = cmd.acc;
   
   //Calcula CRC com base no Frame de comando
   retCmd[5+m57Cmodel] = mksCRC(cmd);

   //Envia comando
   mksSendCommand(retCmd);
}

//Função para envio de comandos
void mksSendCommand(uint8_t commands[]){
     int l = byteVarSize(commands)+1;
     
    char buff[l];    
    snprintf(buff, sizeof(buff), "%s", commands);
    uart_write_bytes(UART_NUM_2,buff,sizeof(buff));          
}



//Realiza calculo de velocidade convertendo velocidade linear em RPM e posteriormente no valor recebido pelo motor.
int mksSpeedCalc(long vel){
    int speed;
    int Vrpm = (int)(vel)/(pi*diametro);
    speed = (Vrpm * Mstep * 200) / 6000;
    return speed;
}


//***********Função que calcula o CRC**************
uint8_t mksCRC(mksServoFrame_t cmd) {  //cria a função com o valor e o tamanho
   uint8_t soma = 0, crc = 0;

   uint8_t retCmd[7+m57Cmodel];

   if(m57Cmodel){
        retCmd[0] = cmd.head;        
   }
   retCmd[0+m57Cmodel] = cmd.addr;
   retCmd[1+m57Cmodel] = cmd.Function;
   retCmd[2+m57Cmodel] = ((cmd.dir&0b1) << 8) + ((cmd.rev & 0b00000111)<<4) + ((cmd.speed >> 8)&0b00001111);
   retCmd[3+m57Cmodel] = cmd.speed&0b0000000011111111;
   retCmd[4+m57Cmodel] = cmd.acc;

                   //cria a variavel soma
  for (uint8_t i = 0; i <= (4+m57Cmodel); i++) {  //subtrai 1 do tamano para deixar livre o local do CVR
    soma += retCmd[i];                            //faz a soma de todos os valores anteriores ao CRV
  }                                               //fim fo for
  crc = soma & 0xFF;                              //cria a variavel CRV e faz uma logica AND com OxFF para obter o valor de CRV
  crc = crc & 0xFF;                               //executa novamente a logica AND com 0xFF
  return crc;                                     //retorna o valor de CRV
}  //fim da função d CRV