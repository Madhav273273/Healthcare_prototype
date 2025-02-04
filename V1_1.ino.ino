#include <WiFi.h>

// *Wi-Fi Configuration*
const char* ssid = "ZTE_2.4G_6SDbuZ";         // Replace with your Wi-Fi SSID
const char* password = "Q3AC29Yd"; // Replace with your Wi-Fi password
void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to Wi-Fi...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print IP Address
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Do nothing in the loop
}