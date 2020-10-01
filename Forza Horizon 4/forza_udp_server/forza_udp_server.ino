#include <ESPAsyncUDP.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>

//Change these variables
#define LED_PIN         5
#define NUM_LEDS        11
#define RPM_LEDS        5
#define ACCEL_LEDS      3
#define BRAKE_LEDS      3
#define BRIGHTNESS      150
#define LED_TYPE        WS2811
#define COLOR_ORDER     BRG

CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 60

//Change these variables
IPAddress staticIP(192, 168, 0, 90); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask/
IPAddress primaryDNS(1, 1, 1, 1);  //Primary DNS
IPAddress secondaryDNS(1, 0, 0, 1);  //Secondary DNS

//Set your router's SSID and Password
const char* ssid = "";
const char* password = "";

//Creating UDP Listener Object.
AsyncUDP forzaUDP;
const int UDPPort = 4123;

//Packet Buffer
const int packetSize = 324;
byte packetBuffer[packetSize];

char maxRPM[4]; // four bytes in a float 32
char rpm[4]; // four bytes in a float 32
char idle[4]; // four bytes in a float 32
byte Accel;
byte Brake;
float EngineMaxRPM;
float EngineIdleRPM;
float CurrentEngineRPM;
int rpm_percent;
int accel_percent;
int brake_percent;
char tacho[RPM_LEDS];
char throttle[ACCEL_LEDS];
char brakes[BRAKE_LEDS];

int tacho_led = 100/RPM_LEDS;
int accel_led = 100/ACCEL_LEDS;
int brake_led = 100/BRAKE_LEDS;
int rpm_led;
int throttle_led;
int brakes_led;

int debug=0;

void setup() {
  if(debug) {
    Serial.begin(9600);
  }
  if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
    if(debug) {
      Serial.println("STA Failed to configure");
    }
  }
  if(debug) {
    Serial.print("\nConnecting");
  }

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debug) {
      Serial.print(".");
    }
  }
  
  if(debug) {
    Serial.println("Connection to wifi successful");
  }
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  //Accel LEDs
  for(int i=0;i<3;i++) {
    leds[i] = CHSV(HUE_GREEN, 255, BRIGHTNESS);
  }

  //RPM LEDs
  leds[7] = CHSV(HUE_GREEN, 255, BRIGHTNESS);
  leds[6] = CHSV(HUE_GREEN, 255, BRIGHTNESS);
  leds[5] = CHSV(HUE_ORANGE, 255, BRIGHTNESS);
  leds[4] = CHSV(5, 255, BRIGHTNESS);         //light red
  leds[3] = CHSV(170, 240, BRIGHTNESS);       //blue

  
  //Brake LEDs
  for(int j=8;j<11;j++) {
    leds[j] = CHSV(HUE_RED, 255, BRIGHTNESS);
  }
  
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.show();
}

void loop() {
  // Start UDP server
  if (forzaUDP.listen(UDPPort)) {
    
    forzaUDP.onPacket([](AsyncUDPPacket packet) {

      //Get EngineMaxRPM (f32), Byte offset: 8:12
      maxRPM[0] = packet.data()[8];
      maxRPM[1] = packet.data()[9];
      maxRPM[2] = packet.data()[10];
      maxRPM[3] = packet.data()[11];
      EngineMaxRPM = *( (float*) maxRPM ); // Convert byte array to floating point number

      //Get EngineIdleRPM (f32), Byte offset: 12:16
      idle[0] = packet.data()[12];
      idle[1] = packet.data()[13];
      idle[2] = packet.data()[14];
      idle[3] = packet.data()[15];
      EngineIdleRPM = *( (float*) idle ); // Convert byte array to floating point number

      //Subtract EngineIdleRPM from EngineMaxRPM to make the RPM led strip more accurate
      EngineMaxRPM = EngineMaxRPM - EngineIdleRPM;

      // Get CurrentEngineRPM (f32), Byte offset: 16:20
      rpm[0] = packet.data()[16];
      rpm[1] = packet.data()[17];
      rpm[2] = packet.data()[18];
      rpm[3] = packet.data()[19];
      CurrentEngineRPM = *( (float*) rpm ); // Convert byte array to floating point number

      // Get Accelerator Position (u8), Byte offset: 315
      Accel = packet.data()[315];

      // Get Brake Position (u8), Byte offset: 316
      Brake = packet.data()[316];

      if(CurrentEngineRPM==0) {
        for(int j=0;j<NUM_LEDS;j++) {
          leds[j] = CHSV(0, 0, BRIGHTNESS);
        }
      } else {
        //Set RPM Lights
        rpm_percent = (int) 100*(CurrentEngineRPM/EngineMaxRPM);
        
        for(int i=0; i<RPM_LEDS;i++) {
          rpm_led = min(rpm_percent, tacho_led);
          tacho[i] = map(max(0, rpm_led), 0, tacho_led, 0, BRIGHTNESS);
          rpm_percent = rpm_percent - tacho_led;
        }
        
        leds[7] = CHSV(HUE_GREEN, 255, tacho[0]);
        leds[6] = CHSV(HUE_GREEN, 255, tacho[1]);
        leds[5] = CHSV(HUE_ORANGE, 255, tacho[2]);
        leds[4] = CHSV(5, 255, tacho[3]);
        leds[3] = CHSV(170, 240, tacho[4]);

        //Set Accelerator Lights
        accel_percent = (100*Accel)/255;
        for(int j=0;j<ACCEL_LEDS;j++) {
          throttle_led = min(accel_percent, accel_led);
          throttle[j] = map(max(0, throttle_led), 0, accel_led, 0, BRIGHTNESS);
          accel_percent = accel_percent - accel_led;
        }
  
        leds[2] = CHSV(HUE_GREEN, 255, throttle[0]);
        leds[1] = CHSV(HUE_GREEN, 255, throttle[1]);
        leds[0] = CHSV(HUE_GREEN, 255, throttle[2]);
  
        //Set Brake Lights
        brake_percent = (100*Brake)/255;
        for(int k=0;k<BRAKE_LEDS;k++) {
          brakes_led = min(brake_percent, brake_led);
          brakes[k] = map(max(0, brakes_led), 0, brake_led, 0, BRIGHTNESS);
          brake_percent = brake_percent - brake_led;
        }
  
        leds[8] = CHSV(HUE_RED, 255, brakes[0]);
        leds[9] = CHSV(HUE_RED, 255, brakes[1]);
        leds[10] = CHSV(HUE_RED, 255, brakes[2]);
      }

      FastLED.show();
    });
  }
}
