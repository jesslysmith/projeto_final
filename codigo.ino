#include "Led.h"
#include "Buzzer.h"
#include <LiquidCrystal.h>
#include<SoftwareSerial.h>
const int sensor = A0;
Led led(D1);
Buzzer buzzer(D2);
const String LED = "\"LED\":";
const String BUZZER = "\"BUZZER\":";
const String T_OFF = "\"T_OFF\":";
const String SENSOR = "\"SENSOR\":";
const int gatilhosensor = 820; //valor de gatilho
bool flag; //flag para impressão do alerta de perigo
const int rs = D3, en = D0, d4 = D4, d5 = D5, d6 = D6, d7 = D7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String JSON_INDICATORS(){

   return "{" +
      LED + " " + (String)led.status() + "," +
      BUZZER + " " + (String)buzzer.status() +
      "}";
 }

String JSON_SENSOR(){
 int sensor_value = analogRead(sensor);
 String str;
 str+="{";
 str+=SENSOR;
 str+=' ';
 str+=(String) sensor_value;
 str+="}";
 return str;
}

void desligarAlarme(String comando){
   if(comando.startsWith("{") && comando.endsWith("}")){
      if(comando.indexOf(T_OFF +" 0") > 0){
          led.apagar();
          buzzer.desligar();
      }
   }
}

void setup() {
  pinMode(sensor, INPUT);
  Serial.begin(115200);
  lcd.begin(16,2);
  delay(2000);
}

void loop() {
  int analogSensor = analogRead(sensor);
  
  if(Serial.available() > 0){
    String comando = Serial.readString();
    desligarAlarme(comando);
    Serial.println(JSON_SENSOR());
    Serial.print(JSON_INDICATORS());
  }
  
  lcd.clear();
  flag=0;
  lcd.print("Em funcionamento!");
  lcd.setCursor(0, 1);
  delay (200);
  Serial.println(JSON_SENSOR());
  Serial.print(JSON_INDICATORS());
  
  // confronto entre valor de gatilho e o valor recebido pelo sensor
  while(analogSensor > gatilhosensor)
  {
    led.acender();
    buzzer.acionar();
    delay (500);
    Serial.println(JSON_SENSOR());
    Serial.print(JSON_INDICATORS());
    delay(10);
    if (flag==0){
      lcd.clear();
      lcd.print("PERIGO! NIVEL ");
      lcd.setCursor(0, 1);
      lcd.print ("ALARMANTE!");
      delay(200);
      flag=1;
    }
    analogSensor = analogRead(sensor); //função repetida para que o valor lido seja atualizado
  }
}
