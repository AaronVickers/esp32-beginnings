// LED details
#define LED_RED 33
#define LED_YELLOW 15
#define LED_GREEN 32

// Capacitive touch details
#define CAPACITIVE_TOUCH 14
#define CAPACITIVE_TOUCH_THRESHOLD 20

// Entry point function
void setup() {
  // Start serial connection
  Serial.begin(115200);
  
  // Set up LED pin modes
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

// Function to turn specified LED on for 1s
void turnOnForOneSecond(int ledNumber) {
  digitalWrite(ledNumber, HIGH);
  delay(1000);
  digitalWrite(ledNumber, LOW);
}

// Loop (after setup) function
void loop() {
  // Read capacitive touch value
  int touchValue = touchRead(CAPACITIVE_TOUCH);
  
  // Print capacitive touch value
  Serial.println(touchValue);
  
  if (touchValue < CAPACITIVE_TOUCH_THRESHOLD) {
    // Touch detected; cycle through LEDs
    turnOnForOneSecond(LED_RED);
    turnOnForOneSecond(LED_YELLOW);
    turnOnForOneSecond(LED_GREEN);
    turnOnForOneSecond(LED_YELLOW);
    turnOnForOneSecond(LED_RED);
  } else {
    // Touch not detected; LEDs off
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
  }
}
