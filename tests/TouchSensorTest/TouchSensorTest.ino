/*  TouchSensorTest.ino
    December 2020
    Christopher Datsikas

    Use this to test your touch sensor. Code based in part from:
    ArduinoGetStarted.com
    https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press

   Hardware
    - NodeMCU
    - AT42QT1070 Capacitive Touch

*/

// Pin assignments
const int buttonPin = 12;    // D6 NodeMCU - the number of the input pin from the large button
const int primaryLedPin = 16;             // the number of the output pin - onboard LED for debugging
const int secondaryLedPin = 2;             // the number of the output pin - onboard LED for debugging

// Variables for LED State
boolean primaryLedState = HIGH; // if nodeMCU LED is on or off
boolean secondaryLedState = HIGH; // if ESP8266 LED is on or off

// Variables for touch sensor tracking
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
const long DEBOUNCE = 20;   // the debounce time, increase if the output flickers
const long LONG_PRESS_TIME = 6000;

void setup() {
  // Debug console
  Serial.begin(9600);

  // initialize capacitive touch buttons
  pinMode(buttonPin, INPUT);

  // initialize NodeMCU debug onboard LEDs
  primaryLedState = HIGH;
  pinMode(primaryLedPin, OUTPUT);      // Onboard LED is active LOW
  digitalWrite(primaryLedPin, primaryLedState);
  Serial.print("Primary Led State: "); Serial.println(primaryLedState);

  // initilize ESP Debug onboard LED
  secondaryLedState = HIGH;
  pinMode(secondaryLedPin, OUTPUT);      // Onboard LED is active LOW
  digitalWrite(secondaryLedPin, secondaryLedState);
  Serial.print("Secondary Led State: "); Serial.println(secondaryLedState);

  delay(5000);

  Serial.println("Setup Complete");
}

void loop() {

  // continuously polls the capacitive sensor - active low
  currentState = digitalRead(buttonPin);
  if (lastState == HIGH && currentState == LOW) {       // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if (lastState == LOW && currentState == HIGH) { // button is released
    isPressing = false;
    releasedTime = millis();
    Serial.println(pressedTime); Serial.println(releasedTime); Serial.println(LONG_PRESS_TIME);

    // button is released within certain duration range
    long pressDuration = releasedTime - pressedTime;
    if (pressDuration > DEBOUNCE && pressDuration < LONG_PRESS_TIME) {
      Serial.println("A short press is detected");
      toggleSecondaryLed();
    }
  }

  // button has been pressed for a long time
  if (isPressing == true && isLongDetected == false) {
    if (millis() - pressedTime >= LONG_PRESS_TIME) {
      Serial.println("A long press is detected");
      togglePrimaryLed();
      isLongDetected = true;
    }
  }

  // save the the last state
  lastState = currentState;
}


void togglePrimaryLed() {
  Serial.println("Toggle Primary Led");
  Serial.print("Primary Led State Before: ");
  Serial.println(primaryLedState);
  primaryLedState = !primaryLedState;
  digitalWrite(primaryLedPin, primaryLedState);
  Serial.print("Primary Led State After: ");
  Serial.println(primaryLedState);
}

void toggleSecondaryLed() {
  Serial.println("Toggle Secondary Led");
  Serial.print("Secondary Led State Before: ");
  Serial.println(secondaryLedState);
  secondaryLedState = !secondaryLedState;
  digitalWrite(secondaryLedPin, secondaryLedState);
  Serial.print("Secondary Led State After: ");
  Serial.println(secondaryLedState);
}
