//SLAVE 1 - LED
#include <HardwareSerial.h>
HardwareSerial SerialPort(2);
const int Enable =  2;
const int SlaveNumber = 1;
int Slave;
const int LED = 4;
void setup() 
{ 
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  SerialPort.setTimeout(250);
  pinMode(Enable, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(Enable, LOW);
  digitalWrite(LED, LOW);
} 
void loop() 
{
  digitalWrite(Enable, LOW); 
  if(SerialPort.available())
  {
      SerialPort.flush();
      Slave = SerialPort.parseInt();
      Serial.println(Slave);
      if(Slave == SlaveNumber)
      {   
        String command = SerialPort.readString();  
          Serial.println(command);
           if(command == "ON")
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
