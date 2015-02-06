#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>

// Adafruit_BMP085 - https://github.com/adafruit/Adafruit-BMP085-Library
// PubSubClient - https://github.com/knolleary/pubsubclient

#define MQTT_SERVER "<YOUR MQTT HOST>"
#define MQTT_USERNAME "<MQTT USERNAME>"
#define MQTT_PASSWORD "<MQTT PASSWORD>"

#define MQTT_TOPIC "temperature"
#define MQTT_CLIENT_ID "ardu-1"
#define MQTT_DEVICE_LOCATION "computer rack"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
Adafruit_BMP085 bmp;
EthernetClient client;
PubSubClient mqtt(MQTT_SERVER, 1883, callback, client);

char message_buff[500];

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  setupEthernet();
  setupBmp();
}

void setupBmp() {
if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }  
}

void setupEthernet() {
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
  Serial.println(Ethernet.localIP()); 
}

void loop()
{
  if (!mqtt.connected())
  {
    mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
    Serial.print("establishing MQTT connection");
    Serial.print(mqtt.connected());
   }
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0;
    String pubString = "{\"degreesInCelsius\": " + String(temperature) + ",\"pressureInHPa\":" + String(pressure) + "}";
    pubString.toCharArray(message_buff, pubString.length()+1);
    Serial.println(pubString);
    mqtt.publish(MQTT_TOPIC, message_buff);
    mqtt.loop();
    delay(15000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

