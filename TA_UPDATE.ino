/* CATATAN 

   -NEXTION : kabel biru pin 2, kabel kuning pin 3
   -Komunikasi serial nextion --> arduino
      #include <SoftwareSerial.h>

      SoftwareSerial lcd(2,3);

      void setup(){
        Serial.begin(9600);
        lcd.begin(9600);
      }

      void loop(){
        if(lcd.available()){
          String y = lcd.readString();
          Serial.println(y);
        }
      }

*/

#include <PID_v1.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>

#define DHTPIN 4
#define DHTTYPE DHT22 
#define IN1 7
#define IN2 8  
#define IN3 9 
#define IN4 10  
#define ENA 6 
#define ENB 11  
 

SoftwareSerial lcd(2,3);
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

double Setpoint; 
double Input; 
double Output ; 
double Kp=0, Ki=5, Kd=0; 
unsigned long val = 0;
 
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

const int arraySize = 3;
const int Relay_Kipas = 12;
const int Relay_Peltier = 13;
const int Relay_Mistmaker = 5;
int lock = 0; 
int kelembaban;
int dats[arraySize];     
int count = 0; 
int KelembabanSementara = 70;
int SuhuSementara = 26;
int LuxSementara = 30;

float temperatur;

uint16_t Address = 0;
uint16_t lux;

char data[] = "ANGGREK";
char data2[] = "ANTHURIUM";

unsigned long interval22 = 0;
unsigned long interval2 = 0;
unsigned long interval = 0;
String LCDREAD;

struct ModeTanamanHias {

  int kelembabans;
  int suhus;
  uint16_t cahayas;

};

ModeTanamanHias modeTerpilih;

void running(){

  if(millis() - interval > 3000){
      TampilkanDHT(kelembaban, temperatur);
      TampilkanLux(lux);
      TampilkanSetpoint();
      DisplayMode();
      interval = millis();
  }

}

void TampilkanSetpoint(){

       lcd.print("t4.txt=\"");
  lcd.print(modeTerpilih.suhus,1);
  lcd.print("`C");
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

    lcd.print("t6.txt=\"");
  lcd.print(modeTerpilih.kelembabans);
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

    lcd.print("t8.txt=\"");
  lcd.print(modeTerpilih.cahayas);
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

}

void DisplayMode(){

    lcd.print("t00.txt=\"");

      char Mode1[100];
      for(int index = 0; index < sizeof(Mode1); index++){
        Mode1[index] = EEPROM.read(Address + index);
        if(Mode1[index] == '\0'){
          break;
        }
      }

    lcd.print(Mode1);
    lcd.print("\"");
    lcd.write(0xff);
    lcd.write(0xff);
    lcd.write(0xff);

}

void TampilkanDHT(int kelembaban,  float suhu){

     lcd.print("t13.txt=\"");
  lcd.print(suhu,1);
  lcd.print("`C");
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

    lcd.print("t15.txt=\"");
  lcd.print(kelembaban);
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

}

void TampilkanLux(int lux){

  lcd.print("t17.txt=\"");
  lcd.print(lux);
  lcd.print("\"");
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);

}

void Subsistem_Kelembaban(ModeTanamanHias &Value){

  int setpoint_kelembaban = Value.kelembabans;
  
  if(kelembaban > setpoint_kelembaban){
    
    digitalWrite(Relay_Kipas, LOW);

  }else if(kelembaban <= setpoint_kelembaban){

    digitalWrite(Relay_Kipas, HIGH);

  }

  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);

  if(kelembaban > setpoint_kelembaban){

    digitalWrite(Relay_Mistmaker, HIGH);
    digitalWrite(ENB, 0);

  }else if(kelembaban < setpoint_kelembaban){

    digitalWrite(Relay_Mistmaker, LOW);
    digitalWrite(ENB, 255);

  }
 
}

void Subsistem_Subu(ModeTanamanHias &Nilai){

  int setpoint_suhu = Nilai.suhus;

  if(temperatur > setpoint_suhu){
    
    digitalWrite(Relay_Peltier, LOW);

  }else if(temperatur <= setpoint_suhu){

    digitalWrite(Relay_Peltier, HIGH);

  }

}

void Subsisten_Cahaya(ModeTanamanHias &Nilai2){

  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  Setpoint = Nilai2.cahayas; 
  Input = map(lux, 0, 1024, 0, 255);
  myPID.Compute();
  analogWrite(ENA,Output); 
  // Serial.print(Input);
  // Serial.print(" ");
  // Serial.println(Output);
  // Serial.print(" ");  
  // Serial.println(Setpoint);
  delay(100); 

}


void runPlantSystem(ModeTanamanHias &plantMode, int choice){
 
  String mode[] = {"anggrek", "anthurium", "jandabolong"};
  byte value;
  

      if(mode[choice].compareTo("anggrek") == 0){
        
        for(int index = 0; index < sizeof(data); index++){
          EEPROM.update(Address + index, data[index]);
        }

        char BacaData[sizeof(data)];
        for(int index2 = 0; index2 < sizeof(BacaData); index2++){
          BacaData[index2] = EEPROM.read(0 + index2);
        }

        Serial.println(BacaData);

      } else if(mode[choice].compareTo("anthurium") == 0){
       Serial.println("anthurium");

         for(int index = 0; index < sizeof(data2); index++){
          EEPROM.update(Address + index, data2[index]);
        }

        char BacaData[sizeof(data2)];
        for(int index2 = 0; index2 < sizeof(BacaData); index2++){
          BacaData[index2] = EEPROM.read(0 + index2);
        }
      }

}

void yourFunction() {
   int arraySize = sizeof(dats) / sizeof(dats[0]); // Mendapatkan ukuran array dats

   // Simpan nilai dats[] ke dalam array lain
   int copiedArray[arraySize];
   for (int i = 0; i < arraySize; i++) {
       copiedArray[i] = dats[i];
   }

   int datss = copiedArray[0] * 100 +  copiedArray[1] * 10 +  copiedArray[2];
   lcd.print("A1.txt=\"");
   lcd.print(datss);
   lcd.print("\"");
   lcd.write(0xff);
   lcd.write(0xff);
   lcd.write(0xff);

}

void setup() {

  pinMode(Relay_Kipas, OUTPUT);
  pinMode(Relay_Peltier, OUTPUT);
  pinMode(Relay_Mistmaker, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600);
  lcd.begin(9600);
  Wire.begin();
  dht.begin();
  lightMeter.begin();

  myPID.SetMode(AUTOMATIC);

  myPID.SetTunings(Kp, Ki, Kd);

  modeTerpilih = {75, 28, 25};
  digitalWrite(Relay_Peltier, HIGH);
  digitalWrite(Relay_Kipas, HIGH);

}

void loop(){
  int f = 1;
  int x; int z; int K; int count = 0; 
  unsigned long Interval3;

  kelembaban = dht.readHumidity();
  temperatur = dht.readTemperature();
  lux = lightMeter.readLightLevel();
  running();

    if (lcd.available() && lock == 0) {
              z = lcd.read(); 
              Serial.println(z); 
              if(isPrintable(z)){ 
                Serial.println(z);
              }
              if(z == 83){
                lock = 1;
              } 
    }

   if (lcd.available() && lock == 1) {
            x = lcd.read();
            if(isPrintable(x)){ 
              Serial.println(x);
            }
            if(x == 84){
              lock = 2;
            }
            if(x == 'A'){
              lock = 0;
            }
        }     

    while (lcd.available() && count < arraySize && lock == 2) {

          K = lcd.read();

          if(isPrintable(K)){ 
              Serial.println(K);
          }

          if(K == 83){
            lock = 1;
          }

          if(K == 97){
            if(SuhuSementara > 20){
               SuhuSementara -= 1;
            }
            Serial.println("suhu" + String(SuhuSementara));
          }

          if(K == 98){
            if(SuhuSementara < 30){
               SuhuSementara += 1;
            }
            Serial.println("suhu" + String(SuhuSementara));
    
          }

          if(K == 99){
            if(KelembabanSementara > 60){
               KelembabanSementara -= 5;
            }
            Serial.println("kelem" + String(KelembabanSementara));
          }

          if(K == 100){
            if(KelembabanSementara < 80){
               KelembabanSementara += 5;
            }
            Serial.println("kelem" + String(KelembabanSementara));
          }

          if(K == 101){
            if(LuxSementara > 15){
               LuxSementara -= 5;
            }
            Serial.println("Lux" + String(LuxSementara));
          }

          if(K == 102){
            if(LuxSementara < 50){
               LuxSementara += 5;
            }
            Serial.println("Lux" + String(LuxSementara));
          }

          if(K == 109){
            if(SuhuSementara != 26 || KelembabanSementara != 70 || LuxSementara != 30){
              modeTerpilih = {KelembabanSementara, SuhuSementara, LuxSementara};
              SuhuSementara = 26;
              KelembabanSementara = 70;
              LuxSementara = 30;
            } 
          }

          // char c = lcd.read();
          // if (c >= '0' && c <= '9') { 
          //   dats[count] = c - '0';    
          //   count++;                  
          // }

    }

    if (count > 0) {
      Serial.print("Angka yang terbaca: ");
      for (int i = 0; i < count; i++) {
        Serial.print(dats[i]); // Mencetak angka ke Serial Monitor
      }
      Serial.println(); // Pindah ke baris baru setelah mencetak
    }
     x = x - 49;
     if (x == 0){

      modeTerpilih = {50, 25, 40};

      Serial.println("1d");

     }else if(x == 1){

      modeTerpilih = {80, 33, 10};

      Serial.println("2d");
    }
  
  delay(100);
  runPlantSystem(modeTerpilih, x);
  Subsistem_Kelembaban(modeTerpilih);
  Subsistem_Subu(modeTerpilih);
  Subsisten_Cahaya(modeTerpilih);
    
}