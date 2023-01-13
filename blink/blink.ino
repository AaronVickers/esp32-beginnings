// LED details
#define LED_PIN 12
#define LED_ON_DURATION 50
#define LED_OFF_DURATION 1000 - LED_ON_DURATION

// Entry point function
void setup() {
  pinMode(LED_PIN, OUTPUT);
}

// Loop (after setup) function
void loop() {
  // Light on
  digitalWrite(LED_PIN, HIGH);
  delay(LED_ON_DURATION);

  // Light off
  digitalWrite(LED_PIN, LOW);
  delay(LED_OFF_DURATION);
}
