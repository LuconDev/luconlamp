/*
   References:
   MQTT and Wifi Client - Varun Sonava - Public Domain
   https://medium.com/dataseries/smart-led-iot-diy-698512b26730
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

// in credentials.h
//const char *ssid = "";
//const char *password = "";
//const char *device_id = "";
//const char *mqtt_server = "";
//const int mqtt_port = ;
//const char *cloud_username = "";
//const char *cloud_password = "";
//const char *sub_topic_name = "";

WiFiClient espClient;
PubSubClient client(espClient);

const byte ledPin = 16;

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  char message_buff[length+1];
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
      digitalWrite(ledPin, LOW); // PIN HIGH will switch OFF the relay
    }
    if (msgString == "0")
    {
      digitalWrite(ledPin, HIGH); // PIN LOW will switch ON the relay
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(device_id, cloud_username, cloud_password))
    {
      Serial.println("connected");
      client.publish("online", device_id);
      client.subscribe(sub_topic_name); // write your unique ID here
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
void setup()
{
  Serial.begin(9600);
  client.setServer(mqtt_server, mqtt_port); // change port number as mentioned in your cloudmqtt console
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
