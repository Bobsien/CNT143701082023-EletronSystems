#ifndef __NEXTION_H
   #define __NEXTION_H


   #include "global.h"   

   int nxGraphMain;
   int nxGraphComplete;

   void nextionTxTask(void * params);
   void nextionRxTask(void * params);
   void NxValueSend(const char variable[], const char value[]);
   void NxTextSend(const char variable[], const char value[]);
   void NxPageSend(const char page[]);
   void NxGraphPlot(const char graphID, const char can, const char value[], int hscale);  
   long NxTakeLongValue(int dataaddr);
   void NxSend(const char variable[], const char value[]);
   char* floatToCharArray(float value);

#endif