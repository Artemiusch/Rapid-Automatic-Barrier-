#include <SPI.h>
#include <MFRC522.h>
#include <ShiftLCD.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

const int I2 = 3; // dla RFID
const int I3 = A4; // pod Szlabanem
const int I1 = A5; // przed Szlabanem
const int I5 = 5; // czujnik otwarcia
const int I6 = 4; // czujnik zamkniecia

// Sterowanie silniczkiem
const int Q1 = 6; // sygnal do otwarcia
const int Q2 = 7; // sygnal do zamkniecia

long duration; // zmienna pomocnicza dla odleglosci przed szlabanem
int distance; // odleglosc przed szlabanem
long duration2; // zmienna pomocnicza dla odleglosci pod szlabanem
int distance2; // odleglosc pod szlabanem

const int trigpin = A0; // zdefiniowanie pinu nadawczego dla czujnika pod szlabanem
const int echopin = A1; // zdefiniowanie pinu odbiorczego dla czujnika pod szlabanem
const int trigpin2 = A2; // zdefiniowanie pinu nadawczego dla czujnika przed szlabanem
const int echopin2 = A3; // zdefiniowanie pinu odbiorczego dla czujnika przed szlabanem

// zmienne pomocnicze dla wyswietlacza
long d = 4; // odejmuje iteracji 
long milis = 0; // zmienna pomocnicza
int flag; 

ShiftLCD lcd(0,2,1); // wyswietlacz b
MFRC522 mfrc522( SS_PIN, RST_PIN); // definiowanie pinow dla RFID MFRC522

int oldg_1 = 8; // ograniczenia na odleglosc

// nasz piekny Silniczek Xd
Servo myServo;
int j = 86; // zmienna dla sterowania silniczkiem / 90-zamkniete / 0-otwarte

String zmienna = ""; // przepisanie odczytu karty RFID

void setup(){
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.begin(16,2);

  pinMode(I2,OUTPUT); // dla RFID
  pinMode(I3,OUTPUT); // pod szlabanem
  pinMode(I1,OUTPUT); // przed szlabanem
  pinMode(I5,OUTPUT); // czujnik całkowitego otwarcia
  pinMode(I6,OUTPUT); // czujnik całkowitego zamknięcia

  pinMode(trigpin, OUTPUT); // nadanie sygnaly
  pinMode(echopin, INPUT); // odbieranie sygnalu

  pinMode(trigpin2, OUTPUT); // nadanie sygnalu
  pinMode(echopin2, INPUT); // odbieranie sygnalu

  myServo.attach(8); // pin dla silniczka
  myServo.write(j); // poczatkowy kat silnika
  // czujniki calkowitego otwarcia oraz zamkniecia
  if ( j==0 ){
    digitalWrite(I5,HIGH);
    digitalWrite(I6,LOW);
    }
  else if ( j==86 ){
    digitalWrite(I5,LOW);
    digitalWrite(I6,HIGH);
    } 
  // wyswietlania poczatkowego komunikatu
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Please wait ");
  lcd.setCursor(0,1);
  lcd.print("  loading...");
  delay(2000);
}

void loop(){  
//wyswietlacza kod
  if(zmienna == ""){
  lcd.setCursor(0,0);
  lcd.print(" Witaj Kliencie ");
  lcd.setCursor(0,1);
  lcd.print("  Przyloz tag!  ");
 }else if(zmienna == "31 A0 2A 2E"){
  lcd.clear();
  lcd.print( "    Zgoda ");
  lcd.print(d);
  lcd.print("s");
  lcd.setCursor(0,1);
  lcd.print("Prosze wjezdzac!");
  if(millis()>1000+milis){
    d--;
    milis = millis();
  }
  if(d<0){
    zmienna = "";
    d=4;
  }
 }else if(zmienna == "86 37 52 83" ){
  lcd.setCursor(0,0);
  lcd.print( "   Odmowa ");
  lcd.print(d);
  lcd.print(" s   ");
  lcd.setCursor(0,1);
  lcd.print("Spobuj ponownie!");
  if(millis()>1000+milis){
  d--;
  milis = millis();
  }
  if(d<0){
    zmienna = "";
    d=4;
  }
 }
//podanie sygnalu do logo od RFiD
 if (zmienna=="31 A0 2A 2E"){
    digitalWrite(I2,LOW); // dla sygnalu na LOGO zaswieca DIODE czyli podaje 1
    delay(300);
  }
  else {
    digitalWrite(I2,HIGH);
    delay(300);
  }
// czujnik odleglosci pod szlabanem
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);

  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);

  duration = pulseIn(echopin,HIGH);

  distance = duration *0.034/2;

//warunek dla czujnika pod szlabanem
  if (distance<oldg_1){
          digitalWrite(I3,LOW); // dla sygnalu na LOGO zaswieca DIODE czyli podaje 1
          delay(30);

  } else {
          digitalWrite(I3,HIGH); // dla sygnalu na logo zgasza diode czyli podaje 0
          delay(30);

  }
// czujnik odleglosci przed szlabanem
  digitalWrite(trigpin2,LOW);
  delayMicroseconds(2);

  digitalWrite(trigpin2,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin2,LOW);

  duration2 = pulseIn(echopin2,HIGH);
  distance2 = duration2 *0.034/2;
//warunek dla czujnika przed szlabanem
  if (distance2<oldg_1){
         digitalWrite(I1,LOW); // dla sygnalu na LOGO zaswieca DIODE czyli podaje 1
         delay(30);

  } else {
        digitalWrite(I1,HIGH); // dla sygnalu na logo zgasza diode czyli podaje 0
        delay(30);
  }

// odbior sygnalu z logo Q1 i Q2
  if(digitalRead(Q2) == HIGH){
    while( j<86){     
      myServo.write(j);
      j++;
      delay(30);
      if(j<70){
  // czujnik odleglosci pod szlabanem
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);

  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);

  duration = pulseIn(echopin,HIGH);
  distance = duration *0.034/2;
//warunek dla czujnika pod szlabanem
  if (distance<oldg_1){
          digitalWrite(I3,LOW); // dla sygnalu na LOGO zaswieca DIODE czyli podaje 1
          delay(10);
          break;
  }else {
          digitalWrite(I3,HIGH); // dla sygnalu na logo zgasza diode czyli podaje 0
          delay(30);
  }}}
  if ( j==86 ){ // jesli zamkniete
    digitalWrite(I5,LOW); // czujnik otwarcia false
    digitalWrite(I6,HIGH); // czujnik zamkniecia true
    }}
  if(digitalRead(Q1) == HIGH){
    while( j>0 ){
      myServo.write(j);
      j--;
      delay(30);
      }
  if ( j==0 ){ // jesli otwarte
    digitalWrite(I5,HIGH); // czujnik otwarcia true
    digitalWrite(I6,LOW); // czujnik zamkniecia false
    }}
// sprawdzanie czy przylozono nowa karte
  if(! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
// odczyt ID karty
  if(! mfrc522.PICC_ReadCardSerial()){
    return;
  }

// przeksztalcenie ID karty
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] <0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
    content.toUpperCase();
    zmienna = content.substring(1);
}