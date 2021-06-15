# Introduction
This guide will walk through setting up your environment to be able to upload firmware to your microcontroller.

# Part 1 - Include the Libraries
Using the Manage Libraries tool in the arduino IDE add the following libraries.
![Screen Shot 2021-06-14 at 9 44 31 PM](https://user-images.githubusercontent.com/7762651/121994269-d4265f00-cd59-11eb-8d92-aea1364ede25.png)

1. Neopixel Library 1.7.0 - Adafruit
<img width="932" alt="Screen Shot 2021-06-14 at 9 46 32 PM" src="https://user-images.githubusercontent.com/7762651/121994455-25cee980-cd5a-11eb-8d31-0f83d2bbed92.png">
2. pubsubclient library (MQTT and Wifi Client) 2.8.0 
<img width="914" alt="Screen Shot 2021-06-14 at 9 46 43 PM" src="https://user-images.githubusercontent.com/7762651/121994475-32ebd880-cd5a-11eb-92c8-36aa7259fccd.png">
3. Wifi Manager Library 0.16.0 (Wifi AP & Captive Portal) - tzapu
<img width="932" alt="Screen Shot 2021-06-14 at 9 46 57 PM" src="https://user-images.githubusercontent.com/7762651/121994496-3d0dd700-cd5a-11eb-827d-83f5e64d2189.png">
4. ESP8266 Board (with ESP8266 Arduino Wifi Library)
Follow: https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

# Part 2 - Set up the Arduino IDE Board
Select the board that corresponds to your microcontroller.
For example for Adafruit Feather Huzzah ESP8266
<img width="789" alt="Screen Shot 2021-06-14 at 9 47 31 PM" src="https://user-images.githubusercontent.com/7762651/121994511-46973f00-cd5a-11eb-95e9-1d965f956e43.png">

Or for Node MCU
<img width="963" alt="Screen Shot 2021-06-14 at 9 56 40 PM" src="https://user-images.githubusercontent.com/7762651/121995212-6ed36d80-cd5b-11eb-9c35-32f00c5637da.png">


# Part 3 - Tips
* Check that your serial monitor is set to 115200, in case you open it later.
* Once your microcontroller is plugged in, check that the Port is properly set
* If you are having trouble, try https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide or other online resources
