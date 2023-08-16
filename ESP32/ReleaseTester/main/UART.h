#ifndef __UART_H
    #define __UART_H

    #include <stdio.h>
    #include "esp_log.h"
    #include "driver/uart.h"
    #include "string.h"    
    #include "global_variables.h"



    #define RX_BUX_SIZE 256
    #define BUFFL RX_BUX_SIZE

    char UART1_BUFHW[RX_BUX_SIZE];
    char UART1_BUF[RX_BUX_SIZE];

    


    //NEXTION
    #define TXD1_PIN 25
    #define RXD1_PIN 26   

    void uartTask(void * params);
    bool LocateOnBuffer_EOC(const char compare[], const char EOC[], unsigned short *addr, unsigned short *dataAddr);
    void ClearOnBuffer(const char term[], int addr);

#endif