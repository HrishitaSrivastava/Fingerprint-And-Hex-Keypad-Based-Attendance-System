#include <Keypad.h>
#include <EEPROM.h>
#include <Adafruit_Fingerprint.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const byte ROWS = 4; // Number of rows
const byte COLS = 4; // Number of columns

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 25, 33, 32}; // Connect to the column pinouts of the keypad


Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define PASSWORD_LENGTH 6
#define PASSWORD_ADDRESS 0

#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUZZER_PIN 4


void setup() {
  Serial.begin(9600);
  EEPROM.begin(512); // Initialize EEPROM

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Welcome Student1");
  display.display();
  delay(1000);
  display.clearDisplay();

  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == '#') {
      // Enter password checking mode
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Enter password:");
      display.display();

      char enteredPassword[PASSWORD_LENGTH + 1];
      int index = 0;

      while (index < PASSWORD_LENGTH) {
        char key = keypad.getKey();

        if (key) {
          Serial.print("*");
          display.print("*");
          enteredPassword[index] = key;
          index++;
          delay(100); // Debounce delay
        }
      }

      enteredPassword[PASSWORD_LENGTH] = '\0'; // Null-terminate the password string

      if (checkPassword(enteredPassword)) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Correct password!");
        display.display();
        display.setCursor(0, 10);
        display.println("Access Granted!");
        activateBuzzer(1000); // Buzzer activation for 1 second
        delay(2000);
        // Perform action when the correct password is entered
      } else {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Incorrect password!");
        display.display();
        display.setCursor(0, 10);
        display.println("Access Denied!");
        activateBuzzer(1000); // Buzzer activation for 200 milliseconds
        delay(2000);
        // Perform action when an incorrect password is entered
      }

      display.clearDisplay();
      display.display();
    }
  }
}

bool checkPassword(const char* enteredPassword) {
  char storedPassword[PASSWORD_LENGTH + 1];

  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    storedPassword[i] = EEPROM.read(PASSWORD_ADDRESS + i);
  }

  storedPassword[PASSWORD_LENGTH] = '\0'; // Null-terminate the password string

  return strcmp(enteredPassword, storedPassword) == 0;
}

void activateBuzzer(unsigned int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}