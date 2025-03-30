#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

// IR
#define IR_PIN 2

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(9600);
  Wire.begin();  // Ensure I2C communication starts

  // Initialize IR
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready...");

  // Initialize OLED display
  Serial.println("Initializing OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Change to 0x3D if needed
    Serial.println("SSD1306 allocation failed! Check wiring and I2C address.");
    while (true); // Stop execution if display fails
  }

  Serial.println("OLED initialized.");
  display.clearDisplay();
  display.setTextSize(1);
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

  // Voltage Reading / Matrix 4x4
  float voltageThresholds[4] = {3.61, 2.61, 1.74, 0.89}; 
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

  const char* keypadMatrix[4][4] = {
    {"1!", "2!", "3!", "Power On/Off"},
    {"4!", "5!", "6!", "IR Record"},
    {"7!", "8!", "9!", "Switch"},
    {"*", "0!", "#", "Name Matrix"}
  };

  //IR Receiver
  // if (IrReceiver.decode()) {
  //   Serial.print("IR Signal Received: ");
  //   Serial.println(IrReceiver.decodedIRData.command, HEX);

  //   display.clearDisplay();
  //   display.setCursor(10, 10);
  //   display.print("IR Cmd: ");
  //   display.print(IrReceiver.decodedIRData.command, HEX);
  //   display.display();

  //   IrReceiver.resume();
  // }

  // Keypad Handling
  if (activeRow != -1 && detectedColumn[activeRow] != -1) {
    const char* pressedKey = keypadMatrix[activeRow][detectedColumn[activeRow]];

    Serial.print("Key Pressed: ");
    Serial.println(pressedKey);

    // Update OLED display with pressed key
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print(pressedKey);
    display.display();
  }

  delay(100);
}
