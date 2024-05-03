//SLAVE 2 - SENSOR
#include <HardwareSerial.h>
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float h;
float t;

HardwareSerial SerialPort(2);

const int Enable =  2;
const int SlaveNumber = 2;
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
    Slave = SerialPort.parseInt();
    Serial.println(Slave);

    if(Slave == SlaveNumber){   
      String command = SerialPort.readString();  
      Serial.println(command);

      if(command == "ON"){
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