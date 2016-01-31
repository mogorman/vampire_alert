#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <math.h>

#include <PubSubClient.h>


const char* ssid = "jingle_bells";
const char* password = "0123456789abcdef";
const char* host = "orlock";
const char* mqtt_server = "home.rldn.net";

#define LED 2
#define PWMA 5
#define MOTOR_SPEED 200

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
uint8_t lock_state;


int16_t test_var = 0;
int16_t toggle = 0;
unsigned long timer = 0;

boolean mog_toggle = true;
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval =120;           // interval at which to blink (milliseconds)

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

void unlock();
void lock();
void reset_lock();
void stop_motor();
void move_motor(uint8_t dir);

void callback(char* topic, byte* payload, unsigned int length)
{
   bite();
   bite();
   analogWrite(PWMA, 0);
   analogWrite(LED,1023);
   client.publish("orlock", "bite");
}


void setup() {
  pinMode(LED, OUTPUT);
  pinMode(PWMA, OUTPUT);
  digitalWrite(LED, LOW);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  
  ArduinoOTA.setPort(8266);

  ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() {
//    digitalWrite(LED, LOW);
     asm("nop");
  });
  ArduinoOTA.onEnd([]() {
//    digitalWrite(LED, LOW);
    asm("nop");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//    digitalWrite(LED,progress%2);
     asm("nop");
  });
  ArduinoOTA.onError([](ota_error_t error) {
//    digitalWrite(LED, LOW);
    asm("nop");
//    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
//    // Attempt to connect
    if (client.connect("ESP8266Client")) {
//      // Once connected, publish an announcement...
      client.publish("orlock", "connected");
      // ... and resubscribe
      client.subscribe("bitlbee2/alert/message");
    } else {
      delay(5000);
    }
  }
}

void loop()
{

    for(int i = 0 ; i < 10; i++) {
       ArduinoOTA.handle();
    }
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
}


void bite()
{
  /* digitalWrite(LED, mog_toggle); */
  /* mog_toggle  = !mog_toggle; */
  int temp_fade = fadeAmount;
  while(temp_fade == fadeAmount) {				/*  */
  interval = 5000;
  // set the brightness of pin 9:
  analogWrite(LED, 1023-brightness);
  analogWrite(PWMA, brightness);
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0) {
    fadeAmount = 5;
    brightness = 0;
  } else if ( brightness >= 1023) {
      fadeAmount = -5;
      brightness = 1023;
    }
  delay(10);
  }
  
  // wait for 30 milliseconds to see the dimming effect    
  //  delayMicroa/seconds(interval);  
}
