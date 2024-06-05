
#include <bluefruit.h>

const int flx1 = D1;
const int flx2 = D2;
const int flx3 = D5;
const int flx4 = D4;
const int flx5 = D3;

const int flexPins[5] = {flx1, flx2, flx3, flx4, flx5};

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

const float FILTER_ALPHA = 0.5;
ExponentialFilter flexFilters[5] = {
  ExponentialFilter(FILTER_ALPHA, 0),
  ExponentialFilter(FILTER_ALPHA, 0),
  ExponentialFilter(FILTER_ALPHA, 0),
  ExponentialFilter(FILTER_ALPHA, 0),
  ExponentialFilter(FILTER_ALPHA, 0)
};

void flex_setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(flexPins[i], INPUT);
  }

  analogReadResolution(14);
}

void flex_loop() {
  // read the value from the sensor:
  int flexVals[5];

  for (int i = 0; i < 5; i++) {
    int valr = analogRead(flexPins[i]);
    int val = flexFilters[i].filter(valr);
    flexVals[i] = val;
    Serial.print(flexPins[i]);
    Serial.print(": ");
    Serial.println(val);
  }
  
  send_flex(flexVals, 5);

  delay(50);
}


