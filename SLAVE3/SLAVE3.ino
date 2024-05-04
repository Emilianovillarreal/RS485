//SLAVE MOTOR

#include <HardwareSerial.h>

HardwareSerial SerialPort(2);
//************************************************************

#define x_paso 15   // Define el Pin de STEP para Motor de eje X
#define x_dire 2    // Define el Pin de DIR  para Motor de eje X
#define x_habi 4    // Define el Pin de ENABLE  para Motor de eje X

int retardo = 3000;   // Menor numero el giro es mas rapido
int tiempo = 2000;   // durante cuanto timpo da el giro el motor (vueltas)
//************************************************************
const byte slave_3_id = 0b11;
//************************************************************

int Slave;
const int Enable =  2;


void setup() {
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  SerialPort.setTimeout(250);

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(x_paso, OUTPUT); pinMode(x_dire, OUTPUT); pinMode(x_habi, OUTPUT); 


}

void loop() 
{
  digitalWrite(Enable, LOW); 
  if(SerialPort.available())
  {
    SerialPort.flush();
    byte trama = SerialPort.read();
    Serial.println((trama>>6)&0b11);
    if(slave_3_id == ((trama>>6)&0b11))
    {   
      byte command =(trama>>4)&0b11;  
      Serial.println(command);
      if(command == 0b11)
      {
        giro(x_paso,x_dire,x_habi,1);
        delay(500);
        giro(x_paso,x_dire,x_habi,0);
        delay(500);
          
      }
      digitalWrite(Enable, HIGH);
      SerialPort.print("Slave 1 is triggered"); //
      SerialPort.flush();
    }
           //Serial.println("loop");/
  }   
}
  



void giro(int paso_,int dire_,int habi_,int dir) {
   digitalWrite(habi_, LOW);  // Habilita el Driver
  if( dir==0){ 
   digitalWrite(dire_, LOW);   // direccion de giro 0
   for(int i=0;i<tiempo;i++){  // da  pasos por un tiempo  
    digitalWrite(paso_, HIGH);      
    delayMicroseconds(retardo);          
    digitalWrite(paso_, LOW);       
    delayMicroseconds(retardo); 
   }
  }
  if( dir==1){ 
  digitalWrite(dire_, HIGH);   // direccion de giro 1
  for(int i=0;i<tiempo;i++){   // da  pasos por un tiempo  
    digitalWrite(paso_, HIGH);      
    delayMicroseconds(retardo);          
    digitalWrite(paso_, LOW);       
    delayMicroseconds(retardo);  
   }
  }
  digitalWrite(habi_, HIGH);   // quita la habilitacion del Driver

}
