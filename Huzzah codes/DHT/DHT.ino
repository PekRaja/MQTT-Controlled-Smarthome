/***************************************************
  Adafruit MQTT Library Arbitrary Data Example
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Stuart Feichtinger
  Modifed from the mqtt_esp8266 example written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

 /*
 This is the code for a DHT sensor
 The DHT sends data over WiFi using MQTT (a Raspberry Pi was used as the broker and WiFi hotspot)
 Code is built based on an example by Stuart Feichtinger and Tony DiCola mentioned above
 Project used this version of Huzzah https://www.adafruit.com/product/2821
 Made by Pekka Rajala, Aki Seppänen, Eemeli Tikka from Oulu University of Applied Sciences (second year students at the time)
 as part of a larger project for a remote controlled Smarthome.
 */

#include <DHT.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/**************************** DHT11 Setup ************************************/

#define DHTPIN 14     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11 and 22 supported
DHT dht(DHTPIN, DHTTYPE);


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "******"
#define WLAN_PASS       "*********"

/************************* Adafruit.io Setup *********************************/

#define ARB_SERVER      "255.255.255.255"
#define ARB_SERVERPORT  1883                   // use 8883 for SSL
#define ARB_USERNAME    "****"
#define ARB_PW          "*******"


/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, ARB_SERVER, ARB_SERVERPORT, ARB_USERNAME, ARB_PW);

/****************************** Feeds ***************************************/

// Setup a feed called 'arb_packet' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
#define ARB_FEED "dht"
Adafruit_MQTT_Publish ap = Adafruit_MQTT_Publish(&mqtt, ARB_FEED);


void MQTT_connect();


void setup() {

  
  dht.begin();
  
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: ")); 
  Serial.println(WiFi.localIP());

}
  
//Intrducing variables
float temp = 0;
float old_temp = 0;
//float hum = 0;
unsigned long time_ms = 0;
int period = 5000;


void loop() 
{
  
  MQTT_connect();


  //delay to cut measurement rate
  if(millis() > time_ms + period) {
    
  time_ms = millis();    
  //Read sensor values
  temp = dht.readTemperature();
  //hum = dht.readHumidity();
  }


  //Serial prints for debug
  //Serial.println(temp);
  //Serial.println(old_temp);
  //Serial.println(hum);


  if (temp != old_temp) {
    ap.publish(temp);
    old_temp = temp;
  }
    
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting to MQTT... "));

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println(F("Retrying MQTT connection in 5 seconds..."));
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println(F("MQTT Connected!"));
}
