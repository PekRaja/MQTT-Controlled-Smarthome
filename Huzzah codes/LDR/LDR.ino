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
 This is the code for a LDR controlled LED
 Both the LDR and the LED can be toggled via MQTT (a Raspberry Pi was used as the broker and WiFi hotspot)
 The LED is connected to pin 16 and the LDR to analog pin 0 (beware that Huzzah analog pin can only take maximum on 1V)
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

#define WLAN_SSID       "*******"
#define WLAN_PASS       "********"


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "***********"    //Broker IP or address
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "***********"    //Username that has been set up in your broker
#define AIO_KEY         "***********"    //Password for the above username in your broker
#define AIO_CLIENTID    "***********"    //Device id, this needs to be unique for each of your devices connected to your broker

/*********************** Define pins *****************************************/
#define BLUE_LED 2
#define H1 16                             //Room 1 ie. Livingroom
#define LDR A0

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_CLIENTID, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feeds called h1 and ldr
Adafruit_MQTT_Subscribe h1 = Adafruit_MQTT_Subscribe(&mqtt, "h1");
Adafruit_MQTT_Subscribe ldr = Adafruit_MQTT_Subscribe(&mqtt, "ldr");

// Setup functions to publish messages to topic info and h1
Adafruit_MQTT_Publish sendPub = Adafruit_MQTT_Publish(&mqtt, "info");
Adafruit_MQTT_Publish sendh1 = Adafruit_MQTT_Publish(&mqtt, "h1");

// Setup messages to be sent (these are used in info topic)
uint8_t setH1[] = "Livingroom Light On";
uint8_t setLDR[] = "LDR On";
uint8_t clearH1[] = "Livingroom Light Off";
uint8_t clearLDR[] = "LDR Off";

int ldrval = 0;
int h1state = LOW;
bool Auto = false;
/*************************** Sketch Code ************************************/
void Ldr()
{
  ldrval = analogRead(LDR);
  if(ldrval < 600 && h1state == LOW)
  {
    sendh1.publish(1);
  }
  else if(ldrval > 650 && h1state == HIGH)
  {
    sendh1.publish(0);
  }
  else
  {
    return;
  }
}

void h1callback(double x) {
  if(x > 0)
  {
    digitalWrite(H1,HIGH);
    h1state = HIGH;
    sendPub.publish(setH1, sizeof(setH1));
  }
  else{
    digitalWrite(H1,LOW);
    h1state = LOW;
    sendPub.publish(clearH1, sizeof(clearH1));
  }
}
void ldrcallback(double x) 
{
  if(x > 0)
  {
    Auto = true;
    digitalWrite(BLUE_LED,LOW);
    sendPub.publish(setLDR, sizeof(setLDR));
  }
  else
  {
    Auto = false;
    digitalWrite(BLUE_LED,HIGH);
    sendPub.publish(clearLDR, sizeof(clearLDR));
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(10);

/*******************Wifi connection*********************/

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
  

  pinMode(BLUE_LED,OUTPUT);
  pinMode(H1,OUTPUT);
  digitalWrite(H1,LOW);
  digitalWrite(BLUE_LED,HIGH);
  
/*******************Pin declaring*********************/
  
  h1.setCallback(h1callback);
  ldr.setCallback(ldrcallback);  
  // Setup MQTT subscription for h1 and ldr.
  mqtt.subscribe(&h1);
  mqtt.subscribe(&ldr);
}

uint32_t x=0;

void loop() 
{
/***MQTT function calls*****/
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
  
// LDR read only if it is set on
  if (Auto == true)
  {
    Ldr();
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
