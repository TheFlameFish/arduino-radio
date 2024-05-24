#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_LEDBackpack.h>

Adafruit_7segment matrix = Adafruit_7segment();

String customStr;
String customStrBackup;

boolean customStrModified = false;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  matrix.begin(0x70);
  Serial.begin(9600);

  pinMode(2, INPUT); // C4 (BOOM)
  pinMode(3, INPUT); // C3
  pinMode(4, INPUT); // C2
  pinMode(5, INPUT); // C1

  pinMode(6, INPUT); // R4
  pinMode(7, INPUT); // R3
  pinMode(8, INPUT); // R2
  pinMode(9, INPUT); // R1
}

void updateMatrix() {
  if (customStr.length() <= 4) {
    matrix.println(customStr);
  } else {
    scroll(customStr);
  }
  matrix.writeDisplay();
}

void scroll(String text) {
  static int scrollIndex = 0;
  int textLength = text.length();
  int displayLength = 4; // Number of characters the display can show at a time

  String segment = text.substring(scrollIndex, scrollIndex + displayLength);
  matrix.println(segment);
  matrix.writeDisplay();

  scrollIndex++;
  if (scrollIndex > textLength - displayLength) {
    scrollIndex = 0; // Reset scroll index when it reaches the end of the text
  }
}

void loop() {
  static unsigned long lastScrollTime = 0;

  char customKey = customKeypad.getKey();

  if (customKey) {
    customStrModified = true;
    if (customKey == '*') {
      customKey = '.';
      customStr += customKey;
    } else if (customKey == '#') {
      customStr = ""; // Clear the string
      Serial.println("Cleared");
    } else {
      customStr += customKey;
    }

    Serial.println(customStr + "FM");
    lastScrollTime = millis(); // Reset last scroll time
    updateMatrix();
  }

  if (customStr.length() > 4) {
    unsigned long currentTime = millis();
    if (currentTime - lastScrollTime >= 500) { // Update every 500ms
      lastScrollTime = currentTime;
      updateMatrix();
    }
  }
  
  Serial.println(customStrBackup);

  if ((!customStrModified) && customStr != customStrBackup) {
    Serial.println("CustomStr compromised!");
    if (customStrBackup != "") {
      customStr = customStrBackup;
    } 
  }
  customStrBackup = customStr;
  customStrModified = false;
}
