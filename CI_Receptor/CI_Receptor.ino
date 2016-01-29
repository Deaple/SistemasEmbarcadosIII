#include "RCSwitch.h"

#define DESLOCAMENTO_RFID   24
#define RFID_LIM_INF  95  
#define RDIF_LIMP_SUP 105 

RCSwitch receptor = RCSwitch();

long infoRF;
void setup(){
  Serial.begin(9600);
  receptor.enableReceive(0);  
}

void enviarUSB(){
  Serial.write("I");
  Serial.write((uint8_t*)&infoRF,sizeof(infoRF));
  Serial.write("F");
}

boolean RFIDValido(long rf){ 
  boolean valido = false;
  int id = (rf & 2130706432)>> DESLOCAMENTO_RFID;
  if((id >= RFID_LIM_INF) && (id <= RDIF_LIMP_SUP)){
    valido = true;
  }
  return valido;
}

void loop(){
  if(receptor.available()){
    infoRF = receptor.getReceivedValue();
    if(infoRF != -1){
      if(RFIDValido(infoRF)){
         enviarUSB();       
      } 
    }   
    receptor.resetAvailable();
  }
  delay(1000);
}
