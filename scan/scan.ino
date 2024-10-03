#include <SPI.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <MFRC522.h>

// Pin Definitions for ESP32 WROOM-DA
#define SS_PIN 14    // Slave Select pin for RC522
#define RST_PIN 13   // Reset pin for RC522
#define SCK_PIN 27   // SCK pin for SPI
#define MOSI_PIN 26  // MOSI pin for SPI
#define MISO_PIN 12  // MISO pin for SPI

const char* ssid = "#";
const char* password = "#";

MFRC522 rfid(SS_PIN, RST_PIN);  // Create an instance of the RC522 class

void setup() {
  Serial.begin(115200);  // Initialize serial communications
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // Initialize SPI bus with ESP32 pins
  rfid.PCD_Init();  // Initialize RC522 module
  Serial.println("Ready to read NFC tag...");

  // Initialize Wi-Fi in STA (station) mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait until the ESP32 is connected to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Retrieve and print Wi-Fi configuration using esp_wifi.h functions
  wifi_config_t wifi_config;
  esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
  Serial.printf("SSID: %s\n", wifi_config.sta.ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // Check if wifi drops
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi lost. Attempting to reconnect...");

        // Reconnect to Wi-Fi
        WiFi.disconnect();  // Ensure any previous connection is reset
        WiFi.begin(ssid, password); // Start reconnecting

        // Wait until the connection is re-established
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Reconnecting...");
        }

        Serial.println("Reconnected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }

  // Check if a new card is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("Tag UID: ");
    
    // Print each byte of the UID (Unique Identifier)
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    
    rfid.PICC_HaltA();  // Halt the current card to stop further readings
  }

  delay(1000);  // Delay for a second before attempting to read the card again

  // if 10 seconds have passed send  a message to tell the the device is still alive
  static unsigned long lastAlive = 0;
  if (millis() - lastAlive > 10000) {
    lastAlive = millis();
    Serial.println("Still alive :)");
  }
}
