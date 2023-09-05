#ifndef __UART_H
    #define __UART_H

    #include <stdio.h>
    #include "esp_log.h"
    #include "driver/uart.h"
    #include "string.h"    
    #include "global.h"



    #define RX_BUX_SIZE 256
    #define BUFFL RX_BUX_SIZE

    char UART1_BUFHW[RX_BUX_SIZE];
    char UART1_BUF[RX_BUX_SIZE];

    char UART2_BUFHW[RX_BUX_SIZE];
    char UART2_BUF[RX_BUX_SIZE];

    


    //NEXTION
    #define TXD1_PIN 25
    #define RXD1_PIN 26   
    //MOTOR
    #define TXD2_PIN 33
    #define RXD2_PIN 32       

    void uart1Task(void * params);
    void uart2Task(void * params);
    bool LocateOnBuffer1_EOC(const char compare[], const char EOC[], unsigned short *addr, unsigned short *dataAddr);
    void ClearOnBuffer1(const char term[], int addr);
    int charVarSize(const char var[]);
    int byteVarSize(const uint8_t var[]);
    int floatVarSize(const float var[]);

#endif