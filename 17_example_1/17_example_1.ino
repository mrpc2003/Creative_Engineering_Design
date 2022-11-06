#include <Servo.h> // include the Servo library

// Arduino pin assignment
#define PIN_POTENTIOMETER 3 // Potentiometer at Pin A3

// Add IR Sensor Definition Here !!!
#define IR_Sensor 0 // IR Sensor at Pin A0

#define PIN_LED 9    // LED at Pin 9
#define PIN_SERVO 10 // Servo at Pin 10

#define DIST_MIN 100 // Minimum distance in cm
#define DIST_MAX 250 // Maximum distance in cm

#define _EMA_ALPHA 0.3 // Exponential Moving Average (EMA) alpha value

#define _DUTY_MIN 553  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counter-clockwise position (180 degree)

#define LOOP_INTERVAL 50 // Loop Interval (unit: msec)

Servo myservo;                        // create servo object to control a servo
float dist_ema, dist_prev = DIST_MIN; // Exponential Moving Average (EMA) of distance
unsigned long last_loop_time;         // unit: msec

void setup()
{
  pinMode(PIN_LED, OUTPUT);             // set LED pin as output
  myservo.attach(PIN_SERVO);            // attaches the servo on pin 10 to the servo object
  myservo.writeMicroseconds(_DUTY_NEU); // set servo to neutral position
  Serial.begin(2000000);                // set serial port baud rate
}

void loop()
{
  unsigned long time_curr = millis(); // unit: msec
  int a_value, duty, dist;            // analog value, duty cycle, distance

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;                        // not yet
  last_loop_time += LOOP_INTERVAL; // update last event time

  // Read IR Sensor value !!!
  a_value = analogRead(IR_Sensor); // read the input pin

  // Convert IR sensor value into distance !!!
  dist = (6762.0 / (a_value - 9) - 4.0) * 10.0 - 60.0; // cm

  // we need distance range filter here !!!
  if (dist < DIST_MIN)
  {                           // cut lower than minimum
    dist = dist_prev;         // use previous value
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else if (dist > DIST_MAX)
  {                           // Cut higher than maximum
    dist = dist_prev;         // use previous value
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else
  {                           // In desired Range
    digitalWrite(PIN_LED, 0); // LED ON
    dist_prev = dist;         // Update previous distance
  }
  // we need EMA filter here !!!
  dist_ema = _EMA_ALPHA * dist + (1 - _EMA_ALPHA) * dist_ema; // EMA filter

  // map distance into duty
  if (dist_ema < DIST_MIN)
  {                                       // cut lower than minimum
    myservo.writeMicroseconds(_DUTY_MIN); // set servo to minimum position
  }
  else if (dist_ema > DIST_MAX)
  {                                       // cut higher than maximum
    myservo.writeMicroseconds(_DUTY_MAX); // set servo to maximum position
  }
  else
  {                                                             // In desired Range
    myservo.writeMicroseconds(_DUTY_MIN + 12.3 * (dist - 100)); // set servo to desired position
    duty = _DUTY_MIN + 12.3 * (dist - 100);
  };

  // print IR sensor value, distnace, duty !!!
  Serial.print("MIN:");
  Serial.print(DIST_MIN); // print minimum distance
  Serial.print(",IR:");
  Serial.print(a_value); // print IR sensor value
  Serial.print(",dist:");
  Serial.print(dist); // print distance
  Serial.print(",ema:");
  Serial.print(dist_ema); // print EMA distance
  Serial.print(",servo:");
  Serial.print(duty); // print servo duty
  Serial.print(",MAX:");
  Serial.print(DIST_MAX); // print maximum distance
  Serial.println("");     // print new line
}