#include <ArduinoOTA.h>
#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <LittleFS.h>
#include <cstdio>
#include "credentials.h" //external library for credentials

#define RGB_LED_PIN 3 //Led signal pin
#define POWER_PIN 14 //Signal pin to transistor that switch on ATX power supply
#define NUM_LED 109

//===============================================================
// Wifi Credentials
//===============================================================
const char* ssid = WIFI_SSID;  //SSID
const char* password = WIFI_PASSWORD; //Password

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LED, RGB_LED_PIN); /*
 * NeoPixelBus declaration for WS2812B pixels,
 * on ESP8266 only GPIO 3 pin or Rxo pin is available for this method */

AsyncWebServer server(80); //Web server on port 80

//Global Variables
unsigned long pixelsInterval = 40;  //animation interval
unsigned long rainbowPreviousMillis = 0; //rainbow animation clock

String animationMode = ""; //lighting mode: solid, rainbow animation

int Red = 0, Green = 0, Blue = 0; //Color values updated by handleColor()
int rainbowCycles = 0; //rainbow() animation state
String ledState;
String hexColor;
//===============================================================
// LED ANIMATIONS FUNCTIONS
//===============================================================

void setPixel(int, byte, byte, byte);

void setAll(byte, byte, byte);

RgbColor rainbowWheel(byte);

void rainbow();

//===============================================================
// Web-Client Handlers
//===============================================================
void handleColor(AsyncWebServerRequest *request);

void handlePower(AsyncWebServerRequest *request);

void handleAnimationState(AsyncWebServerRequest *request);

void updateLed(AsyncWebServerRequest *request);

void updateColor(AsyncWebServerRequest *request);

void netInit() //network initialization function
{
    WiFi.hostname(HOSTNAME);
    WiFi.begin(ssid, password);     //Connect to your WiFi router

    //SERIAL FEEDBACK SECTION
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED) { // Wait for connection
        delay(500);
        Serial.print(".");
    }
    Serial.println("");//If connection successful show IP address in serial monitor
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());//IP address assigned to your ESP


    //DEFINE HANDLERS
    server.on("/", HTTP_GET,
              [](AsyncWebServerRequest *request) {                     //Define the handling function for root path (HTML message)
                  request->send(LittleFS, "/index.html", String());
              });

    server.on("/javaScript.js",
              [](AsyncWebServerRequest *request) { //Define the handling function for the javascript path
                  request->send(LittleFS, "/javaScript.js", "text/html");
              });
    server.on("/iro5.js",
              [](AsyncWebServerRequest *request) { //Define the handling function for the color picker javascript path
                  request->send(LittleFS, "/iro5.js", "text/html");
              });
    server.on("/cssCode.css", [](AsyncWebServerRequest *request) { //Define the handling function for the CSS path
        request->send(LittleFS, "/cssCode.css", "text/css");
    });
    server.on("/setLED", handlePower); //Asynchronous javascript to send power led to server
    server.on("/animationState", handleAnimationState); //Asynchronous javascript to send animation state to server
    server.on("/color", handleColor); //Asynchronous javascript to send color to server
    server.on("/updateColor", updateColor);
    server.on("/updateLed", updateLed);

    server.begin(); //Start server

    Serial.println("HTTP server started"); //Feedback
}

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
    Serial.begin(115200); //setup serial for feedback

    LittleFS.begin(); //LittleFS filesystem initialization

    //Onboard LED port Direction output
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(POWER_PIN,OUTPUT);

    netInit(); //Network initialization

    strip.Begin(); //Addressable LEDs initialization
    strip.Show(); //Addressable LEDs all off
    //=============================
    //          OTA             //
    //============================
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { //U_FS
            type = "filesystem";
        }
        LittleFS.end(); //IMPORTANT: unmount filesystem before OTA update
        Serial.println("Start updating " + type); //Serial feedback
    });
    ArduinoOTA.onEnd([]() {
        ESP.restart();
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100))); //Serial feedback
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        } //Serial feedback
    });
    ArduinoOTA.begin(); //OTA initialization

    //Serial feedback
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

//==============================================================
//                     LOOP
//==============================================================

bool otaFlag = true; //

void loop(void){

   /* if(otaFlag)  //OTA enabled for 15 seconds after start for safety reasons
    {
        if (millis() <= 15000UL) {
            ArduinoOTA.handle();
        } else {
            otaFlag = false;
        }
    }*/
    ArduinoOTA.handle();
    if (WiFi.status() != WL_CONNECTED) //reinitialize server when connection lost
    {
        server.reset();
        server.end();
        netInit();
    }

    if(animationMode == "")//solid color mode
    {
        setAll(Red, Green, Blue);
    }
    else if(animationMode == "rain")//rainbow animation mode
    {
        if (millis() - rainbowPreviousMillis >= pixelsInterval) {
            rainbowPreviousMillis = millis();
            rainbow();
        }
    }

}

//===============================================================
// LED ANIMATIONS FUNCTIONS
//===============================================================
void setPixel(int Pixel, byte red, byte green, byte blue) //simplify setting specific pixel to specific color
{
    strip.SetPixelColor(Pixel, RgbColor(red, green, blue));
}

void setAll(byte red, byte green, byte blue) //all available led to specific color
{
    for(int i = 0; i < NUM_LED; i++ ) {
        setPixel(i, red, green, blue);
    }
    strip.Show();
}

RgbColor rainbowWheel(byte WheelPos) //drives rainbow animation
{
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return RgbColor(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
        WheelPos -= 85;
        return RgbColor(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return RgbColor(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow()
{
    for(uint16_t i=0; i<strip.PixelCount(); i++) {
        strip.SetPixelColor(i, rainbowWheel((i + rainbowCycles) & 255));
    }
    strip.Show(); //update strip colors
    rainbowCycles++; //update rainbow state
    if(rainbowCycles >= 256) rainbowCycles = 0; //reset rainbow animation to the beginning when ended
}

//===============================================================
// Web-Client Handlers
//===============================================================

void handleColor(AsyncWebServerRequest *request) //Used when client update color value
{
    auto hex = request->arg("hex").c_str(); //Get hex color value from client
    hexColor = hex;
    sscanf(hex, "%02x%02x%02x", &Red, &Green, &Blue); //Convert to Red, Green, Blue integers.
    request->send(200, "text/plain", hex); //Confirm receive with feedback
}

void handlePower(AsyncWebServerRequest *request) //Used when client switch led power button
{

    auto powerState = ledState = request->arg("LEDstate"); //Get power state from client

    if(powerState == "1") //Led turned on
    {
        digitalWrite(LED_BUILTIN,LOW); //Build-in led on as feedback
        digitalWrite(POWER_PIN, HIGH);//LED ON

    }
    else if(powerState == "0") //Led turned off
    {
        digitalWrite(LED_BUILTIN,HIGH); //Build-in led off as feedback
        digitalWrite(POWER_PIN, LOW);//LED OFF
    }

    request->send(200, "text/plain", powerState); //Confirm receive with feedback
}

void handleAnimationState(AsyncWebServerRequest *request) // //Used when client update animation type
{
    animationMode = request->arg("anim"); //update animation state
    request->send(200, "text/plain", animationMode); //Confirm receive with feedback
}

void updateColor(AsyncWebServerRequest *request) {
    request->send(200, "text/plane", hexColor);
}

void updateLed(AsyncWebServerRequest *request) {
    request->send(200, "text/plane", ledState);
}
