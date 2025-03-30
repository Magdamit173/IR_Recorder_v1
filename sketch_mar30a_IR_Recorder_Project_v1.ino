#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for (;;); // Halt execution if display fails
  }

  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("ECE Student");
  display.display();

  Serial.println("Setup complete!");
}

int getClosestIndex(float referenceValues[], int size, float target) {
  int closestIndex = 0;
  for (int i = 1; i < size; i++) {
    if (abs(referenceValues[i] - target) < abs(referenceValues[closestIndex] - target)) {
      closestIndex = i;
    }
  }
  return closestIndex;
}

int getFirstValidIndex(int values[], int size) {
  for (int i = 0; i < size; i++) {
    if (values[i] >= 0) { 
      return i;
    }
  }
  return -1; 
}

void loop() {
  float voltageThresholds[4] = {3.7, 2.68, 1.78, 0.92}; 
  int thresholdCount = sizeof(voltageThresholds) / sizeof(voltageThresholds[0]);

  float voltageA0 = analogRead(A0) * (5.0 / 1023.0);
  float voltageA1 = analogRead(A1) * (5.0 / 1023.0);
  float voltageA2 = analogRead(A2) * (5.0 / 1023.0);
  float voltageA3 = analogRead(A3) * (5.0 / 1023.0);

  int detectedColumn[4] = {
    voltageA0 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA0),
    voltageA1 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA1),
    voltageA2 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA2),
    voltageA3 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA3)
  };

  int activeRow = getFirstValidIndex(detectedColumn, 4);

  String keypadMatrix[4][4] = {
    {"M", "N", "O", "P"},
    {"I", "J", "K", "L"},
    {"E", "F", "G", "H"},
    {"A", "B", "C", "D"}
  };

  if (activeRow != -1 && detectedColumn[activeRow] != -1) {
    String pressedKey = keypadMatrix[activeRow][detectedColumn[activeRow]];
    
    Serial.println(pressedKey);

    // Update OLED display with pressed key
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print(pressedKey);
    display.display();
  }

  delay(100);
}
