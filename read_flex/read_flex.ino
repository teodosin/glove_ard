#include <bluefruit.h>

const int flx1 = D1;
const int flx2 = D2;
const int flx3 = D3;
const int flx4 = D4;
const int flx5 = D5;

const int s2 = D0;
const int s1 = D1;
const int s0 = D2; 

const int WINDOW_SIZE = 10; // Adjust the window size as needed
int sensorReadings[WINDOW_SIZE];
int readIndex = 0;
int total = 0;

int flx_bottom = 16383;
int flx_top = 0;

BLEUart bleuartr; // uart over ble

class ExponentialFilter {
private:
  float alpha;
  float filteredValue;

public:
  ExponentialFilter(float alpha, float initialValue) {
    this->alpha = alpha;
    this->filteredValue = initialValue;
  }

  float filter(float rawValue) {
    filteredValue = alpha * rawValue + (1 - alpha) * filteredValue;
    return filteredValue;
  }

  float getCurrentValue() {
    return filteredValue;
  }
};

ExponentialFilter flexFilter(0.05, 0);

void flex_setup() {
  pinMode(flx1, INPUT);
  pinMode(flx2, INPUT);
  pinMode(flx3, INPUT);
  pinMode(flx4, INPUT);
  pinMode(flx5, INPUT);  
  pinMode(s2, OUTPUT);   
  pinMode(s1, OUTPUT);
  pinMode(s0, OUTPUT);


  set_selector(4, s0, s1, s2);
  analogReadResolution(14);

  Serial.begin(9600);
  while (!Serial);
}

void flex_loop() {
  // read the value from the sensor:
  int raw = analogRead(flx1);

  // int smooth = flexFilter.filter(raw);
  int smooth = raw;

  // Send the average sensor vWalue
  Serial.println(smooth);
  send_flex(smooth);

  delay(50);
}

void set_selector(int pin, int s0, int s1, int s2) {
  switch (pin) {
    case 0: // A0
      digitalWrite(s2, LOW);
      digitalWrite(s1, LOW);
      digitalWrite(s0, LOW);
      break;
    case 1: // A1
      digitalWrite(s2, LOW);
      digitalWrite(s1, LOW);
      digitalWrite(s0, HIGH);
      break;
    case 2: // A2
      digitalWrite(s2, LOW);
      digitalWrite(s1, HIGH);
      digitalWrite(s0, LOW);
      break;
    case 3: // A3
      digitalWrite(s2, LOW);
      digitalWrite(s1, HIGH);
      digitalWrite(s0, HIGH);
      break;
    case 4: // A4
      digitalWrite(s2, HIGH);
      digitalWrite(s1, LOW);
      digitalWrite(s0, LOW);
      break;
    case 5: // A5
      digitalWrite(s2, HIGH);
      digitalWrite(s1, LOW);
      digitalWrite(s0, HIGH);
      break;
    case 6: // A6
      digitalWrite(s2, HIGH);
      digitalWrite(s1, HIGH);
      digitalWrite(s0, LOW);
      break;
    case 7: // A7
      digitalWrite(s2, HIGH);
      digitalWrite(s1, HIGH);
      digitalWrite(s0, HIGH);
      break;
  }
}


