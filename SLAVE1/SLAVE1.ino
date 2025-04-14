// SLAVE 1 - LED y sensor DHT11 con MEF
#include <HardwareSerial.h>
#include "DHT.h"
#include <stdint.h>

#define DHTPIN 27
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
HardwareSerial SerialPort(2);

float humedad;
float temperatura;

const byte slave_1_id = 0b01;
const byte funcion_0 = 0b001; // LED
const byte funcion_1 = 0b010; // DHT11

const int Enable = 2;
const int LED = 14;

// Estados de la MEF
enum EstadoMEF {
  ESPERANDO_TRAMA,
  IDENTIFICANDO_ID,
  EJECUTANDO_FUNCION,
  ENVIANDO_RESPUESTA
};

EstadoMEF estadoActual = ESPERANDO_TRAMA;
byte tramaRecibida = 0;
byte funcion = 0;

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  dht.begin();
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO_TRAMA:
      if (SerialPort.available()) {
        tramaRecibida = SerialPort.read();
        estadoActual = IDENTIFICANDO_ID;
      }
      break;

    case IDENTIFICANDO_ID: {
      byte id = (tramaRecibida >> 6) & 0b00000011;
      funcion = (tramaRecibida >> 3) & 0b00000111;
      byte crc = tramaRecibida & 0b00000111;

      Serial.print("ID: "); Serial.println(id, BIN);
      Serial.print("Función: "); Serial.println(funcion, BIN);
      Serial.print("CRC: "); Serial.println(crc, BIN);

      if (id == slave_1_id) {
        estadoActual = EJECUTANDO_FUNCION;
      } else {
        estadoActual = ESPERANDO_TRAMA; // Ignora tramas de otros IDs
      }
      break;
    }

    case EJECUTANDO_FUNCION:
      if (funcion == funcion_0) {
        Serial.println("Ejecutando función: Encender LED");
        digitalWrite(LED, HIGH);
        delay(2000);
        digitalWrite(LED, LOW);
        estadoActual = ENVIANDO_RESPUESTA;
      } else if (funcion == funcion_1) {
        Serial.println("Ejecutando función: Leer sensor DHT11");
        humedad = dht.readHumidity();
        temperatura = dht.readTemperature();

        if (isnan(humedad) || isnan(temperatura)) {
          Serial.println(F("Error leyendo el sensor"));
          estadoActual = ESPERANDO_TRAMA;
        } else {
          estadoActual = ENVIANDO_RESPUESTA;
        }
      } else {
        Serial.println("Función no reconocida");
        estadoActual = ESPERANDO_TRAMA;
      }
      break;

    case ENVIANDO_RESPUESTA:
      if (funcion == funcion_0) {
        byte trama[3] = {0xFF, slave_1_id, funcion_0};
        enviarTrama(trama, 3);
      } else if (funcion == funcion_1) {
        byte trama[5] = {0xFF, slave_1_id, funcion_1, (byte)humedad, (byte)temperatura};
        enviarTrama(trama, 5);
      }
      estadoActual = ESPERANDO_TRAMA;
      break;
  }
}

void enviarTrama(byte* trama, int longitud) {
  Serial.println("Enviando trama");
  for (int i = 0; i < longitud; i++) {
    Serial.print("Trama["); Serial.print(i); Serial.print("]: ");
    Serial.println(trama[i], BIN);
  }
  digitalWrite(Enable, HIGH);
  SerialPort.write(trama, longitud);
  SerialPort.flush();
  digitalWrite(Enable, LOW);
  Serial.println("Trama enviada");
}

































































// //SLAVE 1 - LED y sensor DHT11
// #include <HardwareSerial.h>
// #include "DHT.h"
// #include <stdint.h>

// #define DHTPIN 27
// #define DHTTYPE DHT11

// DHT dht(DHTPIN, DHTTYPE);
// HardwareSerial SerialPort(2);

// float humedad;
// float temperatura;

// // ID del esclavo
// const byte slave_1_id = 0b01;

// // Funciones
// const byte funcion_0 = 0b001; // Encender LED
// const byte funcion_1 = 0b010; // Leer DHT11

// // Pin de habilitación y LED
// const int Enable = 2;
// const int LED = 14;

// void setup() {
//   Serial.begin(9600);
//   SerialPort.begin(9600, SERIAL_8N1, 16, 17);

//   pinMode(Enable, OUTPUT);
//   digitalWrite(Enable, LOW);

//   pinMode(LED, OUTPUT);
//   digitalWrite(LED, LOW);
//   dht.begin();
// }

// void loop() {
//   if (SerialPort.available()) {
//     byte trama = SerialPort.read();

//     byte id = (trama >> 6) & 0b00000011;
//     byte funcion = (trama >> 3) & 0b00000111;
//     byte crc = trama & 0b00000111;

//     Serial.print("ID: ");
//     Serial.println(id, BIN);
//     Serial.print("Función: ");
//     Serial.println(funcion, BIN);
//     Serial.print("CRC: ");
//     Serial.println(crc, BIN);

//     if (id == slave_1_id) {
//       Serial.println("ID válido recibido");

//       switch (funcion) {
//         case funcion_0:
//           Serial.println("Ejecutando función: Encender LED");

//           digitalWrite(LED, HIGH);
//           delay(2000);
//           digitalWrite(LED, LOW);

//           {
//             byte trama[3] = {0xFF, slave_1_id, funcion_0};

//             Serial.print(F("Trama[0]: "));
//             Serial.println(trama[0], BIN);
//             Serial.print(F("Trama[1]: "));
//             Serial.println(trama[1], BIN);
//             Serial.print(F("Trama[2]: "));
//             Serial.println(trama[2], BIN);

//             Serial.println("Enviando trama");

//             digitalWrite(Enable, HIGH);
//             SerialPort.write(trama, sizeof(trama));
//             SerialPort.flush();
//             digitalWrite(Enable, LOW);

//             Serial.println("Trama enviada");
//           }
//           break;

//         case funcion_1:
//           Serial.println(F("Ejecutando función: Leer sensor DHT11"));

//           humedad = dht.readHumidity();
//           temperatura = dht.readTemperature();

//           if (isnan(humedad) || isnan(temperatura)) {
//             Serial.println(F("Error leyendo el sensor"));
//             return;
//           }

//           Serial.print("Humedad: ");
//           Serial.println(humedad);
//           Serial.print("Temperatura: ");
//           Serial.println(temperatura);

//           {
//             byte trama[5] = {0xFF, slave_1_id, funcion_1, (byte)humedad, (byte)temperatura};

//             Serial.print(F("Trama[0]: "));
//             Serial.println(trama[0], BIN);
//             Serial.print(F("Trama[1]: "));
//             Serial.println(trama[1], BIN);
//             Serial.print(F("Trama[2]: "));
//             Serial.println(trama[2], BIN);
//             Serial.print(F("Trama[3]: "));
//             Serial.println(trama[3], BIN);
//             Serial.print(F("Trama[4]: "));
//             Serial.println(trama[4], BIN);

//             Serial.println("Enviando trama");

//             digitalWrite(Enable, HIGH);
//             SerialPort.write(trama, sizeof(trama));
//             SerialPort.flush();
//             digitalWrite(Enable, LOW);

//             Serial.println("Trama enviada");
//           }
//           break;

//         default:
//           Serial.println("Función no reconocida");
//           break;
//       }
//     }
//   }
// }
