// Include configuration
#include "config.h"

// Include required headers
#include <WiFi.h>
#include "WiFiClientSecure.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Light variables
uint8_t lightIntensity = 100;
bool lightStatus = false;

// WiFiClientSecure for SSL/TLS support
WiFiClientSecure client;

// Set up the MQTT client class by passing in the WiFi client, MQTT server, and login details
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_PORT, AIO_USERNAME, AIO_KEY);

// Set up light intensity and light status subscribe feed
Adafruit_MQTT_Subscribe lightIntensityFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/" FEED_LIGHT_INTENSITY);
Adafruit_MQTT_Subscribe lightStatusFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/" FEED_LIGHT_STATUS);

// Set up light intensity and light status /get feeds
Adafruit_MQTT_Publish getLightIntensityFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" FEED_LIGHT_INTENSITY "/get");
Adafruit_MQTT_Publish getLightStatusFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" FEED_LIGHT_STATUS "/get");

// Set up photocell publish feed
Adafruit_MQTT_Publish photocellFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/" FEED_PHOTOCELL);

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

  // Print connecting to WiFi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Print connected to WiFi
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set Adafruit IO root CA
  client.setCACert(AIO_ROOT_CA);

  // Set up light intensity and light status callbacks
  lightIntensityFeed.setCallback(handleLightIntensity);
  lightStatusFeed.setCallback(handleLightStatus);

  // Set up light intensity and light status subscriptions
  mqtt.subscribe(&lightIntensityFeed);
  mqtt.subscribe(&lightStatusFeed);
  
  // Establish MQTT session
  mqttConnect();

  // Send /get requests for light intensity and light status feeds
  getLightIntensityFeed.publish("\0");
  getLightStatusFeed.publish("\0");
}

// Loop (after setup) function
void loop() {
  // Establish MQTT session
  mqttConnect();

  // Get LDR reading
  int ldrReading = analogRead(LDR_PIN);

  // Print LDR data being sent
  Serial.print("Sending LDR: ");
  Serial.print(ldrReading);

  // Attempt to publish LDR data
  bool success = photocellFeed.publish(ldrReading);
  
  // Handle success of publishing LDR data
  if (success) {
    Serial.println(" (Success)");
  } else {
    Serial.println(" (Failed)");
  }

  // Listen for subscriptions for 5 seconds (also avoids Adafruit IO rate limit)
  mqtt.processPackets(5000);

  // No ping required because publish is done within KEEPALIVE time
}

// Function to handle light intensity changes
void handleLightIntensity(uint32_t lightIntensityRaw) {
  // Get light intensity value (constrained to percentage values)
  lightIntensity = constrain(lightIntensityRaw, 0, 100);

  // Print light intensity data received
  Serial.print("Light intensity: ");
  Serial.println(lightIntensity);

  // Handle light intensity change
  handleLightChange();
}

// Function to handle light status changes
void handleLightStatus(char *lightStatusString, uint16_t len) {
  // Get light status value
  if (strncmp(lightStatusString, "True", len) == 0) {
    lightStatus = true;
  } else {
    lightStatus = false;
  }

  // Print light status data received
  Serial.print("Light status: ");
  if (lightStatus) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // Handle light status change
  handleLightChange();
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

// Function to establish MQTT connection
void mqttConnect() {
  // Return if already connected
  if (mqtt.connected()) {
    return;
  }

  // Print connecting to Adafruit IO
  Serial.println("Connecting to Adafruit IO via MQTT...");

  // Result store and retries counter
  uint8_t result;
  uint8_t retries = 3;

  // Attempt to connect to Adafruit IO
  while ((result = mqtt.connect()) != 0) {
    // Print error and retrying message
    Serial.println(mqtt.connectErrorString(result));
    Serial.println("Retrying in 5 seconds...");

    // Disconnect before reconnecting
    mqtt.disconnect();

    // Decrease retries
    retries--;

    // Stop when no more retries
    if (retries == 0) {
      Serial.println("Failed to connect to Adafruit IO via MQTT!");
      
      while (1);
    }

    // Delay between retries
    delay(5000);
  }

  // Print connected to Adafruit IO
  Serial.println("Connected to Adafruit IO via MQTT!");
}
