#define START_PIN 26
#define STOP_PIN 14
#define OUTPUT_PIN 25

bool latched = false;

void setup() {
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  bool start = digitalRead(START_PIN); // read the START pin
  bool stop = digitalRead(STOP_PIN); // read the STOP pin

  if (!stop)
      { latched = false; } 
  
    else if (!start) 
      { latched = true; }

  digitalWrite(OUTPUT_PIN, latched);

  delay(10);
}