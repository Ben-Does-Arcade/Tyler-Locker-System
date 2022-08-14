#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

const byte numberOfLockers = 6;
const String cardNumbers[] = {"1791678928", "", "", "", "", ""};

const byte locker[] = {22, 3, 4, 5, 6, 7};
const byte piezo = 10;
const byte lockerClosed = HIGH;
const byte lockerOpened = LOW;

byte cardNumber[4];

LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 mfrc522(9, 8);

void printMessage(String line1, String line2, String line3, String line4) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcd.setCursor(0, 2);
  lcd.print(line3);
  lcd.setCursor(0, 3);
  lcd.print(line4);
}

void setup() {
  Serial.begin(9600);
  
  for (byte i = 0; i < numberOfLockers; i++) {
    Serial.println("Configuring " + String(i) + " on pin " + String(locker[i]) + "...");
    pinMode(locker[i], OUTPUT);
    digitalWrite(locker[i], lockerClosed);
  }

  pinMode(piezo, OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  String compiledCardNumber = "";
  
  printMessage("WELCOME PLAYER!", "", "WIN KEY FOB TO", "UNLOCK A PRIZE!");

  Serial.println("Waiting for rfid...");
  
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
  }
  
  Serial.print(F("Card "));
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    compiledCardNumber += String(mfrc522.uid.uidByte[i]);
  }
  
  Serial.println(" scanned.");

  printMessage("", "PLEASE WAIT", "READING TAG...", "");

  digitalWrite(piezo, HIGH);
  delay(300);
  digitalWrite(piezo, LOW);

  for (byte i = 0; i < numberOfLockers; i++) {
    Serial.println(String(cardNumbers[i]));
    Serial.println(compiledCardNumber);
    if (String(cardNumbers[i]) == compiledCardNumber) {
      byte lockerNum = i;
  
      Serial.println(lockerNum);
      Serial.println(locker[lockerNum]);
  
      printMessage("PRIZE " + String(lockerNum + 1) + " UNLOCKED!", "OPEN DOOR NOW!", "", "");
  
      digitalWrite(locker[i], lockerOpened);
      digitalWrite(piezo, HIGH);
      delay(1000);
      digitalWrite(piezo, LOW);
      delay(20000);
      digitalWrite(locker[i], lockerClosed);

      return;
    }
  }
  
  printMessage("SORRY!", "NOT A WINNER!", "", "ID: " + compiledCardNumber);
  delay(4000);
}
