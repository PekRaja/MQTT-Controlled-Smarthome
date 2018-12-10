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
 This is the code for MQTT-message controlled alarm system.
 The alarm system can be remotely activated via web-based UI page or MQTT mobile application.
 Alarm is triggered when the circuit supervising the door is broken and the buzzer will start playing an alarm sound until it is turned off by the user.
 Code is built based on an example by Tony DiCola mentioned above
 Project used this version of Huzzah https://www.adafruit.com/product/2821
 Made by Pekka Rajala, Aki Seppänen, Eemeli Tikka from Oulu University of Applied Sciences (second year students at the time)
 as part of a larger project for a remote controlled Smarthome.
 */

/*******************Libaries*********************/


#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/*******************Defines*********************/

/***************Wifi Access Point***************/

#define WLAN_SSID       "Raspi"
#define WLAN_PASS       "Ytrewq123"


#define AIO_SERVER      "172.20.248.132"

#define AIO_SERVERPORT  1883

#define AIO_USERNAME    "raspi"
#define AIO_KEY         "Ytrewq"
#define AIO_CLIENTID     "Alarm"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_CLIENTID, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe alarm = Adafruit_MQTT_Subscribe(&mqtt, "alarm");
Adafruit_MQTT_Publish sendPub = Adafruit_MQTT_Publish(&mqtt, "info");

/*******************Variable declaration*********************/

const byte INTERNAL_LED = 2;
const byte ALARM = 14;
const byte DOOR = 15;

bool alarmStatus = true;
bool doorStatus = true;
bool alarmTripped = false;
bool clearBit = true;
int laskuri = 0;

uint8_t setAlarm[] = "Alarm on";
uint8_t tripAlarm[] = "Alarm tripped";
uint8_t turnOffAlarm[] = "Alarm off";


/**********************ISR function************************/

void doorOpened ()
{
  
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();

//SW debounce to stabilize circuit triggering
   if (interrupt_time - last_interrupt_time > 400) 
  {

//Toggle door status each time interrupt is triggered
      doorStatus = !doorStatus;

//Alarm is tripped if this condition is fulfilled      
      if(doorStatus == false && alarmStatus == true)
      {
        alarmTripped = true;
      }
  
  last_interrupt_time = interrupt_time;
  }
}

/*******************Setup*********************/

void setup ()
{
//Initialize serial connection using baud rate of 115200  
  Serial.begin(115200);

/*******************Wifi connection*********************/

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

/*******************Pin declaring*********************/
  
  pinMode (INTERNAL_LED, OUTPUT); 
  pinMode (ALARM, OUTPUT);
  pinMode (DOOR, INPUT);
  
  attachInterrupt (digitalPinToInterrupt (DOOR), doorOpened, CHANGE);
  alarm.setCallback(alarmcallback);
  mqtt.subscribe(&alarm);

//Read door status each time Huzzah is reset
if (digitalRead(DOOR) == LOW) {
  doorStatus = false;
}

else {
  doorStatus = true;
}
}  

/*******************Function for alarm trigger*********************/

void alarmOnTripped() 
{
  
  digitalWrite(INTERNAL_LED, HIGH);
  if (clearBit == true) 
  {
    if (! sendPub.publish(tripAlarm, sizeof(tripAlarm)))   
    {
        Serial.println(F("Publish Failed."));
    }
    else 
    {
        Serial.println(F("Publish Success! Alarm message sent!"));
        clearBit = !clearBit;
    }
  }

  else if (clearBit == false)
  {
    for (int i = 0; i < 10; i++) 
    {
      digitalWrite(ALARM, LOW);
      delay(250);
      digitalWrite(ALARM, HIGH);
      delay(250); 
      digitalWrite(ALARM, LOW);
    }
  }    
}


/*******************Callback*********************/

void alarmcallback(double x)
{
  if(x > 0)
  {
    alarmStatus = true;
    digitalWrite (INTERNAL_LED, LOW);
    if (! sendPub.publish(setAlarm, sizeof(setAlarm)))
        Serial.println(F("Publish Failed."));
    else {
        Serial.println(F("Publish Success! Alarm has been set ON"));
    }
  }
  
  else
  {
    alarmStatus = false;
    digitalWrite (INTERNAL_LED, HIGH);
    alarmTripped = false;

        if (! sendPub.publish(turnOffAlarm, sizeof(turnOffAlarm)))
            Serial.println(F("Publish Failed."));
            
        else 
        {
            Serial.println(F("Publish Success! Alarm is now OFF"));
            clearBit = true;
        }
  }
}
uint32_t x=0;

/*******************Loop*********************/

void loop ()
{  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(5000);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds 
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  //Call this function if alarm is set on and alarm was tripped in the ISR
  if (alarmStatus == true && alarmTripped == true)
  {
    alarmOnTripped();
  } 

/****************Function for MQTT connection****************/

} 
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

