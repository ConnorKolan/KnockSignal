void setup() {
  // Initialisierung des seriellen Monitors
  Serial.begin(115200);
  pinMode(A0, INPUT);

}


void loop() {
  //TODO writePattern in die loop holen
  int val = 0;
  for(int i = 0; i < 100; i++){
    val += analogRead(A0);
    delay(5);
  }

  int aRead = val / 100;

  float voltage = aRead * (5.0/1023.0);
  float current = (voltage / 10) * 1000;

  Serial.print("Current: ");
  Serial.print(current);
  Serial.println("mA");

}
