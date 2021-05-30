/*
  Input Pull-up Serial

  This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a digital
  input on pin 2 and prints the results to the Serial Monitor.

  The circuit:
  - momentary switch attached from pin 6 to ground
  - built-in LED on pin 13

  Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
  20K-ohm resistor is pulled to 5V. This configuration causes the input to read
  HIGH when the switch is open, and LOW when it is closed.

  created 14 Mar 2012
  by Scott Fitzgerald

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/InputPullupSerial

  modified 16 May 2021  
*/

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library

// Constant pin assignments
const int switchPin = 14;   //GPIO-14: Feather Huzzah ESP8266 Port Pin 14. NodeMCU Port Pin D5. 
const int ledPin = 2;       // GPIO-16 Node MCU LED - the number of the output pin - onboard LED for debugging
                            // GPIO-02 ESP-12 LED

// Variable assignments                         
boolean powerState = HIGH;  // if device is on or off
int previousSwitchState = 0;
int currentSwitchState = 0;
unsigned long switchTime = 0;
const long DEBOUNCE = 20;           // the debounce time, increase if the output flickers

void setup() {
  //start serial connection
  Serial.begin(115200); // Set serial to 115200 to match lucon.ino
  
  //configure switchPin as an input and enable the internal pull-up resistor
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

}

void loop() {
  //read the switch value into a variable
  int currentSwitchState = digitalRead(switchPin);
  //print out the value of the switch
  Serial.println(currentSwitchState);

  // if the switch input has changed and we've waited long enough
  if (currentSwitchState != previousSwitchState && millis() - switchTime > DEBOUNCE) {
    Serial.print("Previous powerState "); Serial.println(powerState);
    powerState = !powerState;
    Serial.print("New powerState "); Serial.println(powerState);
    previousSwitchState = currentSwitchState;
    switchTime = millis();
  }

  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (powerState == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
  delay(1);
}
