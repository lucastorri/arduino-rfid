# Arduino-RFID

Identify users using RFID cards. Similar to [mifare-card-reader](https://github.com/lucastorri/mifare-card-reader), but for arduino and with more stuff.

That's how it looks like: 

![Image](docs/images/IMG_6918.jpg?raw=true)


## What it does?

On starting, the LCD presents a "booting" message and then proceeds to display current temperature/humidity. Once a card is read, two things can happen:

 * matches a known user: a "welcome" message is diplayed on the LCD and a green LED is activated for 3 seconds. Also, a relay is activated for a few seconds, which can be used to activate an electrict door lock;
 * don't match any know user: a red LED is activated for 3 seconds, plus a "unknown" LCD message.

On the meanwhile the last known card uid read together with temperature/humidity can be requested to the device through HTTP.


## TODO

* Have a server side app
  * associate cards with user's Facebook profiles


## References

#### Arduino Kit

* <http://dx.com/p/rfid-stepper-motor-learning-kit-for-arduino-multicolored-247020>


#### Ethernet

* <http://arduino.cc/en/Main/ArduinoEthernetShield>


#### DHT11

* <https://github.com/niesteszeck/idDHT11>
* <http://chrisbaume.wordpress.com/2013/02/10/beer-monitoring/>
* <https://github.com/mkschreder/example-dht-11>


#### LCD

* <http://magnusglad.wordpress.com/tag/lcm1602/>


#### RFID

* <https://github.com/miguelbalboa/rfid>
* <http://fritzing.org/projects/display-16x2-rfid-rc522/>


#### Scheduler

* <https://github.com/lucastorri/arduino-scheduler>
