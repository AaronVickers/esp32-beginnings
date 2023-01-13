// WiFi details
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// Include WiFi header
#include <WiFi.h>

// Entry point function
void setup() {
  // Start serial connection
  Serial.begin(115200);

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
}

// Loop (after setup) function
void loop() {
  // Print WiFi connection details
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // 5s delay
  delay(5000);
}
