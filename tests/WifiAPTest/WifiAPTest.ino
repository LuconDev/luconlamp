#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


//define your default values here, if there are different values in config.json, they are overwritten.
 char *device_id = "Lucon-A-Test";
 char *mqtt_server = "driver.cloudmqtt.com";
 char *mqtt_port = "18950";
 char *mqtt_username = "stmrlnmq";
 char *mqtt_password = "IRSrsl2OgNzm";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Top. The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(mqtt_port));
  Serial.println("\tmqtt_username : " + String(mqtt_username));
  Serial.println("\tmqtt_password : " + String(mqtt_password));


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
  wifiManager.resetSettings();

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

    //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_username);
  wifiManager.addParameter(&custom_mqtt_password);


  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");

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

void loop() {
  // put your main code here, to run repeatedly:

}
