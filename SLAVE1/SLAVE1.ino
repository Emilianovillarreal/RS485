//SLAVE 1 - LED
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);
//************************************************************

const byte slave_1_id = 0b01;
//************************************************************
const int Enable =  2;
const int LED = 4;

int Slave;

void setup() 
{ 
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  SerialPort.setTimeout(250);

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW);
} 
void loop() 
{
  digitalWrite(Enable, LOW); 
  if(SerialPort.available())
  {
      SerialPort.flush();
      byte trama = SerialPort.read();
      Serial.println((trama>>6)&0b11);
      if(slave_1_id == ((trama>>6)&0b11))
      {   
        byte command =(trama>>4)&0b11;  
          Serial.println(command);
           if(command == 0b01)
           {
              digitalWrite(LED, HIGH);
              delay(500);
              digitalWrite(LED, LOW);
            }
              digitalWrite(Enable, HIGH);
              SerialPort.print("Slave 1 is triggered"); //
              SerialPort.flush();
           }
           //Serial.println("loop");/
        }   
    }
