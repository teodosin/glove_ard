
#define ADC_ONESHOT_FORCE_USE_ADC2_ON_C3 true

const int flx = 5;
const int s2 = D0;
const int s1 = D1;
const int s0 = D2; 

void flex_setup() {
  
  pinMode(flx, INPUT);  // declare the sensorPin as an INPUT
  pinMode(s2, OUTPUT);   // declare the ledPin as an OUTPUT
  pinMode(s1, OUTPUT);
  pinMode(s0, OUTPUT);

  set_selector(1, s0, s1, s2);

  Serial.begin(115200);
  while (!Serial);
}

void flex_loop() {
  // read the value from the sensor:
  int sensorValue = analogRead(flx);

  Serial.println(sensorValue);
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

