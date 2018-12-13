# MQTT-Controlled-Smarthome
Oulu University of Applied Sciences, second year communications application project by Pekka Rajala, Aki Seppänen and Eemeli Tikka.

Still updating and editing this readme file, but all the codes are finished

In this project we had four Adafruit Feather HUZZAH ESP8266 devices working as different parts of a Smarthome environment.
https://www.adafruit.com/product/2821 <-- Link to the exact  model we used. 
All the codes should work the same with other versions of ESP8266.
- Temperature sensor with DHT11
- Lights represented by a LED with an LDR-sensor
- 12v fan powered by and external power source and controlled with a mosfet. (During testing we used a 9v battery as the power source)
- Door alarm with a buzzer

If you wish to build this yourself there is the Circuit diagram.jpeg that shows the basic connections of all four Huzzah boards and separate images of all these built on a breadboard.

We had a Raspberry Pi 3 working as a WiFi-hotspot, Mqtt broker(Mosquitto), Mysql database and website host for the end devices.

Links to Raspberry tutorials below
  Wifi hotspot: https://thepi.io/how-to-use-your-raspberry-pi-as-a-wireless-access-point/ 
  MQTT Broker tutorial: https://www.instructables.com/id/How-to-Use-MQTT-With-the-Raspberry-Pi-and-ESP8266/
  Mysql and php https://pimylifeup.com/raspberry-pi-mysql-phpmyadmin/

We made and uploaded the database tables, processes and triggers to Raspberry with MySQL Workbench. 
Find it here: https://www.mysql.com/products/workbench/

Website codes are quite basic, but atleast they work. The background image is from https://pixabay.com/en/smart-home-house-technology-2005993/ and is Free for commercial use.
The buttons work by sending a terminal command to Raspberry to publish a mqtt message.
You can also use MQTT Dash to control these devices https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en (not ours, we just found it quite useful)
