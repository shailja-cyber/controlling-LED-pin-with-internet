int rled = D1 ;
int  gled = D2 ;
int bled = D3 ;


#include "DHTesp.h"
DHTesp dht ;

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "shailja"
#define WLAN_PASS       "kalmilteh"

//#define WLAN_SSID       "Yashi"
//#define WLAN_PASS       "12345678"


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "shailja123"
#define AIO_KEY         "412d7c873b734257aa4eab27743a1229"

/************ Global State (you don't need to change this!) ******************/
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");
Adafruit_MQTT_Publish test = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/test");


Adafruit_MQTT_Subscribe call = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/call");

Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");


/*************************** Sketch Code ************************************/
void MQTT_connect();



void setup() 
{
  dht.setup(D4); // Connect DHT sensor to GPIO 17
  pinMode(rled, OUTPUT);
   pinMode(gled, OUTPUT);
   pinMode(bled, OUTPUT);

  
  Serial.begin(115200);
  delay(10);
  
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  mqtt.subscribe(&onoffbutton);
  mqtt.subscribe(&call);

  
  
}

//uint32_t x=0;

void loop() 
{ 
  
  delay(dht.getMinimumSamplingPeriod());
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  Serial.print(humidity);
  Serial.println(temperature) ;
  
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //Serial.print( &onoffbutton);
    if (subscription == &onoffbutton) 
    {
      if(strcmp((char *)onoffbutton.lastread, "0") == 0)
      {
        digitalWrite(rled,HIGH);
        
        Serial.println("red Led off");
      }
      if(strcmp((char *)onoffbutton.lastread, "1") == 0)
      {
        digitalWrite(rled,LOW);
        
        Serial.println("Led on");
      }
      if(strcmp((char *)onoffbutton.lastread, "2") == 0)
      {
        digitalWrite(gled,HIGH);
        
        Serial.println("green led off");
      }
      if(strcmp((char *)onoffbutton.lastread, "3") == 0)
      {
        digitalWrite(gled,LOW);
        
        Serial.println("green led on");
      }
      if(strcmp((char *)onoffbutton.lastread, "4") == 0)
      {
        digitalWrite(bled,HIGH);
        Serial.println("blue led off");
        
      }
      if(strcmp((char *)onoffbutton.lastread, "5") == 0)
      {
        digitalWrite(bled,LOW);
        Serial.println("blue led on");
      }
    }
     
      
   
    
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
        
    }
 
  // Now we can publish stuff!
   Serial.println(humidity);
  photocell.publish(humidity);
  test.publish(temperature);  
}



void MQTT_connect() 
{
  int8_t ret;
  if (mqtt.connected()) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) 
  {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
