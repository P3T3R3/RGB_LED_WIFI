#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <cstdio>
#include "LittleFS.h"

const char* ssid = "Katowice";
const char* password = "Akant24#!";

#define PIN 4
#define POWER_PIN 14
#define NUM_LEDS 120
#define LED 2  //On board LED

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80); //Server on port 80

unsigned long previousMillis = 0;
unsigned long pixelsInterval=40;  // the time we need to wait
unsigned long colorWipePreviousMillis=0;
unsigned long rainbowPreviousMillis=0;
unsigned long colorWipePreviousMillis2=0;
unsigned long rainbowCyclesPreviousMillis=0;

int Red, Green, Blue;
int rainbowCycles = 0;
int rainbowCycleCycles = 0;
uint16_t currentPixel = 0;

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================

void setPixel(int Pixel, byte red, byte green, byte blue) {

    strip.setPixelColor(Pixel, strip.Color(red, green, blue));

}

void setAll(byte red, byte green, byte blue) {
    for(int i = 0; i < NUM_LEDS; i++ ) {
        setPixel(i, red, green, blue);
    }
    strip.show();
}

uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorWipe(uint32_t c){
    strip.setPixelColor(currentPixel,c);
    strip.show();
    currentPixel++;
    if(currentPixel == NUM_LEDS){
        currentPixel = 0;
    }
}

void rainbow() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i+rainbowCycles) & 255));
    }
    strip.show();
    rainbowCycles++;
    if(rainbowCycles >= 256) rainbowCycles = 0;
}

void handleColor(AsyncWebServerRequest *request) {
    auto hex_state = request->arg("hex");
    const char *cstr = hex_state.c_str();
    Serial.println(hex_state);
    sscanf(cstr, "%02x%02x%02x", &Red, &Green, &Blue);
    Serial.println(Red);
    Serial.println(Green);
    Serial.println(Blue);
    request->send(200, "text/plane", hex_state);
}

void handleLED(AsyncWebServerRequest *request) {

    String ledState = "OFF";
    String t_state = request->arg("LEDstate");
    Serial.println(t_state);

    if(t_state == "1")
    {
        digitalWrite(LED,LOW);
        digitalWrite(POWER_PIN, HIGH);//LED ON
        ledState = "ON"; //Feedback parameter
    }
    else
    {
        digitalWrite(LED,HIGH);
        digitalWrite(POWER_PIN, LOW);//LED OFF
        ledState = "OFF"; //Feedback parameter
    }

    request->send(200, "text/plane", ledState); //Send web page
}

String animationState;
void handleAnimation(AsyncWebServerRequest *request) {
    animationState = request->arg("anim");
    Serial.println(animationState);
    request->send(200, "text/plane", animationState);
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
    Serial.begin(115200);
    LittleFS.begin();
    strip.begin();
    strip.show();
    currentPixel = 0;
    WiFi.hostname("Wemos_Led_RGB");
    WiFi.begin(ssid, password);     //Connect to your WiFi router
    Serial.println("");

    //Onboard LED port Direction output
    pinMode(LED,OUTPUT);
    pinMode(POWER_PIN,OUTPUT);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    //If connection successful show IP address in serial monitor
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    server.on("/",HTTP_GET, [](AsyncWebServerRequest *request) {                     //Define the handling function for root path (HTML message)

        request->send(LittleFS, "/index.html", String());

    });

    server.on("/javaScript.js", [](AsyncWebServerRequest *request) { //Define the handling function for the javascript path

        request->send(LittleFS,"/javaScript.js", "text/html");

    });

    server.on("/cssCode.css", [](AsyncWebServerRequest *request) { //Define the handling function for the CSS path

        request->send(LittleFS,"/cssCode.css", "text/css");

    });
    server.on("/setLED", handleLED);
    server.on("/animationState", handleAnimation);
    server.on("/color", handleColor);
    server.begin();                  //Start server
    Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================

void loop(void){
    if(animationState == "none")
    {
        setAll(Red, Green, Blue);
    }
    if(animationState == "rain") {
        if ((unsigned long)(millis() - rainbowPreviousMillis) >= pixelsInterval) {
            rainbowPreviousMillis = millis();
            rainbow();
        }
    }
    /*if(animationState == "fade"){
        float r, g, b;
        for(int k = 0; k < 256; k=k+1) {
            r = (k/256.0)*Red;
            g = (k/256.0)*Green;
            b = (k/256.0)*Blue;
            setAll(r,g,b);
            strip.show();
        }

        for(int k = 255; k >= 0; k=k-2) {
            r = (k/256.0)*Red;
            g = (k/256.0)*Green;
            b = (k/256.0)*Blue;
            setAll(r,g,b);
            strip.show();
        }
    }*/
    if(animationState == "wipe"){
        if ((unsigned long)(millis() - colorWipePreviousMillis) >= pixelsInterval) {
            colorWipePreviousMillis = millis();
            colorWipe(strip.Color(Red,Green,Blue));
            }
        if ((unsigned long)(millis() - colorWipePreviousMillis2) >= pixelsInterval) {
            colorWipePreviousMillis2 = millis();
            colorWipe(strip.Color(0,0,0));
        }

    }
}