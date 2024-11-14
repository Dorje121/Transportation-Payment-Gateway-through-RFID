#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);  
LiquidCrystal_I2C lcd(0x27, 16, 2); 

float balance = 100.0;   
float fare = 25.0;      
bool isEntry = true;     

void setup() {
  Serial.begin(9600);
  SPI.begin();              
  mfrc522.PCD_Init();      
  lcd.init();
  lcd.backlight();
  
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }


  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

 
  if (isEntry) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("WELCOME");
    tone(BUZZER_PIN, 1000, 200); 
    delay(200);

    isEntry = false;  
  } 
  else { 
   
    if (balance >= fare) {
      // Sufficient balance: deduct fare
      balance -= fare;

      // Successful payment message
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Thank You!");
      lcd.setCursor(0, 1);
      lcd.print("Balance: Rs ");
      lcd.print(balance, 2); 

      // Delay to display balance before showing "Thank you for the ride"
      delay(2000);
      
    
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Thank you for");
      lcd.setCursor(4, 1);
      lcd.print("the ride!");

      // Single beep for success
      tone(BUZZER_PIN, 1000, 200); // Beep at 1kHz for 200ms
      delay(200);

      // Reset for next entry scan
      isEntry = true;
    } else {
      // Insufficient balance: display error message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Insufficient");
      lcd.setCursor(0, 1);
      lcd.print("Balance!");

      for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 1000, 200); 
        delay(300); 
      }
    }
  }

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
    mfrc522.PICC_HaltA();
}


  // Halt PICC to stop reading the same card multiple times
  mfrc522.PICC_HaltA();
}
