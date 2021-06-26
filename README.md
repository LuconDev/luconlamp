# luconlamp

Lucon is a connected lamp kit for connecting people. 

Lucon was launched as a [Kickstarter campaign](https://www.kickstarter.com/projects/chrisdats/lucon-a-connected-lamp-kit-for-connecting-people-make-100) in early 2021. We decided to open source the software to give backers flexibility to completely customize their lamp’s behaviors and expressions and to jumpstart the Lucon community.

## Getting Started

The [setup guide](https://github.com/LuconDev/luconlamp/blob/main/setupguide.md) will walk through setting up your environment to be able to upload firmware to your microcontroller.

### Prerequisites

To be able to compile and upload the firmware you will need to add the following libraries to your Arduino project.
- Arduino IDE
- Adafruit Neopixel by Adafruit
- PubSubClient by Nick O'Leary
- WiFiManager by tzapu,tablatronix
- esp8266 by ESP8266 Community (Board)


## Tests

To help isolate issues, we've developed [test code](https://github.com/LuconDev/luconlamp/tree/main/tests) that target specific subsystems of the Lucon lamp.

### List of end-to-end tests

The end-to-end tests cover:
- Lucon switch
- MQTT communication
- Neopixel (LED strip)
- Wifi Access Point
- Touch Sensor


## Contributing

Please read [CONTRIBUTING.md](https://github.com/LuconDev/luconlamp/blob/main/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/LuconDev/luconlamp/tags). 

## Authors

* **Christopher Datsikas** - *Initial work*

See also the list of [contributors](https://github.com/LuconDev/luconlamp/contributors) who participated in this project.


## License

This project is licensed under the GPLv3.0 License - see the [LICENSE.md](LICENSE.md) file for details

