#ifndef __NEXTION_H
   #define __NEXTION_H


   #include "global_variables.h"   

   void nextionTask(void * params);
   void NxValueSend(const char variable[], const char value[]);
   void NxTextSend(const char variable[], const char value[]);
   void NxPageSend(const char page[]);
   void NxGraphPlot(const char graphID, const char can, const char value[], int hscale);  
   long NxTakeLongValue(int dataaddr);
   void NxSend(const char variable[], const char value[]);
   char* floatToCharArray(float value);

#endif