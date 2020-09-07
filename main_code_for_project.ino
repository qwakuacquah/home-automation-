#include <LiquidCrystal_I2C.h>
#include <Nextion.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include "DHT.h"
#include <SPI.h>
#include <MFRC522.h>
#define DHTTYPE DHT11
#define DHTPIN 26
#define SS_PIN 53
#define RST_PIN 5
//////////////////////////////////////////////////////////////////////////////////
//NEXTION
SoftwareSerial nextion(68, 69);
Nextion myNextion(nextion, 9600);
boolean button1State;
boolean button2State;
boolean button3State;
boolean button4State;
boolean button5State;
boolean button6State;
//DHT
DHT dht(DHTPIN, DHTTYPE);
int temp = 0.0f;
int hum = 0.0f;
//servo
Servo myservo;
//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
//MP3
SoftwareSerial mp3(7, 8);

//RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;
int pinServo = 27;
int bulb = 24;
int curtain = 22;
int FAN = 25;
int sensor = 0;
int state = 0;
int Finger = 0;
unsigned char Data[10];
unsigned char i;
String message = myNextion.listen();

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
   Serial3.begin(57600);
 mp3.begin(9600);
  SPI.begin();  
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  dht.begin();
  myNextion.init(); 
  pinMode(FAN, OUTPUT);
  pinMode(bulb, OUTPUT);
  pinMode(curtain, OUTPUT);
 
  
  displayCodeEntryScreen();
  
}

void loop()                     
{

  opendoor();
  if (message >= "") {
    device();
  }
  sensors();
  bluetooth();
  
}

void readSensor()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();
}
void bluetooth()
{
   if(Serial3.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial3.read(); // Reads the data from the serial port
 Serial.println("ready");
 }

 if (state == '1') {
  digitalWrite(bulb, LOW); // Turn LED OFF
Serial3.println("BULB: ON"); // Send back, to the phone, the String "LED: ON"
  state=0;
 }
 else if (state == '0') {
  digitalWrite(bulb, HIGH);
  Serial3.println("BULB: OFF ");;
state=0;
 } 
 if (state == '2') {
  digitalWrite(FAN, LOW); // Turn LED OFF
Serial3.println("FAN: ON "); // Send back, to the phone, the String "LED: ON"
  state=0;
 }
 else if (state == '3') {
  digitalWrite(FAN, HIGH);
  Serial3.println("FAN: OFF");;
state=0;
 } 
  }

void device(void) {
  String message = myNextion.listen();
  if (message == "65 1 4 0 ffff ffff ffff") {
    myNextion.buttonToggle(button1State, "b1", 1, 1);
    digitalWrite(bulb, HIGH);
  }
  else if (message == "65 1 3 0 ffff ffff ffff") {
    myNextion.buttonToggle(button2State, "b2", 1, 1);
    digitalWrite(bulb, LOW);
  }
  else  if (message == "65 2 4 1 ffff ffff ffff") {
    myNextion.buttonToggle(button3State, "b1", 1, 1);
    digitalWrite(FAN, HIGH);
  }
  else if (message == "65 2 3 1 ffff ffff ffff") {
    myNextion.buttonToggle(button4State, "b2", 1, 1);
    digitalWrite(FAN, LOW);
  }
   else if (message == "65 3 3 1 ffff ffff ffff") {
    myNextion.buttonToggle(button5State, "b1", 4, 4);
     myservo.attach(curtain);
  myservo.write(0);
  delay(6000);
  myservo.detach();
  }
  else if (message == "65 3 4 1 ffff ffff ffff") {
    myNextion.buttonToggle(button6State, "b2", 4,4);
     myservo.attach(curtain);
  myservo.write(180);
  delay(6000);
  myservo.detach();
  }
}
void sensors(void)
{
  readSensor();
  int temperature = temp;
  int humidity = hum;
  myNextion.setComponentText("temperature", String(temperature));
  myNextion.setComponentText("humidity", String(humidity));
  delay(100);
}
void opendoor(void)
{
  owners();
}
void closedoor(void)
{
  myservo.attach(pinServo);
  myservo.write(0);
  delay(2000);
  myservo.detach();
  displayCodeEntryScreen();
  delay(100);
}

void displayCodeEntryScreen()    // Dispalying start screen for users to enter PIN
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Home Security Sys");
  lcd.setCursor(0, 1);
  lcd.print("By Moses");
  delay (2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan finger");
  lcd.setCursor(0, 1);
  lcd.print("or Card");
}

void owners(void) {

  if (Serial1.available()) {
    Finger = Serial1.read();
    if (Finger == 1) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Welcome Moses");
      Serial.println("Welcome Moses");
      delay(100);
      welcomemoses();
      myservo.attach(pinServo);
      myservo.write(90);
      delay(1000);
      dooropened();
      delay(5000);
      closedoor();
      delay(500);
      doorclosed();
      displayCodeEntryScreen();
      delay(1000);
    }
    else if (Finger == 3) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Welcome PROF");
      delay(100);
      welcomeprofessor();
      Serial.println("Welcome prof");
      myservo.attach(pinServo);
      myservo.write(90);
      delay(1000);
      dooropened();
      delay(5000);
      closedoor();
      delay(500);
      doorclosed();
      displayCodeEntryScreen();
      delay(1000);
    }
      
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Выбор метки
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  uidDec = 0;

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }
  Serial.println("Card UID: ");
  Serial.println(uidDec);
  if (uidDec == 4246204884) // Сравниваем Uid метки, если он равен заданому то серва открывает.
  {
     welcomemoses();
    Serial.print("welcome");
    delay(1000);
    myservo.attach(pinServo);
    myservo.write(180);
    delay(1000);
    dooropened();
    delay (3000);
    closedoor();
    delay(1000);
    doorclosed();
  }
}
void dooropened() {
  mp3.write(0x7E);//INICIO
  mp3.write(0x04);//LENGTH
  mp3.write(0xA2);//A0= SD A1= SPI FLASH A2 UDISK
  mp3.write((byte) 0x00);
  mp3.write(0x01);//PISTA
  mp3.write(0x7E);//FIN
  delay(500);
}
void doorclosed() {
  mp3.write(0x7E);//INICIO
  mp3.write(0x04);//LENGTH
  mp3.write(0xA2);//A0= SD A1= SPI FLASH A2 UDISK
  mp3.write((byte) 0x00);
  mp3.write(0x02);//PISTA
  mp3.write(0x7E);//FIN
  delay(500);
}
void welcomemoses() {
  mp3.write(0x7E);//INICIO
  mp3.write(0x04);//LENGTH
  mp3.write(0xA2);//A0= SD A1= SPI FLASH A2 UDISK
  mp3.write((byte) 0x00);
  mp3.write(0x03);//PISTA
  mp3.write(0x7E);//FIN
  delay(500);
}
void welcomeprofessor() {
  mp3.write(0x7E);//INICIO
  mp3.write(0x04);//LENGTH
  mp3.write(0xA2);//A0= SD A1= SPI FLASH A2 UDISK
  mp3.write((byte) 0x00);
  mp3.write(0x04);//PISTA
  mp3.write(0x7E);//FIN
  delay(500);
}
