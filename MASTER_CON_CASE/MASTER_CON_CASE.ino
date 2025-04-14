// MAESTRO RS-485 con MEF Unificada (estructura fiel al original y verificación detallada)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>
#include <Bounce2.h>

HardwareSerial SerialPort(2);

byte slave_1_id = 0b01;
byte slave_2_id = 0b10;
byte slave_3_id = 0b11;
byte CRC_CONSTANT = 0b011;

byte funcion_0 = 0b001;
byte funcion_1 = 0b010;
byte funcion_2 = 0b011;
byte funcion_3 = 0b100;
byte funcion_4 = 0b101;
byte funcion_5 = 0b110;

const int Enable = 2;
const int btnUp = 13, btnDown = 12, btnSelect = 14;
Bounce debouncerUp, debouncerDown, debouncerSelect;
const int debounceInterval = 50;

LiquidCrystal_I2C lcd(0x27, 16, 2);

enum EstadoSistema {
  MENU_PRINCIPAL,
  SUBMENU_ESCLAVO1,
  SUBMENU_ESCLAVO2,
  SUBMENU_ESCLAVO3,
  ESPERANDO_RESPUESTA
};
EstadoSistema estadoSistema = MENU_PRINCIPAL;

int menuOption = 0;
int submenuOption = 0;
const int maxOptions = 3;
const int maxSubmenuOptions = 3;

unsigned long tiempoEnvio = 0;
unsigned long timeoutRespuesta = 3000;

byte esclavoEsperado = 0;
byte funcionEsperada = 0;

void setup() {
  Serial.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);
  pinMode(btnUp, INPUT);
  pinMode(btnDown, INPUT);
  pinMode(btnSelect, INPUT);
  debouncerUp.attach(btnUp); debouncerUp.interval(debounceInterval);
  debouncerDown.attach(btnDown); debouncerDown.interval(debounceInterval);
  debouncerSelect.attach(btnSelect); debouncerSelect.interval(debounceInterval);
  lcd.init();
  lcd.backlight();
  mostrarMenu(menuOption);
}

void loop() {
  debouncerUp.update();
  debouncerDown.update();
  debouncerSelect.update();

  switch (estadoSistema) {
    case MENU_PRINCIPAL:
      if (debouncerUp.rose()) { menuOption = (menuOption + 1) % maxOptions; mostrarMenu(menuOption); }
      if (debouncerDown.rose()) { menuOption = (menuOption - 1 + maxOptions) % maxOptions; mostrarMenu(menuOption); }
      if (debouncerSelect.rose()) {
        submenuOption = 0;
        estadoSistema = (EstadoSistema)(SUBMENU_ESCLAVO1 + menuOption);
        mostrarSubmenu(menuOption, submenuOption);
      }
      break;

   case SUBMENU_ESCLAVO1:
      if (debouncerUp.rose()) {
        submenuOption = (submenuOption + 1) % maxSubmenuOptions;
        mostrarSubmenu(0, submenuOption);
      }
      if (debouncerDown.rose()) {
        submenuOption = (submenuOption - 1 + maxSubmenuOptions) % maxSubmenuOptions;
        mostrarSubmenu(0, submenuOption);
      }
      if (debouncerSelect.rose()) {
        if (submenuOption == 2) {
          estadoSistema = MENU_PRINCIPAL;
          mostrarMenu(menuOption);
        } else {
          ejecutarFuncion(0, submenuOption);  // Esclavo 1: LED o DHT11
        }
      }
      break;

    case SUBMENU_ESCLAVO2:
      if (debouncerUp.rose()) {
        submenuOption = (submenuOption + 1) % maxSubmenuOptions;
        mostrarSubmenu(1, submenuOption);
      }
      if (debouncerDown.rose()) {
        submenuOption = (submenuOption - 1 + maxSubmenuOptions) % maxSubmenuOptions;
        mostrarSubmenu(1, submenuOption);
      }
      if (debouncerSelect.rose()) {
        if (submenuOption == 2) {
          estadoSistema = MENU_PRINCIPAL;
          mostrarMenu(menuOption);
        } else {
          ejecutarFuncion(1, submenuOption);  // Esclavo 2: Buzzer o HC-SR04
        }
      }
      break;

    case SUBMENU_ESCLAVO3:
      if (debouncerUp.rose()) {
        submenuOption = (submenuOption + 1) % maxSubmenuOptions;
        mostrarSubmenu(2, submenuOption);
      }
      if (debouncerDown.rose()) {
        submenuOption = (submenuOption - 1 + maxSubmenuOptions) % maxSubmenuOptions;
        mostrarSubmenu(2, submenuOption);
      }
      if (debouncerSelect.rose()) {
        if (submenuOption == 2) {
          estadoSistema = MENU_PRINCIPAL;
          mostrarMenu(menuOption);
        } else {
          ejecutarFuncion(2, submenuOption);  // Esclavo 3: Servo o LED
        }
      }
      break;

    case ESPERANDO_RESPUESTA:
      verificarRespuesta();
      break;
  }
}

void mostrarMenu(int opcion) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Seleccionar:");
  lcd.setCursor(0, 1);
  if (opcion == 0) lcd.print("-> Esclavo 1");
  else if (opcion == 1) lcd.print("-> Esclavo 2");
  else lcd.print("-> Esclavo 3");
}

void mostrarSubmenu(int esclavo, int opcion) {
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Esclavo "); 
  lcd.print(esclavo + 1);
  lcd.setCursor(0, 1);

  if (esclavo == 0) { // Esclavo 1
    if (opcion == 0) lcd.print("-> LED");
    else if (opcion == 1) lcd.print("-> DHT11");
    else lcd.print("-> Atras");
  } 
  else if (esclavo == 1) { // Esclavo 2
    if (opcion == 0) lcd.print("-> Buzzer");
    else if (opcion == 1) lcd.print("-> HC-SR04");
    else lcd.print("-> Atras");
  } 
  else if (esclavo == 2) { // Esclavo 3
    if (opcion == 0) lcd.print("-> Servo");
    else if (opcion == 1) lcd.print("-> LED");
    else lcd.print("-> Atras");
  }
}

void ejecutarFuncion(int esclavo, int opcion) {
  byte id = (esclavo == 0) ? slave_1_id : (esclavo == 1) ? slave_2_id : slave_3_id;
  byte funcion = (esclavo == 0 && opcion == 0) ? funcion_0 :
                 (esclavo == 0 && opcion == 1) ? funcion_1 :
                 (esclavo == 1 && opcion == 0) ? funcion_2 :
                 (esclavo == 1 && opcion == 1) ? funcion_3 :
                 (esclavo == 2 && opcion == 0) ? funcion_4 : funcion_5;

  byte trama = (id << 6) | (funcion << 3) | (CRC_CONSTANT & 0b00000111);
  esclavoEsperado = id;
  funcionEsperada = funcion;

  while (SerialPort.available()) SerialPort.read();
  digitalWrite(Enable, HIGH);
  SerialPort.write(trama);
  SerialPort.flush();
  digitalWrite(Enable, LOW);

  estadoSistema = ESPERANDO_RESPUESTA;
  tiempoEnvio = millis();
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("Esperando...");
}

void verificarRespuesta() {
  if (millis() - tiempoEnvio > timeoutRespuesta) {
    Serial.println("Timeout de respuesta");
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Esclavo no");
    lcd.setCursor(0, 1); lcd.print("conectado");
    delay(2000);
    estadoSistema = MENU_PRINCIPAL;
    mostrarMenu(menuOption);
    return;
  }

  if (SerialPort.available()) {
    byte dato = SerialPort.read();
    if (dato == 0xFF) {
      while (SerialPort.available() < 1);
      byte id = SerialPort.read();
      while (SerialPort.available() < 1);
      byte funcion = SerialPort.read();

      if (id == slave_1_id) {
        if (funcion == funcion_0) {
          lcd.clear(); lcd.setCursor(0, 0); lcd.print("Esclavo 1 - LED");
          delay(2000);
        } else if (funcion == funcion_1) {
          while (SerialPort.available() < 2);
          byte humedad = SerialPort.read();
          byte temperatura = SerialPort.read();
          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("Temp: "); lcd.print(temperatura); lcd.print(" C");
          lcd.setCursor(0, 1); lcd.print("Hum: "); lcd.print(humedad); lcd.print(" %");
          delay(2000);
        }
      } else if (id == slave_2_id) {
        if (funcion == funcion_2) {
          lcd.clear(); lcd.setCursor(0, 0); lcd.print("Buzzer-Activo");
          delay(2000);
        } else if (funcion == funcion_3) {
          while (SerialPort.available() < 1);
          byte distancia = SerialPort.read();
          lcd.clear(); lcd.setCursor(0, 0);
          lcd.print("Distancia: "); lcd.print(distancia); lcd.print(" cm");
          delay(2000);
        }
      } else if (id == slave_3_id) {
        if (funcion == funcion_4) {
          lcd.clear(); lcd.setCursor(0, 0); lcd.print("Esclavo 3 - Func 1");
          delay(2000);
        } else if (funcion == funcion_5) {
          lcd.clear(); lcd.setCursor(0, 0); lcd.print("Esclavo 3 - Func 2");
          delay(2000);
        }
      }
      estadoSistema = MENU_PRINCIPAL;
      mostrarMenu(menuOption);
    }
  }
}