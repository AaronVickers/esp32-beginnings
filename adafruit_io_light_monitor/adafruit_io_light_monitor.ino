// Include configuration
#include "config.h"

// Include Adafruit IO WiFi client
#include "AdafruitIO_WiFi.h"

// Light variables
int lightIntensity = 100;
bool lightStatus = false;

// Set Adafruit IO configuration
AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WIFI_SSID, WIFI_PASSWORD);

// Adafruit IO feeds
AdafruitIO_Feed *lightIntensityFeed = io.feed(FEED_LIGHT_INTENSITY);
AdafruitIO_Feed *lightStatusFeed = io.feed(FEED_LIGHT_STATUS);
AdafruitIO_Feed *photocellFeed = io.feed(FEED_PHOTOCELL);

// Entry point function
void setup() {
  // Start serial connection
  Serial.begin(115200);

  // Wait for serial connection
  while (!Serial);

  // Set up LED PWM channel and pin
  ledcSetup(LED_PWM_CHANNEL, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, LED_PWM_CHANNEL);

  // Set up LDR pin mode
  pinMode(LDR_PIN, INPUT);

  // Connect to Adafruit IO
  io.connect();

  // Subscribe to light intensity feed
  lightIntensityFeed->onMessage(handleLightIntensity);

  // Subscribe to light status feed
  lightStatusFeed->onMessage(handleLightStatus);

  // Wait for connection to Adafruit IO
  Serial.print("Connecting to Adafruit IO");
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Get current light intensity
  lightIntensityFeed->get();

  // Get current light status
  lightStatusFeed->get();

  // Print connection status
  Serial.println();
  Serial.println(io.statusText());
}

// Function to handle all light changes
void handleLightChange() {
  // Default to no light
  int lightLevel = 0;

  // If light is on
  if (lightStatus) {
    // Calculate light level
    lightLevel = pow(2, LED_PWM_RESOLUTION) * lightIntensity / 100;
  }

  // Set LED light level
  ledcWrite(LED_PWM_CHANNEL, lightLevel);
}

// Function to handle light intensity changes
void handleLightIntensity(AdafruitIO_Data *data) {
  // Get light intensity value (constrained to percentage values)
  lightIntensity = constrain(data->toInt(), 0, 100);

  // Print data received
  Serial.print("Light intensity: ");
  Serial.println(lightIntensity);

  // Handle light intensity change
  handleLightChange();
}

// Function to handle light status changes
void handleLightStatus(AdafruitIO_Data *data) {
  // Get light status value
  lightStatus = data->toBool();

  // Print data received
  Serial.print("Light status: ");
  if (lightStatus) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // Handle light status change
  handleLightChange();
}

// Loop (after setup) function
void loop() {
  // Keep Adafruit IO client connected
  io.run();

  // Get LDR reading
  int ldrReading = analogRead(LDR_PIN);

  // Print data being sent
  Serial.print("Sending LDR: ");
  Serial.println(ldrReading);

  // Publish data to photocell feed
  photocellFeed->save(ldrReading);

  // Delay for Adafruit IO rate limit
  delay(3000);
}
