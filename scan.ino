#include <SPI.h>
#include <MFRC522.h>

// Pin Definitions for ESP32 WROOM-DA
#define SS_PIN 14    // Slave Select pin for RC522
#define RST_PIN 13   // Reset pin for RC522
#define SCK_PIN 27   // SCK pin for SPI
#define MOSI_PIN 26  // MOSI pin for SPI
#define MISO_PIN 12  // MISO pin for SPI

MFRC522 rfid(SS_PIN, RST_PIN);  // Create an instance of the RC522 class

void setup() {
  Serial.begin(115200);  // Initialize serial communications
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);  // Initialize SPI bus with ESP32 pins
  rfid.PCD_Init();  // Initialize RC522 module
  Serial.println("Place your NFC tag near the reader...");
}

void loop() {
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
}
