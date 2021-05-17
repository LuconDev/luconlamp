/* Lucon.ino
  January 2021
  Christopher Datsikas

  Hardware
  - NodeMCU or Adafruit ESP8266 Huzzah https://www.adafruit.com/product/3046
  - RGBW Neopixel https://www.adafruit.com/product/2832
  - TTP223B Capacitive Touch

  References
  1. Button Control - ArduinoGetStarted.com - Public Domain
  https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press
  2. Neopixel Library 1.7.0 - Adafruit - GNU Lesser General Public License v3.0
  https://github.com/adafruit/Adafruit_NeoPixel
  3A. pubsubclient library (MQTT and Wifi Client) 2.8.0 - MIT
  https://github.com/knolleary/pubsubclient
  3B. Cloud MQTT Setup - Varun Sonava
  https://medium.com/dataseries/smart-led-iot-diy-698512b26730
  4. ESP8266 Arduino Wifi Library - GNU Lesser General Public License v2.1
  https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/readme.rst
  5. Wifi Manager Library 0.16.0 (Wifi AP & Captive Portal) - tzapu - MIT
  https://github.com/tzapu/WiFiManager
  6. Digital Input Pullup for Switch
  https://www.arduino.cc/en/Tutorial/DigitalInputPullup
*/

// Include relevant libraries 
#include <Adafruit_NeoPixel.h>    // Neopixel Control
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library
#include <PubSubClient.h>         // MQTT Communication
#include "credentials.h"          // store Wifi and MQTT credentials locally
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

// in credentials.h
//const char *device_id = "";
//char *ssid = "";
//char *password = "";
//char *mqtt_server = "";
//char *mqtt_port = "";
//char *mqtt_username = "";
//const char *sub_topic_name = "";

// Neopixel constants
#define NEOPIXEL_PIN 5    // GPIO-05 - D5 Adafruit Feather Huzzah / D1 NodeMCU
#define NUMPIXEL_COUNT 4       // Number of LEDs in strip
#define MID_BRIGHTNESS 30
#define MAX_BRIGHTNESS 254
#define PULSE_SLOW 10
#define PULSE_MEDIUM 5
#define PULSE_FAST 1
#define DELAYVAL 5

// Pin Assignments //todo, replace with all caps locks
const int touchPin = 12;    // GPIO-12 - D12 Adafruit Feature  Huzzah / D6 NodeMCU - touch sensor pin
const int ledPin = 2;       // GPIO-16 Node MCU LED - the number of the output pin - onboard LED for debugging
                             // GPIO-02 ESP-12 LED
// Variables for touch sensor tracking
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
const long DEBOUNCE = 20;           // the debounce time, increase if the output flickers
const long LONG_PRESS_TIME = 6000;  // distinction between short and long press

// Variables for Lucon state machine
boolean powerState = HIGH;  // if device is on or off
boolean lightState = HIGH;  // if device is in decorative (high) or support (low) mode todo:lightState
enum Modes {DECORATIVE,
            BREATH,
            GLOW,
            SUPPORTREQUESTED
           };
enum Modes lampMode;
unsigned long lampModeTimer;
unsigned long neopixelTimer; 
const long BREATH_TIMEOUT = 500000;
const long GLOW_TIMEOUT = 500000;
const long SUPPORT_REQUESTED_TIMEOUT = 600000;
int responseCounter = 0;


// Initialize one continuous NeoPixel Strip
// while Adafruit claims this strip to be RGBW, I actually found the strip to behave as GRBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXEL_COUNT, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
// For RGB Neopixels, try using NEO_GRB. If so, make sure to check that all instances of strip.color() are compatible

// Initialize the Wifi and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Variables for the Wifi AP
bool shouldSaveConfig = false;


void setup() {
  // Debug console
  Serial.begin(115200);
  Serial.flush();

  // initialize capacitive touch buttons
  pinMode(touchPin, INPUT);

  // debug onboard LED
  pinMode(ledPin, OUTPUT);      // Onboard LED is active LOW
  digitalWrite(ledPin, LOW);    

  // initialize neopixel strip
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.fill(strip.Color(0, 0, 100, 0));    // turn blue while user sets up wifi password
  //strip.setBrightness(MAX_BRIGHTNESS);
  strip.show();  // Turn all LEDs off ASAP

  // Wifi AP - intialized in setup since we can get rid of it after
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); // for testing
  wifiManager.autoConnect(device_id);
  //setupFullWifiAP(); // if we want to collect mqtt info on the captive portal

  // MQTT Communication
  client.setServer(mqtt_server, atoi(mqtt_port)); // setServer port argument is int 
  client.setCallback(callback);

  strip.fill(strip.Color(100, 0, 0, 0));
  strip.show();
  Serial.print("lightState:"); Serial.println(lightState);
  Serial.print("lampMode: "); Serial.println(lampMode);
  Serial.println("Setup Complete");
} 

void loop() {
  // maintain MQTT Connection
  if (!client.connected())
  {
    
    reconnect();
  }
  client.loop();

  // continuously polls the capacitive sensor - TTP223B is actuve high
  currentState = digitalRead(touchPin);
  if (lastState == LOW && currentState == HIGH) {       // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if (lastState == HIGH && currentState == LOW) { // button is released
    isPressing = false;
    releasedTime = millis();
    //Serial.println(pressedTime); Serial.println(releasedTime); Serial.println(LONG_PRESS_TIME);

    // button is released within certain duration range
    long pressDuration = releasedTime - pressedTime;
    if (pressDuration > DEBOUNCE && pressDuration < LONG_PRESS_TIME) {
      Serial.println("A short press is detected");
      handleShortPress();
    }
  }
  // button has been pressed for a long time
  if (isPressing == true && isLongDetected == false) {
    if (millis() - pressedTime >= LONG_PRESS_TIME) {
      Serial.println("A long press is detected");
      handleLongPress();
      isLongDetected = true;
    }
  }
  lastState = currentState;  // save the the last state

  // NEOPIXEL behavior & lampModeTimeouts
  switch (lampMode) {
    case DECORATIVE:
      // do nothing
      break;
    case BREATH:
      //Serial.println("enter breath case of lampMode");
      pulseWhiteContinuously(PULSE_SLOW);
      if (millis() - lampModeTimer > BREATH_TIMEOUT) { //todo, deal with millis overflow
        Serial.println("BREATH_TIMEOUT Hit");
        setLampMode(DECORATIVE);
      }
      break;
    case GLOW:
        if (millis() - lampModeTimer > GLOW_TIMEOUT) { //todo, deal with millis overflow
        Serial.println("GLOW_TIMEOUT Hit");
        setLampMode(DECORATIVE);
        responseCounter = 0;
      }
      break;
    case SUPPORTREQUESTED:
      pulseWhiteContinuously(PULSE_FAST);
      if (millis() - lampModeTimer > SUPPORT_REQUESTED_TIMEOUT) {
        Serial.println("SUPPORT_REQUESTED_TIMEOUT Hit");
        pulseGreenOnce(PULSE_FAST);
        pulseGreenOnce(PULSE_FAST);
        pulseGreenOnce(PULSE_FAST);
        setLampMode(DECORATIVE);
      }
      break;
  }

} // end of loop ---

void handleShortPress() {         // on short press 
  Serial.print("lampMode: "); Serial.println(lampMode);
  switch (lampMode) {
    case DECORATIVE:
      setLightState(!lightState); // toggle lightState
      break;
    case BREATH:
      setLampMode(DECORATIVE);    // exit BREATH mode
      break;
    case GLOW:
      setLampMode(DECORATIVE);
      break;
    case SUPPORTREQUESTED:
      setLampMode(DECORATIVE);    // decline support request
      responseCounter = 0;
      break;
  }
}

void handleLongPress() {      // on long press
  switch (lampMode) {
    case DECORATIVE:
      requestSupport();       // request support and start BREATH
      setLampMode(BREATH);
      break;
    case BREATH:
      // DO NOTHING
      break;
    case GLOW:
      // DO NOTHING
      break;
    case SUPPORTREQUESTED:
      provideSupport();       // acknowledge support request
      setLampMode(GLOW);    // join in on BREATH session
      break;
  }
}


// sets the "Lamp Mode", a psedo-state machine
// DECORATIVE is the default mode, where the lamp can change light state via short press, or
// user can longPress to enter BREATH, or can transition to SUPPORTREQUESTED, if request comes through MQTT topic
// BREATH takes the user through a medidative session through pulsing night and invites other lamps in the network to join
// SUPPORTREQUESTED is entered when another lamp in the network requests support
void setLampMode(Modes nextLampMode) {
  if (nextLampMode == DECORATIVE) {
    // do nothing
  }
  else if (nextLampMode == BREATH) {
    // do nothing
  }
  else if (nextLampMode == SUPPORTREQUESTED) {
    // do nothing
  }
  setLightState(LOW);    // turn all NEOPIXELS off in mode transitions
  Serial.print("Previous lampMode:"); Serial.print(lampMode);
  lampMode = nextLampMode;
  Serial.print(" Current lampMode:"); Serial.println(lampMode);
  lampModeTimer = millis();
}

//to toggle light state pass in !lightState
void setLightState(boolean nextLightState) {
  if (nextLightState == HIGH) {
    setWhiteOn(DELAYVAL); //todo replace with variable
  }
  else if (nextLightState == LOW) {
    setWhiteOff(DELAYVAL);
  }
  Serial.print("Previous lightState:"); Serial.print(lightState);
  lightState = nextLightState;
  Serial.print(" Current lightState:"); Serial.println(lightState);
}

void requestSupport() {
  client.publish("LuconSupportRequest", device_id);
  Serial.println("Support has been requested");
}

void provideSupport() {
  client.publish("LuconSupportResponse", device_id);
  Serial.println("Support has been provided");
}

// handles new message from subscribed MQTT topics
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]  ");
  char message_buff[length + 1];
  for (int i = 0; i < length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[length] = '\0';
  String msgString = String(message_buff);
  Serial.println(msgString);
  if (strcmp(topic, "esp8266/led_control") == 0)
  {
    if (msgString == "1")
    {
      digitalWrite(ledPin, LOW); // PIN HIGH will switch OFF the relay //todo source
    }
    else if (msgString == "0")
    {
      digitalWrite(ledPin, HIGH); // PIN LOW will switch ON the relay
    }
  }
  else if (strcmp(topic, "LuconSupportRequest") == 0) {
    if (lampMode == DECORATIVE) {
      setLampMode(SUPPORTREQUESTED);
    }
  }
  else if (strcmp(topic, "LuconSupportResponse") == 0) {
    if (lampMode == BREATH) {
      setLampMode(GLOW);
      pulseGreenOnce(PULSE_MEDIUM);
      responseCounter=4;
    }
    if (lampMode == GLOW) {
      //pulseGreenOnce(PULSE_MEDIUM);
      responseCounter++;
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(40*responseCounter)));
      strip.show();
    }
  }
  // below cases are for demo code, in case you want to try controlling your device from MQTT App
  else if (strcmp(topic, "lucon_demo_control") == 0) {
    if (msgString == "shortPress") {
      handleShortPress();
    }
    else if (msgString == "longPress") {
      handleLongPress();
    }
    else if (msgString == "dec") {         // lamp in decorative mode, short press to turn light on
      if (lampMode == DECORATIVE);
      setLightState(HIGH);
    }
    else if (msgString == "off") {    // lamp in decorative mode, short press to turn light off
      if (lampMode == DECORATIVE);
      setLightState(LOW);
    }
    else if (msgString == "breath") {  // lamp in decorative mode, simulates long-press to start BREATH
      setLampMode(BREATH);
    }
    else if (msgString == "flash") {  // lamp in decorative mode, flash white quickly 
      setLampMode(SUPPORTREQUESTED);
    }
    else if (msgString == "response") { // lamp in breath mode, flashes green to simulate someone responding
      pulseGreenOnce(PULSE_FAST);       // even if you were in SUPPORTREQUESTED mode, you transition to
      pulseGreenOnce(PULSE_FAST);       // BREATH fast enough to get this message
      pulseGreenOnce(PULSE_FAST);
    }
  }
}

// establishes wifi & mqtt connection and subscribes to toptics
void reconnect()
{
  if (!client.connected()) //todo: allow user to use decorative mode without wifi connection
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(device_id, mqtt_username, mqtt_password))
    {
      Serial.println("connected");
      digitalWrite(ledPin, HIGH); //turn off the debug led todo
      strip.clear();              // turn off Neopixels when connected
      strip.show();
      client.publish("online", device_id);
      client.subscribe(sub_topic_name);
      client.subscribe("LuconSupportRequest");
      client.subscribe("LuconSupportResponse");
      client.subscribe("lucon_demo_control");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// to be used when we want to collect mqtt parameters from the user OR
// allow user to edit the default values
void setupFullWifiAP() {
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_username("cloud_username", "mqtt username", mqtt_username, 32);
  WiFiManagerParameter custom_mqtt_password("cloud_password", "mqtt password", mqtt_password, 32);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset saved settings - for testing
  //wifiManager.resetSettings();

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

    //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_username);
  wifiManager.addParameter(&custom_mqtt_password);

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(device_id);

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_username, custom_mqtt_username.getValue());
  strcpy(mqtt_password, custom_mqtt_password.getValue());

  Serial.println(WiFi.SSID());
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(mqtt_port));
  Serial.println("\tmqtt_username : " + String(mqtt_username));
  Serial.println("\tmqtt_password : " + String(mqtt_password));
}


// callback notifying us of the need to save config in the Full Wifi Setup
// currently not used
void saveConfigCallback () {  
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


// pulses White continuously by adjustable speed
// non-blocking, by using millis() timer
void pulseWhiteContinuously(uint8_t wait) {
  if (millis() - neopixelTimer > wait) {
    static uint16_t w = 0;
    static boolean reverse = false;
    if (reverse == false){ 
      w++;
      if (w >= 255) reverse = true;
    }
    else if (reverse == true) {
      w--;
      if (w <= 0) reverse = false;
    }
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    // strip.fill(strip.Color(w, w, w)); // for RGB
    strip.show();
    neopixelTimer = millis();
    //Serial.print("2 - Milis:"); Serial.print(millis()); Serial.print("  w :"); Serial.println(w);
  }
}


// pulses Green continuously by adjustable speed- not currently used
// non-blocking, by using millis() timer
void pulseGreenContinuously(uint8_t wait) {
  if (millis() - neopixelTimer > wait) {
    static uint16_t g = 0;
    static boolean reverse = false;
    if (reverse == false){ 
      g++;
      if (g >= 255) reverse = true;
    }
    else if (reverse == true) {
      g--;
      if (g <= 0) reverse = false;
    }
    strip.fill(strip.Color(g, 0, 0, 0));
    strip.show();
    neopixelTimer = millis();
    //Serial.print("2 - Milis:"); Serial.print(millis()); Serial.print("  g :"); Serial.println(g);
  }
}

// pulses Green color, by adjustable speed
// currently blocking due to delay function
void pulseGreenOnce(uint8_t wait) {
  for (int g = 0; g < 256; g++) { // Ramp up from 0 to 255
    strip.fill(strip.Color(g, 0, 0, 0));
    strip.show();
    delay(wait);
  }
  for (int g = 255; g >= 0; g--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(g, 0, 0, 0));
    strip.show();
    delay(wait);
  }
}

// transitions from off to full W brightness, by adjustable speed
// currently blocking due to delay function
void setWhiteOn(uint8_t wait) {
  for (int w = 0; w < 256; w++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'w':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    //strip.fill(strip.Color(w, w, w));  // for RGB
    strip.show();
    delay(wait);
  }
}

// transitions from full W brightness to light off, by adjustable speed
// currently blocking due to delay function
void setWhiteOff(uint8_t wait) {
  for (int w = 255; w >= 0; w--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(w)));
    //strip.fill(strip.Color(w, w, w));  // for RGB
    strip.show();
    delay(wait);
  }
}

// virtually turns on the lamp
void turnOn() {
  powerState = HIGH;
  Serial.print("Power State set to:"); Serial.println(powerState);
}

// turns all LEDs off and irtually turns off the lamp
void turnOff() {
  strip.clear(); // Set all pixel colors to 'off'
  strip.show();
  powerState = LOW;
  Serial.print("Power State set to"); Serial.println(powerState);
}
