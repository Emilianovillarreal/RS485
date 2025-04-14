// SLAVE 2 - SENSOR HC-SR04 Y Buzzer con MEF
#include <HardwareSerial.h>
#include <stdint.h>
#include <Arduino.h>
#include <string.h>

// ID del esclavo
const byte slave_2_id = 0b10;

// Funciones 
const byte funcion_2 = 0b011; // Buzzer
const byte funcion_3 = 0b100; // Sensor HC-SR04

// Pin de habilitación 
const int Enable =  2;
HardwareSerial SerialPort(2);

// Buzzer
const int buzzerPin = 27;
const int frecuenciaSol = 392;
const int duracionSol = 500;

// Sensor HC-SR04
int trigPin = 33;
int echoPin = 12;
long duration, cm;

// Estados de la MEF
enum Estado { ESPERANDO_TRAMA, IDENTIFICANDO_ID, DECODIFICANDO_FUNCION, EJECUTANDO_FUNCION, ENVIANDO_RESPUESTA };
Estado estadoActual = ESPERANDO_TRAMA;

// Variables para decodificación
byte tramaRecibida;
byte id;
byte funcion;
byte crc;

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("ID");
  Serial.println(slave_2_id);  
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO_TRAMA:
      if (SerialPort.available()) {
        tramaRecibida = SerialPort.read();
        estadoActual = IDENTIFICANDO_ID;
      }
      break;

    case IDENTIFICANDO_ID:
      id = (tramaRecibida >> 6) & 0b00000011;
      funcion = (tramaRecibida >> 3) & 0b00000111;
      crc = tramaRecibida & 0b00000111;
      Serial.print("ID: "); Serial.println(id, BIN);
      Serial.print("Función: "); Serial.println(funcion, BIN);
      Serial.print("CRC: "); Serial.println(crc, BIN);
      if (id == slave_2_id) {
        estadoActual = DECODIFICANDO_FUNCION;
      } else {
        estadoActual = ESPERANDO_TRAMA;
      }
      break;

    case DECODIFICANDO_FUNCION:
      if (funcion == funcion_2 || funcion == funcion_3) {
        estadoActual = EJECUTANDO_FUNCION;
      } else {
        Serial.println("Función no válida");
        estadoActual = ESPERANDO_TRAMA;
      }
      break;

    case EJECUTANDO_FUNCION:
      if (funcion == funcion_2) {
        Serial.println("Ejecutando Buzzer");
        tone(buzzerPin, frecuenciaSol, duracionSol);
        delay(duracionSol + 100);
        noTone(buzzerPin);
        delay(2000);
      } else if (funcion == funcion_3) {
        Serial.println("Ejecutando HC-SR04");
        digitalWrite(trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        cm = (duration / 2) / 29.1;
        Serial.print(cm); Serial.println("cm");
      }
      estadoActual = ENVIANDO_RESPUESTA;
      break;

    case ENVIANDO_RESPUESTA:
      if (funcion == funcion_2) {
        byte trama[3] = {0xFF, slave_2_id, funcion_2};
        enviarTrama(trama, 3);
      } else if (funcion == funcion_3) {
        byte trama[4] = {0xFF, slave_2_id, funcion_3, (byte)cm};
        enviarTrama(trama, 4);
      }
      estadoActual = ESPERANDO_TRAMA;
      break;
  }
}

void enviarTrama(byte* trama, int longitud) {
  Serial.println("Enviando trama");
  for (int i = 0; i < longitud; i++) {
    Serial.print("Trama["); Serial.print(i); Serial.print("]: "); Serial.println(trama[i], BIN);
  }
  digitalWrite(Enable, HIGH);
  SerialPort.write(trama, longitud);
  SerialPort.flush();
  digitalWrite(Enable, LOW);
  Serial.println("Trama enviada");
}