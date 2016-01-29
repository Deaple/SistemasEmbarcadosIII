#include <Wire.h>
#include "RCSwitch.h"
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN A2
//funcionando com e sem o MIDDLE
//#define MIDDLEWARE

//verificar uso com 2 pinos
#define PINO_DETEC_CHAMAS A0
#define PINO_DETEC_GAS    A3

#define RFID_LIM_INF  95  
#define RDIF_LIMP_SUP 105 

//Num max de bits usados por cada variavel - informacoes de deslocamento
#define DESLOCAMENTO_RFID   24 
#define DESLOCAMENTO_TEMP   16 
#define DESLOCAMENTO_GAS    8  
//DESLOCAMENTO_CHAMAS         

#ifdef MIDDLEWARE
struct InfoRF{
  int id;           //int
  int temperatura;  //float
  int chamas;       //boolean
  int gas;          //int 
} infoRF;
#else
  long infoRF;
#endif

RCSwitch emissor = RCSwitch();
//Implementar objeto do sensor de temperatura
DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(9600);
  emissor.enableTransmit(4);//Pino digital usado para transmissao
  //receptor.enableReceive(0);//Pino de interrupcao que identifica informacao no sensor  
  //implementar inicializacao do sensor de temperatura
  dht.begin();
}

long lerSensoresRF(){
  long chamas = analogRead(PINO_DETEC_CHAMAS);
  long gas = analogRead(PINO_DETEC_GAS);
  long temperatura = (int)dht.readTemperature();
  
  //Variacao do potenciometro
  chamas = map(chamas, 0, 1023, 0, 100);
  gas    = map(gas, 0, 1023, 0, 175);
  
  long rf = RFID_LIM_INF; //valor mininmo do RFID ou 1º RFID
  long info = info | (rf << DESLOCAMENTO_RFID);  
  info = info | (temperatura << DESLOCAMENTO_TEMP);
  info = info | (gas << DESLOCAMENTO_GAS);  
  info = info | chamas;
  
  return info;
}

//Verifica se o RFID esta dentro da faixa aceita
#ifdef MIDDLEWARE
boolean RFIDValido(long rf){
  
  boolean valido = false;
  #ifdef MIDDLEWARE
  long info = rf;
  infoRF.id = (info & 2130706432)>> DESLOCAMENTO_RFID;
  if((infoRF.id >= RFID_LIM_INF) && (infoRF.id <= RDIF_LIMP_SUP)){
    valido = true;
  }
  #else
  int id = (rf & 2130706432)>> DESLOCAMENTO_RFID;
  if((id >= RFID_LIM_INF) && (id <= RDIF_LIMP_SUP)){
    valido = true;
  }
  #endif
  return valido;
}

//Funcoes que extraem os dados lidos nos sensores da informacao Long
int extrairGas(long info){
  int gas = (info & 65280) >> DESLOCAMENTO_GAS;
  return gas>100?100:gas;
}

int extrairChamas(long info){
  int chamas = (info & 255);
  return chamas;
}

int extrairTemperatura(long info){
  int temperatura = (info & 16711680) >> DESLOCAMENTO_TEMP; 
  return temperatura;
}
#endif


void emitirPeloRF(long info){
  emissor.send(info,32);
}

void loop(){
   long info = lerSensoresRF();
   emitirPeloRF(info);
   delay(50);
  //Visualizacao das informacoes - opcional
  /*if(RFIDValido(info)){
      infoRF.temperatura = extrairTemperatura(info);
      infoRF.chamas = extrairChamas(info);
      infoRF.gas = extrairGas(info);  
      Serial.print("RF = ");
      Serial.println(infoRF.id);
      Serial.print("Temp = ");
      Serial.println(infoRF.temperatura);
      Serial.print("Chamas = ");
      Serial.println(infoRF.chamas);
      Serial.print("Gas = ");
      Serial.println(infoRF.gas);
  }*/
}
