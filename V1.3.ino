// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL3gTF4jgGw"
#define BLYNK_TEMPLATE_NAME "iot healthcare"
#define BLYNK_AUTH_TOKEN "-g9jjSC1nMbqwqZyfZ_XTWYh7nOwHORj"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <U8g2lib.h>

// *Wi-Fi Credentials*
const char* ssid = "ZTE_2.4G_6SDbuZ";         // Replace with your Wi-Fi SSID
const char* password = "Q3AC29Yd"; // Replace with your Wi-Fi password

// *Blynk Auth Token*
char auth[] = BLYNK_AUTH_TOKEN;  

// *I2C OLED Display (128x64, I2C)*
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 22, 21);

// *Time Tracking*
uint32_t tsLastReport = 0;
#define REPORTING_PERIOD_MS 1000  // Update every 1 second

void setup() {
  Serial.begin(115200);
  Serial.println("Blynk Dummy Healthcare System");

  // *Initialize OLED*
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  Serial.println("OLED Initialized");

  // *Connect to Wi-Fi*
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // *Connect to Blynk*
  Blynk.begin(auth, ssid, password);
}

void loop() {
  Blynk.run();  // Keep Blynk connected

  // *Generate Dummy Sensor Values*
  float heartRate = random(60, 100);  // Dummy BPM between 60-100
  float spO2 = random(95, 100);       // Dummy SpO2 between 95-100%
  float temperatureC = random(36, 38) + (random(0, 9) / 10.0); // Dummy Temp 36-38°C

  // *Update OLED Every Second*
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    // *Display on Serial Monitor*
    Serial.print("Heart Rate: ");
    Serial.print(heartRate);
    Serial.print(" BPM | SpO2: ");
    Serial.print(spO2);
    Serial.print(" % | Temp: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // *Update OLED Display*
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Heart Rate: ");
    u8g2.print(heartRate);
    u8g2.print(" BPM");

    u8g2.setCursor(0, 40);
    u8g2.print("SpO2: ");
    u8g2.print(spO2);
    u8g2.print(" %");

    u8g2.setCursor(0, 60);
    u8g2.print("Temp: ");
    u8g2.print(temperatureC);
    u8g2.print(" C");

    u8g2.sendBuffer();

    // *Send Values to Blynk Dashboard*
    Blynk.virtualWrite(V0, heartRate);   // Send BPM to Blynk (V0)
    Blynk.virtualWrite(V1, spO2);        // Send SpO2 to Blynk (V1)
    Blynk.virtualWrite(V2, temperatureC);// Send Temperature to Blynk (V2)
  }

  delay(10);  // Small delay for stability
}