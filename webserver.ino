#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ----- WiFi Credentials -----
const char* wifi_ssid = "ZTE_2.4G_6SDbuZ";
const char* wifi_pass = "Q3AC29Yd";

// ----- Web Server Instance -----
WebServer server(80);

// ----- Global Variables for Web Server Data -----
float gHeartRate = 0, gSpO2 = 0, gTemperatureC = 0;
int   gEcgValue = 0;

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

// --- Function Prototypes for Web Server ---
String SendHTML(float heartRate, float spO2, float temperatureC, int ecgValue);
void handle_OnConnect();

void setup() {
  Serial.begin(115200);
  Serial.println("Pulse, SpO2, ECG & Temperature Monitoring System");

  // ----- Connect to WiFi -----
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());

  // ----- Start the Web Server -----
  server.on("/", handle_OnConnect);
  server.begin();
  Serial.println("HTTP server started");

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
  server.handleClient();  // Process incoming HTTP requests
  pox.update();           // Update Pulse Oximeter readings
  sensors.requestTemperatures();
 
  // **Read Sensor Values**
  float heartRate = pox.getHeartRate();
  float spO2 = pox.getSpO2();
  float temperatureC = sensors.getTempCByIndex(0);

  // **Read ECG Sensor (for Serial Plotter)**
  int ecgValue = analogRead(ECG_PIN);
  Serial.println(ecgValue); // Printed only for Serial Plotter

  // Update global variables for web server use
  gHeartRate = heartRate;
  gSpO2 = spO2;
  gTemperatureC = temperatureC;
  gEcgValue = ecgValue;

  // **Update OLED and Serial Monitor Every Second**
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    // Print sensor values to Serial Monitor
    Serial.print("Heart Rate: ");
    Serial.print((heartRate > 0) ? heartRate : 0);
    Serial.print(" BPM | SpO2: ");
    Serial.print((spO2 > 0) ? spO2 : 0);
    Serial.print(" % | Temp: ");
    Serial.print(temperatureC);
    Serial.println(" °C");

    // Update OLED Display
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
  
  delay(10);
}

// ----- Web Server Handler for Root URL -----
// We add headers to disable caching.
void handle_OnConnect() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
  server.send(200, "text/html", SendHTML(gHeartRate, gSpO2, gTemperatureC, gEcgValue));
}

// ----- Function to Generate HTML Content -----
// This HTML includes a meta refresh tag to reload the page every second.
String SendHTML(float heartRate, float spO2, float temperatureC, int ecgValue) {
  String ptr = "<!DOCTYPE html>";
  ptr += "<html>";
  ptr += "<head>";
  ptr += "<title>ESP32 Patient Health Monitoring</title>";
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  // Refresh the page every 1 second
  ptr += "<meta http-equiv='refresh' content='1'>";
  ptr += "<style>";
  ptr += "html { font-family: Arial; text-align: center; color: #444; } ";
  ptr += "h1 { color: #444; } ";
  ptr += ".data { margin: 20px; padding: 10px; border: 1px solid #ccc; display: inline-block; }";
  ptr += "</style>";
  ptr += "</head>";
  ptr += "<body>";
  ptr += "<h1>ESP32 Patient Health Monitoring</h1>";
  ptr += "<div class='data'><h2>Heart Rate</h2><p>" + String(heartRate, 1) + " BPM</p></div>";
  ptr += "<div class='data'><h2>SpO2</h2><p>" + String(spO2, 1) + " %</p></div>";
  ptr += "<div class='data'><h2>Temperature</h2><p>" + String(temperatureC, 1) + " °C</p></div>";
  ptr += "<div class='data'><h2>ECG</h2><p>" + String(ecgValue) + "</p></div>";
  ptr += "</body></html>";
  return ptr;
}
