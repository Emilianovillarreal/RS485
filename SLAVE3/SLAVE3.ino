//SLAVE MOTOR
#include <ESP32Servo.h>
#include <HardwareSerial.h>

HardwareSerial SerialPort(2);
//********************

#define PIN_SG90 18 // Output pin used

//********************
const byte slave_3_id = 0b11;
const byte funcion_motor = 0b11;
Servo sg90;
//********************

int Slave;
const int Enable =  2;


void setup() {
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
  sg90.setPeriodHertz(50); // PWM frequency for SG90
  sg90.attach(PIN_SG90, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180

  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, LOW);

}

void loop() 
{
  digitalWrite(Enable, LOW); 
  if(SerialPort.available())
  {
    byte trama = SerialPort.read();
    byte id = (trama >> 6) & 0b00000011;
    byte funcion = (trama >> 4) & 0b00000011;
    byte crc = trama & 0b00000011;

    Serial.print("ID: ");
    Serial.println(id, BIN);
    Serial.print("Función: ");
    Serial.println(funcion, BIN);
    Serial.print("CRC: ");
    Serial.println(crc, BIN);

    
    if(id == slave_3_id){   
      if(funcion == funcion_motor)
      {
              //rotation from 0 to 180°
        for (int pos = 0; pos <= 180; pos += 1) {
          sg90.write(pos);
          delay(10);
        }
      // Rotation from 180° to 0
        for (int pos = 180; pos >= 0; pos -= 1) {
          sg90.write(pos);
          delay(10);
        }
          
      }
 
    }
          
  }   
}




// *********************************************************************************************************************************************************************************************************************************************************************************************************************************



//SLAVE MOTOR - Version vieja.

// #include <HardwareSerial.h>

// HardwareSerial SerialPort(2);
// //************************************************************

// #define x_paso 15   // Define el Pin de STEP para Motor de eje X
// #define x_dire 2    // Define el Pin de DIR  para Motor de eje X
// #define x_habi 4    // Define el Pin de ENABLE  para Motor de eje X

// int retardo = 3000;   // Menor numero el giro es mas rapido
// int tiempo = 2000;   // durante cuanto timpo da el giro el motor (vueltas)
// //************************************************************
// const byte slave_3_id = 0b11;
// //************************************************************

// int Slave;
// const int Enable =  2;


// void setup() {
//   Serial.begin(115200);
//   SerialPort.begin(9600, SERIAL_8N1, 16, 17); 
//   SerialPort.setTimeout(250);

//   pinMode(Enable, OUTPUT);
//   digitalWrite(Enable, LOW);

//   pinMode(x_paso, OUTPUT); pinMode(x_dire, OUTPUT); pinMode(x_habi, OUTPUT); 


// }

// void loop() 
// {
//   digitalWrite(Enable, LOW); 
//   if(SerialPort.available())
//   {
//     SerialPort.flush();
//     byte trama = SerialPort.read();
//     Serial.println((trama>>6)&0b11);
//     if(slave_3_id == ((trama>>6)&0b11))
//     {   
//       byte command =(trama>>4)&0b11;  
//       Serial.println(command);
//       if(command == 0b11)
//       {
//         giro(x_paso,x_dire,x_habi,1);
//         delay(500);
//         giro(x_paso,x_dire,x_habi,0);
//         delay(500);
          
//       }
//       digitalWrite(Enable, HIGH);
//       SerialPort.print("Slave 1 is triggered"); //
//       SerialPort.flush();
//     }
//            //Serial.println("loop");/
//   }   
// }
  



// void giro(int paso_,int dire_,int habi_,int dir) {
//    digitalWrite(habi_, LOW);  // Habilita el Driver
//   if( dir==0){ 
//    digitalWrite(dire_, LOW);   // direccion de giro 0
//    for(int i=0;i<tiempo;i++){  // da  pasos por un tiempo  
//     digitalWrite(paso_, HIGH);      
//     delayMicroseconds(retardo);          
//     digitalWrite(paso_, LOW);       
//     delayMicroseconds(retardo); 
//    }
//   }
//   if( dir==1){ 
//   digitalWrite(dire_, HIGH);   // direccion de giro 1
//   for(int i=0;i<tiempo;i++){   // da  pasos por un tiempo  
//     digitalWrite(paso_, HIGH);      
//     delayMicroseconds(retardo);          
//     digitalWrite(paso_, LOW);       
//     delayMicroseconds(retardo);  
//    }
//   }
//   digitalWrite(habi_, HIGH);   // quita la habilitacion del Driver

// }
