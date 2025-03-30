#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

// IR
#define IR_RECEIVE_PIN PB0
#define IR_SEND_PIN PB1
#define RAW_BUFFER_SIZE 100

uint16_t rawIRData[RAW_BUFFER_SIZE];  // Array to store raw IR signals
uint8_t rawIRLength = 0;              // Length of recorded IR signal

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize IR Receiver & Transmitter
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);
  Serial.println("IR Receiver & Transmitter Ready...");

  // Initialize OLED display
  Serial.println("Initializing OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Change to 0x3D if needed
    Serial.println("SSD1306 allocation failed! Check wiring and I2C address.");
    while (true)
      ;  // Stop execution if display fails
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
  float voltageThresholds[4] = { 2.36, 1.72, 1.13, 0.49 };  // Adjusted for 3.3V reference
  int thresholdCount = sizeof(voltageThresholds) / sizeof(voltageThresholds[0]);

  float voltageA0 = analogRead(PA0) * (3.3 / 4095.0);  // Adjusted for STM32 12-bit ADC
  float voltageA1 = analogRead(PA1) * (3.3 / 4095.0);
  float voltageA2 = analogRead(PA2) * (3.3 / 4095.0);
  float voltageA3 = analogRead(PA3) * (3.3 / 4095.0);

  // Serial.println(voltageA0);
  // Serial.println(voltageA1);
  // Serial.println(voltageA2);
  // Serial.println(voltageA3);
  // Serial.println("------------------------");

  int detectedColumn[4] = {
    voltageA0 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA0),
    voltageA1 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA1),
    voltageA2 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA2),
    voltageA3 <= voltageThresholds[thresholdCount - 1] - 0.01 ? -1 : getClosestIndex(voltageThresholds, thresholdCount, voltageA3)
  };

  int activeRow = getFirstValidIndex(detectedColumn, 4);

  const char* keypadMatrix[4][4] = {
    { "1!", "2!", "3!", "Power On/Off" },
    { "4!", "5!", "6!", "IR Record" },
    { "7!", "8!", "9!", "Switch" },
    { "*", "0!", "#", "Name Matrix" }
  };


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


  // IR Receiver
  if (IrReceiver.decode()) {
    Serial.println("IR Signal Received!");
    rawIRLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;  // Get raw length

    if (rawIRLength > RAW_BUFFER_SIZE) rawIRLength = RAW_BUFFER_SIZE;  // Prevent overflow

    // Copy raw data
    for (uint8_t i = 0; i < rawIRLength; i++) {
      rawIRData[i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i + 1];
    }

    Serial.print("Recorded Raw Data Length: ");
    Serial.println(rawIRLength);
    Serial.println("Use 'C' command to send back.");

    // Update OLED
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print("IR Recorded!");
    display.display();

    IrReceiver.resume();  // Get ready for the next signal
  }

  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'C' && rawIRLength > 0) {
      Serial.println("Sending Stored Raw IR Signal...");
      IrSender.sendRaw(rawIRData, rawIRLength, 38);  // 38kHz IR frequency

      // Update OLED
      display.clearDisplay();
      display.setCursor(10, 10);
      display.print("IR Sent!");
      display.display();

      Serial.println("IR Signal Sent Successfully!");
    }
  }


  delay(100);
}
