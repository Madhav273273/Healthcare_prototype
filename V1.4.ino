#define BLYNK_TEMPLATE_ID "TMPL3gTF4jgGw"
#define BLYNK_TEMPLATE_NAME "iot healthcare"
#define BLYNK_AUTH_TOKEN "-g9jjSC1nMbqwqZyfZ_XTWYh7nOwHORj"

#include <WiFi.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32.h>

// **Wi-Fi Credentials**
const char* ssid = "Madhav";     
const char* password = "appa1234";    

// **I2C OLED Display (128x64, I2C)**
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 22, 21);

// **MAX30100 Pulse Oximeter Instance**
PulseOximeter pox;

// **Temperature Sensor (DS18B20) Pin**
#define TEMP_SENSOR_PIN 4  
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

// **ECG Sensor Pin**
#define ECG_PIN 34  

// **Time Tracking Variables**
uint32_t tsLastReport = 0;
#define REPORTING_PERIOD_MS 1000  

// **Callback for Heartbeat Detection**
void onBeatDetected() {
  Serial.println("Beat detected!");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Pulse, SpO2, ECG & Temperature Monitoring System");

  // **Initialize Wi-Fi & Blynk**
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi Connected!");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  Serial.println("Blynk Connected!");

  // **Initialize OLED**
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  Serial.println("OLED Initialized");

  // **Initialize DS18B20 Temperature Sensor**
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, 12);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();

  // **Initialize MAX30100 Pulse Oximeter**
  Serial.print("Initializing MAX30100...");
  if (!pox.begin()) {
    Serial.println("FAILED. Check wiring.");
    while (1);
  }
  Serial.println("SUCCESS");
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  pox.update();  // Update Pulse Oximeter Readings
  sensors.requestTemperatures();

  // **Read Sensor Values**
  float heartRate = pox.getHeartRate();
  float spO2 = pox.getSpO2();
  float temperatureC = sensors.getTempCByIndex(0);

  // **Read ECG Sensor & Plot in Serial Plotter (But NOT in Serial Monitor)**
  int ecgValue = analogRead(ECG_PIN);
  Serial.println(ecgValue); // ONLY used for Serial Plotter

  // **Introduce 1s Delay After Reading MAX30100**
  delay(1000);

  // **Update OLED & Send Data Every Second**
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    // **Display on Serial Monitor**
    Serial.print("Heart Rate: ");
    Serial.print((heartRate > 0) ? heartRate : 0);
    Serial.print(" BPM | SpO2: ");
    Serial.print((spO2 > 0) ? spO2 : 0);
    Serial.print(" % | Temp: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // **Update OLED Display**
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

    // **Send Data to Blynk**
    Blynk.virtualWrite(V0, heartRate);  // BPM
    Blynk.virtualWrite(V1, spO2);       // SpO2
    Blynk.virtualWrite(V2, temperatureC);  // Temperature

    // **Introduce 1s Delay After Sending Data to WiFi**
    delay(1000);
  }

  delay(10);
}



