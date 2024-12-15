//SLAVE 1 - LED
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);

// ID del esclavo
const byte slave_1_id = 0b01;

// Pin de habilitación y LED
const int Enable =  2;
const int LED = 4;

void setup() 
{ 
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW);
} 

void loop() {
    if (SerialPort.available()) {
        byte trama = SerialPort.read();

        byte id = (trama >> 6) & 0b00000011;         // Extraer los 2 bits más significativos
        byte funcion = (trama >> 4) & 0b00000011;    // Extraer los siguientes 2 bits
        byte crc = trama & 0b00001111;               // Extraer los 4 bits menos significativos

        Serial.print("ID: ");
        Serial.println(id, BIN);
        Serial.print("Función: ");
        Serial.println(funcion, BIN);
        Serial.print("CRC: ");
        Serial.println(crc, BIN);

        // Implementa la acción deseada en función del ID y la función
        if (id == 1) {
          Serial.println("id------");
            if (funcion == 2) {  // Función de encender LED
                Serial.println("funcion------");
                digitalWrite(LED, HIGH);
                delay(1000);
                digitalWrite(LED, LOW);
                // enviarRespuesta("Slave 1 is triggered");
            }
        }
    }
}
