#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"
#include <cstdio>

//Static IP address configuration
IPAddress staticIP(192, 168, 66, 63); //ESP static ip
IPAddress gateway(192, 168, 66, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(192, 168, 66, 1);  //DNS

const char* ssid = "Katowice";
const char* password = "Akant24#!";

#define PIN 4
#define NUM_LEDS 60
int Red, Green, Blue;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80); //Server on port 80
#define LED 2  //On board LED
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
//zmiana
void FadeInOut(byte red, byte green, byte blue){
    float r, g, b;

    for(int k = 0; k < 256; k=k+1) {
        r = (k/256.0)*red;
        g = (k/256.0)*green;
        b = (k/256.0)*blue;
        setAll(r,g,b);
        strip.show();
        delay(5);
    }

    for(int k = 255; k >= 0; k=k-2) {
        r = (k/256.0)*red;
        g = (k/256.0)*green;
        b = (k/256.0)*blue;
        setAll(r,g,b);
        strip.show();
        delay(5);
    }
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

void handleColor(AsyncWebServerRequest *request) {
    auto hex_state = request->arg("hex");
    const char *cstr = hex_state.c_str();
    Serial.println(hex_state);
    sscanf(cstr, "%02x%02x%02x", &Red, &Green, &Blue);
    Serial.println(Red);
    Serial.println(Green);
    Serial.println(Blue);
    setAll(Red, Green, Blue);
    request->send(200, "text/plane", hex_state);
}

void handleLED(AsyncWebServerRequest *request) {

    String ledState = "OFF";
    String t_state = request->arg("LEDstate");
    Serial.println(t_state);

    if(t_state == "1")
    {
        digitalWrite(LED,LOW); //LED ON
        ledState = "ON"; //Feedback parameter
    }
    else
    {
        digitalWrite(LED,HIGH); //LED OFF
        ledState = "OFF"; //Feedback parameter
    }

    request->send(200, "text/plane", ledState); //Send web page
}

String animationState;
void handleAnimation(AsyncWebServerRequest *request) {
    animationState = request->arg("anim");
    Serial.println(animationState);
    if(animationState == "none")
    {
        setAll(Red, Green, Blue);
    }
    if(animationState == "rain") {
            uint16_t i, j;
            for (j = 0; j < 256; j++) {
                for (i = 0; i < strip.numPixels(); i++) {
                    strip.setPixelColor(i, Wheel((i + j) & 255));
                }
                strip.show();
                delay(40);
            }
        }
    if(animationState == "fade"){
        float r, g, b;
        for(int k = 0; k < 256; k=k+1) {
            r = (k/256.0)*Red;
            g = (k/256.0)*Green;
            b = (k/256.0)*Blue;
            setAll(r,g,b);
            strip.show();
            delay(5);
        }

        for(int k = 255; k >= 0; k=k-2) {
            r = (k/256.0)*Red;
            g = (k/256.0)*Green;
            b = (k/256.0)*Blue;
            setAll(r,g,b);
            strip.show();
            delay(5);
        }
    }
    if(animationState == "wipe"){
        for(uint16_t i=0; i<NUM_LEDS; i++) {
            setPixel(i, Red, Green, Blue);
            strip.show();
            delay(40);
        }
        for(uint16_t i=0; i<NUM_LEDS; i++) {
            setPixel(i, 0, 0, 0);
            strip.show();
            delay(40);
        }
    }
    request->send(200, "text/plane", animationState);
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
    Serial.begin(115200);

    strip.begin();
    strip.show();

    WiFi.hostname("Wemos_Led_RGB");
    WiFi.begin(ssid, password);     //Connect to your WiFi router
    Serial.println("");

    //Onboard LED port Direction output
    pinMode(LED,OUTPUT);

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

        request->send(200, "text/html", MAIN_page);

    });

    server.on("/javaScript", [](AsyncWebServerRequest *request) { //Define the handling function for the javascript path

        request->send(200, "text/html", javaScript);

    });

    server.on("/cssCode", [](AsyncWebServerRequest *request) { //Define the handling function for the CSS path

        request->send(200, "text/css", cssCode);

    });
    server.on("/setLED", handleLED);
    server.on("/animationState", handleAnimation);
    server.on("/color", handleColor);
    server.begin();                  //Start server
    Serial.println("HTTP server started");
    FadeInOut(100, 100, 100);
}
//==============================================================
//                     LOOP
//==============================================================

void loop(void){

}