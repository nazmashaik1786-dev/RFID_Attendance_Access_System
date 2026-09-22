#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Replace this UID with your authorized RFID card UID.
byte authorizedUID[] = {0xDE, 0xAD, 0xBE, 0xEF};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");

  Serial.println("RFID Attendance and Access System");
  Serial.println("Scan an RFID card...");
}

bool isAuthorized() {
  if (rfid.uid.size != 4) return false;

  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != authorizedUID[i]) {
      return false;
    }
  }

  return true;
}

void printUID() {
  Serial.print("Card UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1) {
      Serial.print(" ");
    }
  }

  Serial.println();
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  printUID();

  lcd.clear();

  if (isAuthorized()) {

    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");

    lcd.setCursor(0, 1);
    lcd.print("Attendance Marked");

    Serial.println("Access Granted");
    Serial.println("Attendance Marked");

    tone(BUZZER_PIN, 1000, 200);

  } else {

    lcd.setCursor(0, 0);
    lcd.print("ACCESS DENIED");

    lcd.setCursor(0, 1);
    lcd.print("Unknown Card");

    Serial.println("Access Denied");
    Serial.println("Unknown Card");

    tone(BUZZER_PIN, 400, 500);
  }

  delay(2000);

  lcd.clear();
  lcd.print("RFID Attendance");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}