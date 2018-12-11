# MQTT-Controlled-Smarthome
Oulu University of Applied Sciences, second year communications application project by Pekka Rajala, Aki Seppänen and Eemeli Tikka

In this project we had four Adafruit Feather HUZZAH ESP8266 devices working as different parts of a Smarthome environment.
https://www.adafruit.com/product/2821 <-- Link to the exact  model we used.
- Temperature sensor with DHT11
- Lights represented by a LED with an LDR-sensor
- 12v fan powered by and external power source and controlled with a mosfet. (During testing we used a 9v battery as the power source)
- Door alarm with a buzzer

We had a Raspbery pi 3 working as a WiFi-hotspot, Mqtt broker(Mosquitto), Mysql database and website host for the end devices.
