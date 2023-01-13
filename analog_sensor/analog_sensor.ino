// LED details
#define LED_PIN 27
#define LED_PWM_CHANNEL 0
#define LED_PWM_FREQUENCY 1000
#define LED_PWM_RESOLUTION 12

// LDR details
#define LDR_PIN 32
#define LDR_MIN 700
#define LDR_MAX 2300

// Entry point function
void setup() {
  // Start serial connection
  Serial.begin(115200);

  // Set up LED PWM channel and pin
  ledcSetup(LED_CHANNEL, LED_FREQUENCY, LED_RESOLUTION);
  ledcAttachPin(LED_PIN, LED_CHANNEL);

  // Set up LDR pin mode
  pinMode(LDR_PIN, INPUT);
}

// Loop (after setup) function
void loop() {
  // Read LDR value
  int ldrRaw = analogRead(LDR_PIN);
  
  // Constrain LDR value to specified bounds
  int ldrContrained = constrain(ldrRaw, LDR_MIN, LDR_MAX);

  // Map LDR value to LED value (inverted)
  int ledValue = map(ldrContrained, LDR_MIN, LDR_MAX, pow(2, LED_RESOLUTION), -1);

  // Print debug information
  Serial.print("LDR Raw: ");
  Serial.print(ldrRaw);
  Serial.print(", LDR Mapped: ");
  Serial.print(ldrContrained);
  Serial.print(", LED Value: ");
  Serial.println(ledValue);
  
  // Set LED channel intensity
  ledcWrite(LED_CHANNEL, ledValue);
}
