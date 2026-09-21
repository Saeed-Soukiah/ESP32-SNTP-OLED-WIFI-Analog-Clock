# ESP32 SH1106 OLED Analog Clock

An internet-synchronized analog clock built for ESP32 and a 128x64 SH1106 I2C OLED display. It utilizes Network Time Protocol (SNTP) to fetch local time via Wi-Fi and renders a smooth, custom-drawn analog clock face complete with hour, minute, and second indicators.

## Features

* **NTP Time Synchronization:** Automatically updates local time using `pool.ntp.org` and `time.nist.gov`.

* **Non-Blocking Architecture:** Features background Wi-Fi reconnect attempts every 30 seconds without freezing display updates.

* **Custom Trigonometric Rendering:** Pre-calculated lookup tables for clock ticks to ensure smooth rendering and optimized CPU usage.

* **Wi-Fi Connection Indicator:** Small pixel indicator on the display shows real-time network connection status.

* **Fallback Mode:** Continues running using local RTC time during Wi-Fi disconnects or startup timeouts.

## Hardware Requirements

* **Microcontroller:** ESP32 (NodeMCU-32S, ESP32 Dev Module, or equivalent)

* **Display:** 128x64 SH1106 OLED Display (I2C)

* **Connecting Wires & Breadboard**

## Pin Connections

Connect the SH1106 OLED display to the default ESP32 I2C pins as follows:

| **SH1106 Pin** | **ESP32 Pin** | **Description** | 
| **VCC** | `3V3` or `5V` | Power (check display rating) | 
| **GND** | `GND` | Ground | 
| **SDA** | `GPIO 21` | I2C Data Line | 
| **SCL** | `GPIO 22` | I2C Clock Line | 

## Software Dependencies

Ensure the following libraries are installed in your Arduino IDE via the Library Manager (`Sketch` -> `Include Library` -> `Manage Libraries...`):

1. **Adafruit GFX Library** (`Adafruit_GFX.h`)

2. **Adafruit SH110X Library** (`Adafruit_SH110X.h`)

3. **Wire** (built-in)

4. **WiFi** (built-in for ESP32)

## Setup & Configuration

1. **Clone the Repository:**

   ```
   git clone https://github.com/YOUR_USERNAME/ESP32-SH1106-Analog-Clock.git
   
   ```

2. **Configure Wi-Fi Credentials:**
   Open the sketch file and set your Wi-Fi details:

   ```
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   
   ```

3. **Configure Timezone Offset:**
   Adjust `gmtOffset_sec` for your target timezone (specified in seconds relative to UTC):

   ```
   // Example: UTC+3 (Damascus / Riyadh)
   const long gmtOffset_sec = 10800; // 3 hours * 3600 seconds
   
   // Daylight Saving Time offset (in seconds)
   const int daylightOffset_sec = 0;
   
   ```

4. **Upload:** Select your ESP32 board and serial port in Arduino IDE, then click **Upload**.

## License

This project is open-source and available under the [MIT License](LICENSE).
