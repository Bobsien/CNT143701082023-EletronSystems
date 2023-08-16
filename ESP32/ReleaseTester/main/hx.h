#ifndef __HX_H
    #define __HX_H

    //Pinos HX711
    #define PD_SCK_GPIO 16
    #define DOUT_GPIO 17

    #define CALBALP01 194
    #define CALBALV01 -2063498
    #define CALBALV00 -771108       

    //Prot. de funcao
    void hxTask(void *params);
    float hx711ConvVal(long RAW);
#endif