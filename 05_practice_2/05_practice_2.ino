int LED_07 = 7;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_07, OUTPUT);
  
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_07, LOW);   // turn the LED on ==== +/- changed
  delay(1000);                       // wait for a second
  for (int i = 0; i < 5; i++) {
  digitalWrite(LED_07, HIGH);
  delay(100);
  digitalWrite(LED_07, LOW);
  delay(100);
  }
  digitalWrite(LED_07, HIGH);
  while(1){}
}
