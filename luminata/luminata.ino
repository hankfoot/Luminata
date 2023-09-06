#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
void setup() {
  SerialBT.begin("ESP32 Bluetooth");
}
void loop() {
  SerialBT.println("Hello World");
  delay(1000);
}
