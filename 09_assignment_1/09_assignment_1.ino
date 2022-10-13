// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.5 // EMA weight of new sample (range: 0 to 1)
                       // Setting EMA to 1 effectively disables EMA filter.

// global variables
unsigned long last_sampling_time; // unit: msec
float dist_prev = _DIST_MAX;      // Distance last-measured
float dist_ema;                   // EMA distance

// 최근 n개의 샘플을 유지하는 배열
// #define SAMPLE_SIZE 3 // 샘플의 개수 = 3
// #define SAMPLE_SIZE 10 // 샘플의 개수 = 10
#define SAMPLE_SIZE 30 // 샘플의 개수 = 30

float samples[SAMPLE_SIZE]; // 샘플을 저장하는 배열
int sample_index = 0;       // 샘플의 인덱스

// 중위수 필터 구현
int dist_median; // median distance

float median_filter(float new_sample)
{
  samples[sample_index] = new_sample;              // 새로운 샘플을 배열에 저장
  sample_index = (sample_index + 1) % SAMPLE_SIZE; // 배열의 인덱스를 증가시킴

  float sorted_samples[SAMPLE_SIZE];                // 샘플을 정렬한 배열
  memcpy(sorted_samples, samples, sizeof(samples)); // 샘플을 정렬한 배열에 복사
  // memcpy(복사받을 메모리, 복사할 메모리, 길이)

  // 삽입정렬 구현
  for (int i = 0; i < SAMPLE_SIZE; i++) //중첩 for문
  {
    for (int j = i + 1; j < SAMPLE_SIZE; j++)
    {
      if (sorted_samples[i] > sorted_samples[j]) // i번째 샘플이 j번째 샘플보다 크면
      {
        float temp = sorted_samples[i];        // i번째 샘플을 임시 변수에 저장
        sorted_samples[i] = sorted_samples[j]; // j번째 샘플을 i번째 샘플에 저장
        sorted_samples[j] = temp;              // 임시 변수에 저장된 i번째 샘플을 j번째 샘플에 저장
      }
    }
  }
  return sorted_samples[SAMPLE_SIZE / 2]; // 중간값을 반환
}

void setup()
{
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);    // LED
  pinMode(PIN_TRIG, OUTPUT);   // Trigger
  pinMode(PIN_ECHO, INPUT);    // Echo
  digitalWrite(PIN_TRIG, LOW); // Trigger LOW

  // initialize serial port
  Serial.begin(57600);

  // initialize last sampling time
  last_sampling_time = 0;
}

void loop()
{
  float dist_raw; // raw distance

  // wait until next sampling time.
  // millis() returns the number of milliseconds since the program started.
  // Will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL) // INTERVAL = 25
    return;                                     // 아직 샘플링 시간이 되지 않았으면 함수 종료

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO); // PIN_TRIG = 12, PIN_ECHO = 13

  // apply median filter
  // dist_raw = median_filter(dist_raw);

  if (dist_raw < _DIST_MIN)
  {
    // dist_raw = _DIST_MIN - 10.0;    // Set Lower Value
    // dist_raw = dist_prev;     // Set Lower Value
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else if (dist_raw > _DIST_MAX)
  {
    // dist_raw = _DIST_MAX + 10.0;    // Set Higher Value
    // dist_raw = dist_prev;     // Set Lower Value
    digitalWrite(PIN_LED, 1); // LED OFF
  }
  else
  {                           // In desired Range
    digitalWrite(PIN_LED, 0); // LED ON
  }

  // Modify the below line to implement the EMA equation
  dist_ema = _EMA_ALPHA * dist_raw + (1 - _EMA_ALPHA) * dist_ema; // EMA equation
  dist_median = median_filter(dist_raw);                           // median filter

  // output the distance to the serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN); // _DIST_MIN = 100
  Serial.print(",raw:");
  Serial.print(dist_raw); // raw distance
  Serial.print(",ema:");
  Serial.print(dist_ema); // EMA distance
  Serial.print(",median:");
  Serial.print(dist_median); // median distance
  Serial.print(",Max:");
  Serial.print(_DIST_MAX); // _DIST_MAX = 300
  Serial.println("");

  // update last sampling time
  last_sampling_time += INTERVAL; // INTERVAL = 25
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION); // PULSE_DURATION = 10
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - round trip distance: 34.6m
  // - expected pulse duration: 0.1 sec, or 100,000us
  // - pulseIn(ECHO, HIGH, timeout) * 0.001 * 0.5 * SND_VEL
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346 * micro * sec * milli * meter
  //                                        ----------------------------
  //                                         micro * sec
  //        = 100 * 173 milli*meter = 17,300 mm = 17.3m
  // pulseIn() returns microseconds.
}
