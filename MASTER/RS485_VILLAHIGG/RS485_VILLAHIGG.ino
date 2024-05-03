#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);
//************************************************************

const byte slave_1_id = 0b00;
const byte slave_2_id = 0b01;
const byte slave_3_id = 0b10;
const byte slave_4_id = 0b11;
//************************************************************

const int BUTTON_PIN_01 = 4;
const int BUTTON_PIN_02 = 12;
const int BUTTON_PIN_03 = 14;
const int BUTTON_PIN_04 = 27;

const int Enable =  2;

const byte funcion_0 = 0b01;
const byte funcion_1 = 0b10;
const byte funcion_2 = 0b11;


int opcion = 0;
//************************************************************
void Select_Opcion(){
  if(BUTTON_PIN_01==HIGH){
    opcion = 1;
  }else if(BUTTON_PIN_02==HIGH){
    opcion = 2;
  }else if(BUTTON_PIN_03==HIGH){
    opcion = 3;
  }else if(BUTTON_PIN_04==HIGH){
    opcion = 0;
  }
}
//************************************************************

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 

  //SerialPort.setTimeout(250);
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(BUTTON_PIN_01, INPUT);
  pinMode(BUTTON_PIN_02, INPUT);
  pinMode(BUTTON_PIN_03, INPUT);
  pinMode(BUTTON_PIN_04, INPUT);

}
//************************************************************

void loop() {
  Select_Opcion();
  switch(opcion){
    
    case 0:   //Ejecucion en bucle
      //LED
      digitalWrite(Enable, HIGH);

      SerialPort.print(slave_1_id);
      SerialPort.print(funcion_0);
      SerialPort.flush();

      digitalWrite(Enable, LOW);

      //SENSOR
      digitalWrite(Enable, HIGH);

      SerialPort.print(slave_2_id);
      SerialPort.print(funcion_1);
      SerialPort.flush();

      digitalWrite(Enable, LOW); 
      if(SerialPort.available()){
        Serial.println(SerialPort.readString());
      }

      //MOTOR
      //CODIGO DEL MOTOR
      
    break;

    case 1:   //Prender LED
      digitalWrite(Enable, HIGH);

      SerialPort.print(slave_1_id);
      SerialPort.print(funcion_0);
      SerialPort.flush();

      opcion = 0;
      //digitalWrite(Enable, LOW);
      //delay(2000);
    
    case 2:   //SENSAR HUMEDAD
      digitalWrite(Enable, HIGH);

      SerialPort.print(slave_2_id);
      SerialPort.print(funcion_1);
      SerialPort.flush();

      digitalWrite(Enable, LOW);

      if(SerialPort.available()){
        Serial.println(SerialPort.readString());
      }

      opcion = 0;
    break;

    case 3:   //MOTOR
      //Codigo del motor

      opcion = 0;
    break;
  }

}
