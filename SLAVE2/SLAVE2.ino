#include <HardwareSerial.h>
#include "DHT.h"
#include <stdint.h>
//#include <LiquidCrystal.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
HardwareSerial SerialPort(2);

float humedad;
float temperatura;

const byte slave_2_id = 0b10;
const byte funcion_DHT = 0b10;

const int Enable = 2;

// Configuración del display LCD (pines RS, E, D4, D5, D6, D7)
//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

  dht.begin();

  //lcd.begin(16, 2);
  //lcd.print(F("Temp y Humedad")); 
  delay(2000);

  Serial.println(F("Entro SETUP-----"));
}

void loop() {
 
  
  if (SerialPort.available() > 1) {
    yield(); // Resetear el watchdog
    digitalWrite(Enable, LOW);
    Serial.println(F("Mensaje disponible"));

    byte trama = SerialPort.read();

    byte id = (trama >> 6) & 0b00000011;         // Extraer los 2 bits más significativos
    byte funcion = (trama >> 4) & 0b00000011;    // Extraer los siguientes 2 bits
    byte crc = trama & 0b00001111;               // Extraer los 4 bits menos significativos

    Serial.print(F("ID: "));
    Serial.println(id, BIN);
    Serial.print(F("Función: "));
    Serial.println(funcion, BIN);
    Serial.print(F("CRC: "));
    Serial.println(crc, BIN);

    if (id == slave_2_id && funcion == funcion_DHT) {
      Serial.println(F("Activando el sensor..."));

      humedad = dht.readHumidity();
      temperatura = dht.readTemperature();
      Serial.print("Humedad: ");
      Serial.println(humedad);

      Serial.print("Temperatura: ");
      Serial.println(temperatura);



      if (isnan(humedad) || isnan(temperatura)) {
        Serial.println(F("Error leyendo el sensor"));
//        lcd.clear();
//        lcd.print(F("Error en sensor"));
        //delay(2000);
        return;
      }

      // Mostrar en el LCD
//      lcd.clear();
//      lcd.setCursor(0, 0);
//      lcd.print(F("Temp: "));
//      lcd.print(temperatura);
//      lcd.print(F(" C"));

  //    lcd.setCursor(0, 1);
  //    lcd.print(F("Hum: "));
  //    lcd.print(humedad);
  ///    lcd.print(F(" %"));

     // delay(2000);

      // Enviar la trama
      byte trama[3];
      trama[0] = 0xFF; // Delimitador de inicio
      trama[1] = (byte)humedad;
      trama[2] = (byte)temperatura;
      SerialPort.write(trama, sizeof(trama));


      Serial.print(F("Trama[0]: "));
      Serial.println(trama[0], BIN);
      Serial.print(F("Trama[1]: "));
      Serial.println(trama[1], BIN);

      digitalWrite(Enable, HIGH);
      SerialPort.write(trama, sizeof(trama));
      SerialPort.flush();
      digitalWrite(Enable, LOW);
      delay(50);
    }
  }
}