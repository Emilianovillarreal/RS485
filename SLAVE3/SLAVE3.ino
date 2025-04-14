#include <ESP32Servo.h>
#include <HardwareSerial.h>

//======================== DEFINICIONES ========================//
enum EstadoMEF {
  ESPERANDO_TRAMA,
  IDENTIFICANDO_ID,
  DECODIFICANDO_FUNCION,
  EJECUTANDO_FUNCION,
  ENVIANDO_RESPUESTA
};

HardwareSerial SerialPort(2);

// ID del esclavo
const byte slave_3_id = 0b11;

// Funciones
const byte funcion_4 = 0b101;    // Movimiento del servo
const byte funcion_5 = 0b110;    // Encender LED

// Pines
#define PIN_SG90 18
const int LED = 15; 
const int Enable = 2;

// Objeto Servo
Servo sg90;

// Variables globales para la MEF
EstadoMEF estado = ESPERANDO_TRAMA;
byte tramaRecibida;  // Variable que almacena la trama completa recibida
byte idRecibido, funcionRecibida, crcRecibido;

//======================== SETUP ========================//
void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 

  // Configuración del servo
  sg90.setPeriodHertz(50);             // Frecuencia PWM para el SG90
  sg90.attach(PIN_SG90, 500, 2400);      // Pulso de 0° a 180°

  // Pines
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW);
}

//======================== LOOP ========================//
void loop() {
  switch (estado) {
    case ESPERANDO_TRAMA:
      // Espera a que haya datos disponibles en el SerialPort
      if (SerialPort.available()) {
        tramaRecibida = SerialPort.read();
        Serial.println("Trama recibida.");
        estado = IDENTIFICANDO_ID;
      }
      break;

    case IDENTIFICANDO_ID:
      // Extraer y mostrar el ID de la trama
      idRecibido = (tramaRecibida >> 6) & 0b00000011;  // Bits 7 y 6
      Serial.print("ID: ");
      Serial.println(idRecibido, BIN);
      if (idRecibido == slave_3_id) {
        Serial.println("ID coincide. Continuando...");
        estado = DECODIFICANDO_FUNCION;
      } else {
        Serial.println("ID no coincide. Reiniciando MEF.");
        estado = ESPERANDO_TRAMA;
      }
      break;

    case DECODIFICANDO_FUNCION:
      // Decodificación de función y CRC
      funcionRecibida = (tramaRecibida >> 3) & 0b00000111;  // Bits 5 a 3
      crcRecibido = tramaRecibida & 0b00000111;             // Bits 2 a 0
      Serial.print("Función: ");
      Serial.println(funcionRecibida, BIN);
      Serial.print("CRC: ");
      Serial.println(crcRecibido, BIN);
      estado = EJECUTANDO_FUNCION;
      break;

    case EJECUTANDO_FUNCION:
      // Ejecución de la función requerida
      if (funcionRecibida == funcion_4) {
        Serial.println("Ejecutando función 4: Mover Servo");
        // Movimiento del servo de 0° a 180° y vuelta
        for (int pos = 0; pos <= 180; pos++) {
          sg90.write(pos);
          delay(10);
        }
        for (int pos = 180; pos >= 0; pos--) {
          sg90.write(pos);
          delay(10);
        }
      } else if (funcionRecibida == funcion_5) {
        Serial.println("Ejecutando función 5: Encender LED");
        digitalWrite(LED, HIGH);
        delay(2000);
        digitalWrite(LED, LOW);
      }
      estado = ENVIANDO_RESPUESTA;
      break;

    case ENVIANDO_RESPUESTA: {
      // Armado y envío de la trama de respuesta
      byte tramaRespuesta[3] = {0xFF, slave_3_id, funcionRecibida};
      enviarTrama(tramaRespuesta, 3);
      // Reinicia la MEF para esperar la siguiente trama
      estado = ESPERANDO_TRAMA;
      break;
    }
      
    default:
      estado = ESPERANDO_TRAMA;
      break;
  }
}

//======================== FUNCIONES AUXILIARES ========================//
void enviarTrama(byte* trama, int longitud) {
  // Debug de trama a enviar
  for (int i = 0; i < longitud; i++) {
    Serial.print("Trama[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(trama[i], BIN);
  }

  Serial.println("Enviando trama");
  digitalWrite(Enable, HIGH);
  SerialPort.write(trama, longitud);
  SerialPort.flush();
  digitalWrite(Enable, LOW);
  Serial.println("Trama enviada");
}
