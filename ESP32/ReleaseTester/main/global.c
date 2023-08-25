/*  Neste arquivo encontram-se funções de uso geral não categorizadas do projeto. */

#include "global.h"
#include "math.h"



//Esta função retorna apenas o tempo total que o teste vai levar para ser concluido em ms
int totalTestTimeMs(float vel){
    float mm_min = 0,
         mm_seg = 0;
    float factor = 0,
          time_ms = 0;          
    
    //Primeiro, ajustamos a unidade de medida para mm/min
    if (VUNITMM){
       mm_min = vel; 
    }else{
        //Caso esteja em m/min
       mm_min = vel*1000;
    }
    //Agora, verificamos quanto tempo proporcional a 1 minuto irá gastar para concluir o teste
    //onde "COMPRIMENTO_TESTE" é o comprimento de fita a ser testado 
    mm_seg = mm_min/60;
    factor = COMPRIMENTO_TESTE/mm_seg; 
    //Aplicando o fator (% de tempo de 1 minuto) sobre 60 segundos e convertendo para ms
    time_ms = factor*1000; //Como resultado, temos o tempo (em ms) que levará para concluir o teste
    return (int)time_ms;
}

int cicleReadingTimeMs(float vel){    
    float time_ms = 0,
          cicle = 0;

    time_ms = totalTestTimeMs(vel);
    cicle = (float)time_ms/N_LEITURAS;

    if(cicle < 13){
        return 13;
    }else{
        return (int)cicle;
    }
}

float minValueOnArray(float array[]){
    float min = CEL_CARGA+1;
    for(int i=0;array[i]!='\0';i++){
        if(min > array[i]){
            min = array[i];
        }
    }

    return min;
}

float maxValueOnArray(float array[]){
    float max = 0;
    for(int n=0;array[n]!='\0';n++){
        if(max < array[n]){
            max = array[n];
        }
    }

    return max;
}


float meanValueOnArray(float array[]){
    float acc = 0;
    int n = 0;
    for(n=0;array[n]!='\0';n++){
        acc=array[n]+acc;
    }

    return acc/n;
}

float varianceOnArray(float array[]){
    int n = 0;
    float variance = 0.0;
    float mean = meanValueOnArray(array);

    for(n=0;array[n]!='\0';n++){
        variance += pow(array[n] - mean, 2);
    }
    
    return variance / (n-1);
}

float stdDevOnArray(float array[]){
    //float mean = meanValueOnArray(array);
    float variance = varianceOnArray(array);
    return sqrt(variance);
}

float meanWorkTestResult(float array[]){
    float mean = meanValueOnArray(array);    
    return mean*((float)COMPRIMENTO_TESTE/1000);
}

//Função responsavel por realizar os calculos de sumarização de resultados
void summarize(){
    MAX = maxValueOnArray(LEITURAS_PESO);
    MIN = minValueOnArray(LEITURAS_PESO);
    MED = meanValueOnArray(LEITURAS_PESO);
    DESV = stdDevOnArray(LEITURAS_PESO);
    VARI = varianceOnArray(LEITURAS_PESO);
    TRAB  = meanWorkTestResult(LEITURAS_PESO);    
}

//Esta função tem objetivo armazenar os valores lidos no array LEITURAS_PESO de forma escalonada
void storeReadedData(float val, int cycle){
  int cycleMax = totalTestTimeMs(VEL)/cicleReadingTimeMs(VEL);  //Realiza o calculo de quantos ciclos de leitura serão viáveis em relação ao tempo que o teste irá levar
  
   //Estacola as leituras
   //Temos um arrauy com X espaços (780, por exemplo)
   //Se o conseguimos realizar apenas 4 leituras devido a velocidade ser alta demais, precisamos preencher os espaços vazios do array
   //com valores repetidos
   int scaling = 0;
   if(cycleMax >= 780) {
        scaling = 1;
   }else{
        scaling = N_LEITURAS/cycleMax;    //Aqui, verificamos a taxa de repetição
   }
  
  int next = scaling*(cycle+1);         //Aqui, verificamos qual a proxima fatia a ser populada

  //Verificamos se a proxima fatia não está alem do máximo do array
  if(next > N_LEITURAS){
    next = N_LEITURAS;
  }

  //Populamos o Array repetindo leituras conforme escala necessária
  for (int i = cycle*scaling; i < next; i++){
    LEITURAS_PESO[i] = val;
   // ESP_LOGI("STORE","PESO: %f",val);
  }
}

//Esta função realiza o salvamento de configurações do equipamento em memoria interna
void saveCfgValues(){
    saveValue("FACT",128);
    saveValue("DDELAY",gDELAY);
    saveValue("VUNITMM",VUNITMM);
    saveValue("VMAX",VMAX);
    saveValue("RAMPA",RAMPA);
    saveLongValue("CALIB_H",CALIB_H);
    saveLongValue("CALIB_L",CALIB_L);    
}

//Esta função realiza a carga de configurações do equipamento a partir da memnoria Flash interna
void loadCfgValues(){
    if( getValue("FACT") ==128){
        gDELAY = getValue("DDELAY");
        VUNITMM = getValue("VUNITMM");
        VMAX = getValue("VMAX");
        RAMPA = getValue("RAMPA");
        CALIB_H = getLongValue("CALIB_H");
        CALIB_L = getLongValue("CALIB_L");
    }else{
        saveCfgValues();
    }
}

//Funções para gravação e recuperação de dados em FLASH
void saveValue(const char *key, int value) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err == ESP_OK) {
        nvs_set_i32(nvsHandle, key, value);
        nvs_commit(nvsHandle);
        nvs_close(nvsHandle);
    }
}

int getValue(const char *key) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err == ESP_OK) {
        int value;
        nvs_get_i32(nvsHandle, key, &value);
        nvs_close(nvsHandle);
        return value;
    }
    return -1;
}

void saveLongValue(const char *key, long value) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err == ESP_OK) {
        nvs_set_i64(nvsHandle, key, value);
        nvs_commit(nvsHandle);
        nvs_close(nvsHandle);
    }
}

long getLongValue(const char *key) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err == ESP_OK) {
        int64_t value;
        nvs_get_i64(nvsHandle, key, &value);
        nvs_close(nvsHandle);
        return value;
    }
    return -1;
}

