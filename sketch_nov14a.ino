#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

float balance = 100.0;   // Initial balance in Rupees
float fare = 25.0;       // Fare amount to deduct per scan in Rupees
bool isEntry = true;     // Flag to track entry (true) or exit (false)

void setup() {
  Serial.begin(9600);
  SPI.begin();              // Init SPI bus
  mfrc522.PCD_Init();       // Init MFRC522
  lcd.init();
  lcd.backlight();
  
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Check if it's an entry scan
  if (isEntry) {
    // Entry scan - display welcome message
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("WELCOME");
    tone(BUZZER_PIN, 1000, 200); // Single beep for welcome
    delay(200);

    isEntry = false;  // Set to false so the next scan will be an exit
  } 
  else { 
    // Exit scan - check balance and attempt to deduct fare
    if (balance >= fare) {
      // Sufficient balance: deduct fare
      balance -= fare;

      // Successful payment message
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Thank You!");
      lcd.setCursor(0, 1);
      lcd.print("Balance: Rs ");
      lcd.print(balance, 2);  // Display remaining balance in Rupees

      // Delay to display balance before showing "Thank you for the ride"
      delay(2000);
      
      // Additional message
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

      // Three beeps for failure
      for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 1000, 200); // Beep at 1kHz for 200ms
        delay(300); // Delay between beeps
      }
    }
  }

  // Delay to allow message to be read
  delay(2000);

  // Reset display after showing message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  // Halt PICC to stop reading the same card multiple times
  mfrc522.PICC_HaltA();
}
