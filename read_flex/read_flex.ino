
#define ADC_ONESHOT_FORCE_USE_ADC2_ON_C3 true

const int flx = 5;
const int s2 = D0;
const int s1 = D1;
const int s0 = D2; 

void setup() {
  
  pinMode(flx, INPUT);  // declare the sensorPin as an INPUT
  pinMode(s2, OUTPUT);   // declare the ledPin as an OUTPUT
  pinMode(s1, OUTPUT);
  pinMode(s0, OUTPUT);

  digitalWrite(s2, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);


  Serial.begin(115200);
  while (!Serial);
}

void loop() {
  // read the value from the sensor:
  int sensorValue = analogRead(flx);

  Serial.println(sensorValue);
}
