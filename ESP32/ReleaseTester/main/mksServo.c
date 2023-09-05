#include "mksServo.h"
#include "global.h"  
#include "UART.h"

//Faz a calibração do encoder
uint8_t cal[]        = {SERVOADDR, 0x80, 0x00, 0x60}; //calibra o encoder

//Seleciona o tipo de motor
uint8_t motType_18[]  = {SERVOADDR, 0x81, 0x01, 0x62}; //calibra o encoder
uint8_t motType_09[]  = {SERVOADDR, 0x81, 0x00, 0x61}; //calibra o encoder

//Seleciona o modo de operação
uint8_t mode_CR_OPEN[] = {SERVOADDR, 0x82, 0x00, 0x62}; //calibra o encoder
uint8_t mode_CR_vFOC[] = {SERVOADDR, 0x82, 0x01, 0x63}; //calibra o encoder
uint8_t mode_CR_UART[] = {SERVOADDR, 0x82, 0x02, 0x64}; //calibra o encoder

//Seteciona a corrente do motor
uint8_t mA_1200[]         = {SERVOADDR, 0x83, 0x06, 0x69}; //(1200ma)

//Seleciona as subdivsões do motor
uint8_t mStep[]      = {0xFA, SERVOADDR, 0x84, Mstep, 0}; //Configura MSTEP


uint8_t en[]         = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t dir[]        = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t autoSDD[]    = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t protect[]    = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t mPlyer[]     = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t uartBaud[]   = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t uartAddr[]   = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t zeroMode[]   = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t setZero[]    = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t zeroSpeed[]  = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t zeroDir[]    = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t gotoZero[]   = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t acc[]        = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t restore[]    = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder
uint8_t exict[]      = {SERVOADDR, 0xF3, 0x01, 0}; //calibra o encoder

uint8_t enbOn[]  = {0xFA, 0x01, 0xF3, 0x01, 0xEF};
uint8_t enbOff[] = {0xFA, 0x01, 0xF3, 0x00, 0xEE};
uint8_t runMotorHMax[] = {SERVOADDR, 0xF6, 0xFF, 0xD5};
uint8_t runMotorAhMax[] = {SERVOADDR, 0xF6, 0x7F, 0x55};
uint8_t disMotor[] = {SERVOADDR, 0xFF, 0xC8, 0xA7}; //desabilita motor e zera contador
uint8_t stopMotor[] = {SERVOADDR, 0xF7, 0xD7};


//uint16_t runMotorSpeedPul[] = {SERVOADDR, 0xFD, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0x5A}; //Run the motor speed/pulsos
                      //{m57C, addr, Function, data0, data0, data0, data0, data0, crc};//Composição do array - Utilizamos struct mksServoFrame_t
                      //OBS: O campo m57C serve como indicador se está utilizando um 57C ou 42C. Quando em '1', o mksSendCommand não enviará o header
uint8_t runMotorCmd[] = {0   , 0   , 0       , 0    , 0    , 0    , 0    , 0    , 0  }; //Run the motor speed/pulsos
//uint8_t runMotorSpeedPul[] = {SERVOADDR, 0xFD, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x5A}; //Run the motor speed/pulsos

uint8_t runMotorSpeedCos[] = {0x00, 0x00, 0x00, 0x00};


void mksInit(){

  mksSendCommand(enbOff);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mksSendCommand(mode_CR_UART);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mStep[4] = mksCRC(mStep);
  mksSendCommand(mStep);        //desabilita o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  mksSendCommand(stopMotor);  //para o motor na inicialização
  vTaskDelay(pdMS_TO_TICKS(100));                           //delau de 100ms par ao motor processar os dados

  //calculo a velocidade em m/min
  vRpm = mksSpeedCalc(0);
}

//Envia comando "Speed Mode"
void mksSendSpeedCommand(mksServoFrame_t cmd){    

   uint8_t retCmd[4+(m57Cmodel*3)];

   if(m57Cmodel){
        retCmd[0] = cmd.head;           
        retCmd[1] = cmd.addr;
        retCmd[2] = cmd.Function;
        retCmd[3] = ((cmd.dir&0b1) << 8) + ((cmd.rev & 0b00000111)<<4) + ((mksSpeedCalc(cmd.speed) >> 8)&0b00001111);
        retCmd[4] = mksSpeedCalc(cmd.speed)&0b0000000011111111;
        retCmd[5] = cmd.acc;
        retCmd[6] = mksCRC(retCmd);
    }else{               
        retCmd[0] = cmd.addr;
        retCmd[1] = cmd.Function;       
        retCmd[2] = ((cmd.dir&0b1) << 8) + (mksSpeedCalc(cmd.speed)&0b0000000001111111);        
        retCmd[3] = mksCRC(retCmd);
    }
   
   
   //Calcula CRC com base no Frame de comando
   ESP_LOGI("MKS","VEL: %d",mksSpeedCalc(cmd.speed));

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
    float speedFactor = 0;
    int MDEG = 0;
    if(m57Cmodel){
       speedFactor = 6000;
    }else{
      speedFactor = 30000;
    }

    if(DEGREE18M){
      MDEG=200;
    }else{
      MDEG=400;
    }

    int speed;
    float Vrpm = (float)(vel)/(pi*diametro);
    speed = (Vrpm * Mstep * MDEG) / speedFactor;
    return speed;
}


//***********Função que calcula o CRC**************
uint8_t mksCRC(const uint8_t CMD[]) {  //cria a função com o valor e o tamanho
   uint8_t soma = 0, crc = 0;

  
                   //cria a variavel soma
  for (uint8_t i = 0; i < byteVarSize(CMD); i++) {  //subtrai 1 do tamano para deixar livre o local do CVR
    soma += CMD[i];                            //faz a soma de todos os valores anteriores ao CRV
  }                                               //fim fo for
  crc = soma & 0xFF;                              //cria a variavel CRV e faz uma logica AND com OxFF para obter o valor de CRV
  crc = crc & 0xFF;                               //executa novamente a logica AND com 0xFF
  return crc;                                     //retorna o valor de CRV
}  //fim da função d CRV