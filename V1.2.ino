#include <WiFi.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>.

// *Wi-Fi Configuration*
const char* ssid = "ZTE_2.4G_6SDbuZ";         // Replace with your Wi-Fi SSID
const char* password = "Q3AC29Yd"; // Replace with your Wi-Fi password

// *I2C OLED Display (128x64, I2C)*
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 22, 21);

// *MAX30100 Pulse Oximeter Instance*
PulseOximeter pox;

// *Temperature Sensor (DS18B20) Pin*
#define TEMP_SENSOR_PIN 4
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// *ECG Sensor Pin*
#define ECG_PIN 34

// *Time Tracking Variables*
uint32_t tsLastReport = 0;
#define REPORTING_PERIOD_MS 1000

// *Callback for Heartbeat Detection*
void onBeatDetected() {
  Serial.println("Beat detected!");
}

// *Setup Wi-Fi*
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Display IP address on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Wi-Fi Connected");
  u8g2.setCursor(0, 40);
  u8g2.print("IP: ");
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();

  delay(3000); // Pause to show IP address on OLED
}

// *Setup Function*
void setup() {
  Serial.begin(115200);
  Serial.println("Pulse, SpO2, ECG & Temperature Monitoring System");

  // Initialize OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  Serial.println("OLED Initialized");

  // Initialize DS18B20 Temperature Sensor
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, 12);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();

  // Initialize MAX30100 Pulse Oximeter
  Serial.print("Initializing MAX30100...");
  if (!pox.begin()) {
    Serial.println("FAILED. Check wiring.");
    while (1);
  }
  Serial.println("SUCCESS");
  pox.setOnBeatDetectedCallback(onBeatDetected);

  // Setup Wi-Fi
  setupWiFi();
}

// *Loop Function*
void loop() {
  pox.update();                      // Update Pulse Oximeter Readings
  sensors.requestTemperatures();     // Request Temperature Reading

  // *Read Sensor Values*
  float heartRate = pox.getHeartRate();
  float spO2 = pox.getSpO2();
  float temperatureC = sensors.getTempCByIndex(0);

  // *Read ECG Sensor & Plot in Serial Plotter*
  int ecgValue = analogRead(ECG_PIN);
  Serial.println(ecgValue); // Log ECG data to Serial Plotter

  // *Update OLED Every Second*
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    // *Display Values on Serial Monitor*
    Serial.print("Heart Rate: ");
    Serial.print((heartRate > 0) ? heartRate : 0);
    Serial.print(" BPM | SpO2: ");
    Serial.print((spO2 > 0) ? spO2 : 0);
    Serial.print(" % | Temp: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // *Update OLED Display*
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Heart Rate: ");
    if (heartRate > 0) {
      u8g2.print(heartRate);
      u8g2.print(" BPM");
    } else {
      u8g2.print("Invalid");
    }

    u8g2.setCursor(0, 40);
    u8g2.print("SpO2: ");
    if (spO2 > 0) {
      u8g2.print(spO2);
      u8g2.print(" %");
    } else {
      u8g2.print("Invalid");
    }

    u8g2.setCursor(0, 60);
    u8g2.print("Temp: ");
    u8g2.print(temperatureC);
    u8g2.print(" C");

    u8g2.sendBuffer();
  }

  delay(10);  // Small delay for stability
}
