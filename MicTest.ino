#include <EEPROM.h>

const int outputPin = 12;
const int inputPinMic = A0;
const int ledPin = 9;
const int redLedPin = 10;
const int buttonPinIn = 2;

const float thresh = 1.95;

int counter = 0;


void setup() {
  // Initialisierung des seriellen Monitors
  Serial.begin(115200);

  // Konfiguration des Ausgangspins
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, HIGH);


  pinMode(ledPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buttonPinIn, INPUT);

  //digitalWrite(buttonPinOut, HIGH);


  analogWrite(ledPin, 60);
  delay(1000);
  analogWrite(ledPin, 0);

  // Initialisierung des seriellen Monitors
  Serial.begin(115200);

  // Konfiguration des Eingangspins
  pinMode(inputPinMic, INPUT);
  Serial.println("Starting");

  //writePattern();

}

long detectKnock(){
  long start = millis();
  while((analogRead(inputPinMic) * (5.0 / 1023.0)) <= thresh){
    if(millis() - start >= 5000){
      return -1;
    }
  }
  return millis();
}

void writePattern(){
  Serial.println("Writing Pattern now");
  long lastStep = detectKnock();
  delay(30);

  if(lastStep == -1){
    Serial.println("No knock detected resuming with old one");
    return;
  }

  long currentTime;
  int i = 0;

  //detect knock at the end of loop to wait for the next
  while((currentTime = detectKnock()) != -1){
    delay(10);
    int difference = currentTime - lastStep;
    byte second = (byte) (difference & 0xFF);
    byte first = (byte) ((difference >> 8) & 0xFF);

    Serial.print(first);
    Serial.print(" ");
    Serial.println(second);

    EEPROM.write(i++, first);
    EEPROM.write(i++, second);

    //TODO vielleicht ein delay hinzufügen 
    lastStep = currentTime;
  }

  EEPROM.write(i++, 255);
  EEPROM.write(i, 255);
  Serial.println("Finished Pattern now");
}

int matchPattern(){
  long lastStep = millis();
  delay(30);

  long currentTime;
  int i = 0;
  int tolerance = 50;

  byte first = EEPROM.read(i++);
  byte second = EEPROM.read(i++);

  while((currentTime = detectKnock()) != -1){
    int allowedDifference = 0;

    allowedDifference |= first;                  // Set the high byte
    allowedDifference = allowedDifference << 8;  // Shift the high byte to the left by 8 bits
    allowedDifference |= second; 
    
    Serial.print((currentTime - lastStep));
    Serial.print(" ");
    Serial.println(allowedDifference);

    if((currentTime - lastStep) <= (allowedDifference - tolerance) || 
      (currentTime - lastStep) >= (allowedDifference + tolerance)){
        return 0;
    }

    lastStep = currentTime;
    first = EEPROM.read(i++);
    second = EEPROM.read(i++);

    if((first == 255) && (second == 255)){
      return 1;
    }
    delay(30);
  }
}

void loop() {
  //TODO writePattern in die loop holen
  float x;

  if(digitalRead(buttonPinIn) == HIGH){
    Serial.println("Recording Pattern now");
    writePattern();
    Serial.println("Finished recording Pattern now");

    //wenn erster knock erfasst wird -> nächsten knock erwarten (matchPattern()) 1.75
  }else if(!((x = (analogRead(inputPinMic) * (5.0 / 1023.0))) <= thresh)){
    if(matchPattern() == 1){
      Serial.println("Opening lock");
      analogWrite(ledPin, 60);
    }else{
      analogWrite(redLedPin, 60);
      Serial.println("Wrong pattern");
    }
    delay(5000);
    analogWrite(ledPin, 0);
    analogWrite(redLedPin, 0);

    Serial.println("Listening again");
  }
}
