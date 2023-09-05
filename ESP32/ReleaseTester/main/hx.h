#ifndef __HX_H
    #define __HX_H

    //Pinos HX711
    #define PD_SCK_GPIO 16
    #define DOUT_GPIO 17

    #define PESO_BASE_CALIB 1000        //PESO BASE UTILIZADO PARA CALIBRAÇÃO
   // #define CALBALV01 -2063498
  //  #define CALBALV00 -771108   

    int32_t LOADCEL_RAW;    

    //Prot. de funcao
    void hxTask(void *params);
    float hx711ConvVal(long RAW);
#endif