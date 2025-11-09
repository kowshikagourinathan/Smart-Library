#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define IR_ENTRY 2
#define IR_EXIT 3
#define LDR A0
#define BUZZER 7

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int count = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  pinMode(IR_ENTRY, INPUT);
  pinMode(IR_EXIT, INPUT);
  pinMode(LDR, INPUT);
  pinMode(BUZZER, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print("Smart Library");
  delay(2000);
  lcd.clear();
}

void loop() {
  // RFID Authentication
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    lcd.clear();
    lcd.print("RFID: " + uid);
    Serial.println("RFID Card: " + uid);
    delay(1500);
    rfid.PICC_HaltA();
  }

  // Entry Detection
  if (digitalRead(IR_ENTRY) == LOW) {
    count++;
    lcd.clear();
    lcd.print("People In: ");
    lcd.print(count);
    delay(1000);
  }

  // Exit Detection
  if (digitalRead(IR_EXIT) == LOW && count > 0) {
    count--;
    lcd.clear();
    lcd.print("People In: ");
    lcd.print(count);
    delay(1000);
  }

  // LDR Light Control
  int lightValue = analogRead(LDR);
  if (lightValue < 400) {
    lcd.setCursor(0, 1);
    lcd.print("Lights ON ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Lights OFF");
  }

  // Unauthorized Access Alert
  if (digitalRead(IR_ENTRY) == LOW && !rfid.PICC_IsNewCardPresent()) {
    digitalWrite(BUZZER, HIGH);
    lcd.clear();
    lcd.print("Alert! No RFID");
    delay(2000);
    digitalWrite(BUZZER, LOW);
  }
}
