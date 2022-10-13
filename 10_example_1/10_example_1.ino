#include <Servo.h>
#define PIN_SERVO 10

Servo myservo;
int velo = 170;

void setup() {
  myservo.attach(PIN_SERVO); 
  myservo.write(90);
  delay(1000);
}

void loop() {
    // add code here.
    myservo.write(0);
    delay(velo);
    myservo.write(90);
    delay(velo);
    myservo.write(180);
    delay(velo);
    myservo.write(90);
    delay(velo);

}
