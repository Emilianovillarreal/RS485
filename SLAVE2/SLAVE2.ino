//SLAVE 2 - SENSOR
#include <HardwareSerial.h>
#include "DHT.h"

HardwareSerial SerialPort(2);
//************************************************************

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float h;
float t;
//************************************************************

const byte slave_2_id = 0b10;
//************************************************************

const int Enable =  2;
int Slave;

void setup() 
{ 
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  SerialPort.setTimeout(250);   //ver para que esta

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  dht.begin();
} 
void loop() 
{
  digitalWrite(Enable, LOW); 

  if(SerialPort.available()){
    byte trama = SerialPort.read();
    Serial.println((trama>>6)&0b11);

    if(slave_2_id == (trama>>6)&0b11){   
      byte command =(trama>>4)&0b11;  
      Serial.println(command);

      if(command == 0b10){
        h=dht.readHumidity();
        t=dht.readTemperature();

        digitalWrite(Enable, HIGH);

        if (isnan(h) || isnan(t)) {
          SerialPort.println("Failed to read from DHT sensor ");
        } else {
          // Envía los valores de humedad y temperatura al maestro
          SerialPort.print("Humidity: ");
          SerialPort.print(h);
          
          SerialPort.print("%  Temperature: ");
          SerialPort.print(t);
          SerialPort.println("°C");
          }
      }     
    }
          
  }   
}