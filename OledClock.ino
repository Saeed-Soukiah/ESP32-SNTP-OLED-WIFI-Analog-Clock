#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include "time.h"
#include "esp_sntp.h"

// Configuration
const char* ssid = "Your Wifi SSID";
const char* password = "Your Wifi Password";

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 10800;  // UTC+3 (Damascus)
const int daylightOffset_sec = 0;   // No DST

// Global Variables
int hrs = 0, mins = 0, secs = 0;
unsigned long lastClockUpdate = 0;
unsigned long lastWiFiCheck = 0;

// Initialize SH1106 display (128x64)
Adafruit_SH1106G display(128, 64, &Wire, -1);

const int NUM_POINTS = 60;
const int RADIUS = 28;
const int CENTER_X = 64;
const int CENTER_Y = 32;

int pointsX[NUM_POINTS];
int pointsY[NUM_POINTS];

void setLocalTime();
void timeavailable(struct timeval* t);

void setup() {
  Serial.begin(115200);

  // Initialize SH1106 Display (0x3C is standard I2C address)
  delay(100); // Small delay for SH1106 startup stability
  if (!display.begin(0x3C, true)) {
    Serial.println(F("SH1106 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 28);
  display.print("Connecting WiFi...");
  display.display();

  // Initialize SNTP configuration
  sntp_set_time_sync_notification_cb(timeavailable);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Non-blocking WiFi connection with timeout
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
  } else {
    Serial.println("\nWiFi Connection Timeout (Running on local RTC)");
  }

  // Pre-calculate 60 ticks around the clock face
  for (int i = 0; i < NUM_POINTS; i++) {
    float angleRad = (i * 6.0f - 90.0f) * DEG_TO_RAD;
    pointsX[i] = CENTER_X + RADIUS * cos(angleRad);
    pointsY[i] = CENTER_Y + RADIUS * sin(angleRad);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Update screen once every second
  if (currentMillis - lastClockUpdate >= 1000) {
    lastClockUpdate = currentMillis;

    setLocalTime();
    drawClock();
  }

  // Non-blocking WiFi reconnect check every 30 seconds
  if (currentMillis - lastWiFiCheck >= 30000) {
    lastWiFiCheck = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
    }
  }
}

void drawClock() {
  display.clearDisplay();

  // 1. Calculate angles
  float secAngle = secs * 6.0f;
  float minAngle = (mins + secs / 60.0f) * 6.0f;
  float hrAngle  = ((hrs % 12) + mins / 60.0f) * 30.0f;

  // 2. Convert angles to polar coordinates
  float hrRad  = (hrAngle - 90.0f) * DEG_TO_RAD;
  float minRad = (minAngle - 90.0f) * DEG_TO_RAD;
  float secRad = (secAngle - 90.0f) * DEG_TO_RAD;

  int hrX  = CENTER_X + (RADIUS - 12) * cos(hrRad);
  int hrY  = CENTER_Y + (RADIUS - 12) * sin(hrRad);
  int minX = CENTER_X + (RADIUS - 5)  * cos(minRad);
  int minY = CENTER_Y + (RADIUS - 5)  * sin(minRad);
  int secX = CENTER_X + (RADIUS)      * cos(secRad);
  int secY = CENTER_Y + (RADIUS)      * sin(secRad);

  // 3. Draw clock face and ticks
  display.drawCircle(CENTER_X, CENTER_Y, RADIUS + 3, SH110X_WHITE);
  for (int i = 0; i < NUM_POINTS; i += 5) {
    display.fillCircle(pointsX[i], pointsY[i], 1, SH110X_WHITE);
  }

  // 4. Draw cardinal numbers
  display.setTextSize(1);
  display.setCursor(59, 7);   display.print("12");
  display.setCursor(88, 29);  display.print("3");
  display.setCursor(61, 50);  display.print("6");
  display.setCursor(34, 29);  display.print("9");

  // 5. Draw hands
  display.drawLine(CENTER_X, CENTER_Y, hrX, hrY, SH110X_WHITE);
  display.drawLine(CENTER_X, CENTER_Y, minX, minY, SH110X_WHITE);
  display.drawCircle(secX, secY, 2, SH110X_WHITE);

  // 6. Wi-Fi status indicator dot (top-left)
  if (WiFi.status() == WL_CONNECTED) {
    display.drawPixel(0, 0, SH110X_WHITE);
  }

  display.display();
}

void setLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  hrs  = timeinfo.tm_hour;
  mins = timeinfo.tm_min;
  secs = timeinfo.tm_sec;
}

void timeavailable(struct timeval* t) {
  Serial.println("NTP time synchronization complete.");
  setLocalTime();
}
