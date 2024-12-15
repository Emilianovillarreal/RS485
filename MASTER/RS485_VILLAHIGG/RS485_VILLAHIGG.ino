//master

#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);

// Pines de los botones
const int boton1Pin = 12;
const int boton2Pin = 13;
const int boton3Pin = 14;

// Variables para gestionar el debounce
unsigned long tiempoDebounce = 50; // Tiempo de debounce en milisegundos
unsigned long ultimaLectura1 = 0;
unsigned long ultimaLectura2 = 0;
unsigned long ultimaLectura3 = 0;

int estadoBoton1 = LOW; // Asumimos que los botones están normalmente en LOW
int estadoBoton2 = LOW;
int estadoBoton3 = LOW;

int estadoBoton1Anterior = LOW;
int estadoBoton2Anterior = LOW;
int estadoBoton3Anterior = LOW;

// IDs de los esclavos
byte slave_1_id = 0b01;
byte slave_2_id = 0b10;
byte slave_3_id = 0b11;

byte CRC_CONSTANT = 0b0011;

// Pin de habilitación para controlar transmisión y recepcion (RS-485)
const int Enable =  2;  

byte funcion_0 = 0b01;    // LED
byte funcion_1 = 0b10;    // SENSOR
byte funcion_2 = 0b11;    // MOTOR

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);
  
  // Configurar los pines de los botones como entrada (debe usarse resistencia pull-up externa o interna si INPUT_PULLUP)
  pinMode(boton1Pin, INPUT);
  pinMode(boton2Pin, INPUT);
  pinMode(boton3Pin, INPUT);
}

void loop() {
  // Lectura del estado de los botones con debounce
  int lecturaActual1 = digitalRead(boton1Pin);
  int lecturaActual2 = digitalRead(boton2Pin);
  int lecturaActual3 = digitalRead(boton3Pin);

  // Actualizar el tiempo de la última lectura si hay un cambio en el estado del botón
  if (lecturaActual1 != estadoBoton1Anterior) {
    ultimaLectura1 = millis();
  }
  if (lecturaActual2 != estadoBoton2Anterior) {
    ultimaLectura2 = millis();
  }
  if (lecturaActual3 != estadoBoton3Anterior) {
    ultimaLectura3 = millis();
  }

  if ((millis() - ultimaLectura1) > tiempoDebounce) {
    if (lecturaActual1 != estadoBoton1) {
      estadoBoton1 = lecturaActual1;
      if (estadoBoton1 == HIGH) { // Detectar cuando se presiona el botón
        ejecutarFuncion(1); // Función para el botón 1
      }
    }
  }

  if ((millis() - ultimaLectura2) > tiempoDebounce) {
    if (lecturaActual2 != estadoBoton2) {
      estadoBoton2 = lecturaActual2;
      if (estadoBoton2 == HIGH) {
        ejecutarFuncion(2); // Función para el botón 2
      }
    }
  }

  if ((millis() - ultimaLectura3) > tiempoDebounce) {
    if (lecturaActual3 != estadoBoton3) {
      estadoBoton3 = lecturaActual3;
      if (estadoBoton3 == HIGH) {
        ejecutarFuncion(3); // Función para el botón 3
      }
    }
  }

  estadoBoton1Anterior = lecturaActual1;
  estadoBoton2Anterior = lecturaActual2;
  estadoBoton3Anterior = lecturaActual3;
}

// Función que ejecuta las acciones dependiendo del botón
void ejecutarFuncion(int boton) {
  byte trama;
  switch (boton) {
    case 1:
      Serial.println("--------------FUNCION DEL BOTON 1--------------");
      trama = enviarComando(slave_1_id,funcion_0,CRC_CONSTANT);
      Serial.println("Impresion de la trama que se envia al slave 1:");
      Serial.println(trama,BIN);

      digitalWrite(Enable, HIGH);

      SerialPort.write(trama);
      SerialPort.flush();

      digitalWrite(Enable, LOW);

      break;
    case 2:
      Serial.println("--------------FUNCION DEL BOTON 2--------------");
      trama = enviarComando(slave_2_id,funcion_1,CRC_CONSTANT);
      Serial.println("Impresion de la trama que se envia al slave 2:");
      Serial.println(trama,BIN);

      //digitalWrite(Enable, LOW);
      // while (SerialPort.available()) {
      //   SerialPort.read(); // Limpiar datos residuales
      // } 

      digitalWrite(Enable, HIGH);

      SerialPort.write(trama);
      SerialPort.flush();
      
      digitalWrite(Enable, LOW);

      recibirDatos();

      break;
    case 3:
      Serial.println("--------------FUNCION DEL BOTON 3--------------");
      trama = enviarComando(slave_3_id,funcion_2,CRC_CONSTANT);
      Serial.println("Impresion de la trama que se envia al slave 3:");
      Serial.println(trama,BIN);

      digitalWrite(Enable, HIGH);

      SerialPort.write(trama);
      SerialPort.flush();

      digitalWrite(Enable, LOW);

      break;
  }
}

// Función para crear una trama de comando a enviar al slave
// Parámetros:
// id: ID del dispositivo esclavo
// funcion: Código de la función a ejecutar (LED, SENSOR, MOTOR)
// CRC: Código de verificación (4 bits constantes)
// Retorna: Trama completa para ser enviada
byte enviarComando(byte id, byte funcion, byte CRC) {
  return (id << 6) | (funcion << 4) | (CRC & 0b00001111);
}


void recibirDatos(){
  uint8_t buffer[2];

  while (SerialPort.available()) {
  byte header = SerialPort.read();
  if (header == 0xFF) {
    SerialPort.read(buffer, sizeof(buffer)); // Leer datos del buffer (Humedad y Temperatura)
    break;
  }
}

  Serial.println("Buffer[0]: ");
  Serial.println(buffer[0],BIN);
  Serial.println("Buffer[1]: ");
  Serial.println(buffer[1],BIN);

  int humedad = buffer[0];
  int temperatura = buffer[1];

  // Mostrar los valores en el monitor serial
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.print("%, Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");

}

