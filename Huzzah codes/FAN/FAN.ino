 /***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board:
  ----> https://www.adafruit.com/product/2471

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

 /*
 This is the code for MQTT-message controlled fan.
 The fan is powered by an external power-soource and controlled by Huzzah! pin 15 via FET
 Code is built based on an example by Tony DiCola mentioned above
 Project used this version of Huzzah https://www.adafruit.com/product/2821
 Made by Pekka Rajala, Aki Seppänen, Eemeli Tikka from Oulu University of Applied Sciences (second year students at the time)
 as part of a larger project for a remote controlled Smarthome.
 */
/*****************************Libraries**************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "***********"
#define WLAN_PASS       "***********"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "***********"    //Broker IP or address
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "***********"    //Username that has been set up in your broker
#define AIO_KEY         "***********"    //Password for the above username in your broker
#define AIO_CLIENTID    "***********"    //Device id, this needs to be unique for each of your devices connected to your broker

/****Define fan pin******/
#define FAN 15

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_CLIENTID, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'fan' this is needed to be subscribed to a topic called fan
Adafruit_MQTT_Subscribe fan = Adafruit_MQTT_Subscribe(&mqtt, "fan");

// Setup a topic called info to publish messages to
Adafruit_MQTT_Publish sendPub = Adafruit_MQTT_Publish(&mqtt, "info");

// Set messages to be published in a topic
uint8_t setFan[] = "Fan On";
uint8_t clearFan[] = "Fan Off";

/*************************** Sketch Code ************************************/

void fancallback(double x) 
{ 
  if(x > 0)
  {
    digitalWrite(FAN,HIGH);
    sendPub.publish(setFan, sizeof(setFan));
  }
  else
  {
    digitalWrite(FAN,LOW);
    sendPub.publish(clearFan, sizeof(clearFan));
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  pinMode(FAN,OUTPUT);
  digitalWrite(FAN,LOW);
 
  fan.setCallback(fancallback);

  // Setup MQTT subscription for fan.
  mqtt.subscribe(&fan);
}

uint32_t x=0;

void loop() 
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
