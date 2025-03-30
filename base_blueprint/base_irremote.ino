// #include <IRremote.h>

// int IR_PIN = 2;  // Connect the IR receiver's signal pin to D2

// void setup() {
//   Serial.begin(9600);
//   IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK); // Start IR receiver
//   Serial.println("IR Receiver Ready...");
// }

// void loop() {
//   if (IrReceiver.decode()) {
//     Serial.print("Received IR Signal: ");
//     Serial.println(IrReceiver.decodedIRData.command, HEX); // Print hex value
//     IrReceiver.resume();  // Receive the next signal
//   }
// }
