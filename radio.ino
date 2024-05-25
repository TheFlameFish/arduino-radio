#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_LEDBackpack.h>

Adafruit_7segment matrix = Adafruit_7segment();

char customStr[17] = "";
char customStrBackup[17] = "";

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
  Serial.println("Radio booting up.");

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
  if (strlen(customStr) <= 4) {
    matrix.println(customStr);
  } else {
    scroll(customStr);
  }
  matrix.writeDisplay();
}

void scroll(char* text) {
  static int scrollIndex = 0;
  int textLength = strlen(text);
  int displayLength = 4; // Number of characters the display can show at a time

  char segment[5];
  strncpy(segment, text + scrollIndex, displayLength);
  segment[displayLength] = '\0'; // Null-terminate the string
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
      strncat(customStr, &customKey, 1);
    } else if (customKey == '#') {
      customStr[0] = '\0'; // Clear the string
      Serial.println("Cleared");
    } else {
      strncat(customStr, &customKey, 1);
    }

    Serial.print(customStr);
    Serial.println("FM");
    lastScrollTime = millis(); // Reset last scroll time
    updateMatrix();
  }

  if (strlen(customStr) > 4) {
    unsigned long currentTime = millis();
    if (currentTime - lastScrollTime >= 500) { // Update every 500ms
      lastScrollTime = currentTime;
      updateMatrix();
    }
  }
  
  if ((!customStrModified) && strcmp(customStr, customStrBackup) != 0) {
    Serial.println("CustomStr compromised!");
    if (customStrBackup[0] != '\0') {
      strncpy(customStr, customStrBackup, sizeof(customStr) - 1);
      customStr[sizeof(customStr) - 1] = '\0'; // Ensure null termination
    }
  }
  strncpy(customStrBackup, customStr, sizeof(customStrBackup) - 1);
  customStrBackup[sizeof(customStrBackup) - 1] = '\0'; // Ensure null termination
  customStrModified = false;

  delay(100);
}

int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
