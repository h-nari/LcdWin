#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <time.h>
#include "ota_util.h"

#include <Adafruit_GFX.h>	
#include <Fontx.h>							
#include <Humblesoft_GFX.h>			
#include <Humblesoft_ILI9341.h>	

#include "LcdRoot.h"
#include "LcdMenu.h"
#include "LcdCalibWin.h"
#include <XPT2046_Touchscreen.h>
#include <LcdTouchscreen.h>
#include <Params.h>
#include <EEPROM.h>
#include "AppTopWin.h"
#include "AppCalibWin.h"

#include "fontx/ILGH24XB.H"
#include "fontx/ILGZ24XB.H"
#if USE_OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#include "wifi-conf.h"

#if USE_STATIC_IP
IPAddress staticIP(192,168,0,203);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
#endif

#define TFT_CS	 2
#define TFT_DC  15
#define TFT_RST -1
#define SD_CS	 0
#define TP_CS   16

ESP8266WebServer server(80);
SpiffsHandler fsHandler("rom",true,true);
SdHandler sdHandler("sd", "/", true, true);

Humblesoft_ILI9341 tft = Humblesoft_ILI9341(TFT_CS,TFT_DC,TFT_RST);
RomFontx font24(ILGH24XB,ILGZ24XB);

LcdTouchscreen ts(TP_CS);
LcdRoot root(&tft, &ts);
AppTopWin topWin;
AppCalibWin calibWin;

ParamEntry paramTable[] = {
  { PARAM_CALIBRATION, ""},
};
static uint16_t c1 = Humblesoft_ILI9341::rgb("GREEN");
static uint16_t c2 = Humblesoft_ILI9341::rgb("YELLOW");
static uint16_t bg = Humblesoft_ILI9341::rgb("BLACK");
static LcdMenuEntry meCalib("Calibration", &calibWin);

Params eeparam( paramTable, sizeof(paramTable)/sizeof(paramTable[0]));

void makeTopMenu(LcdMenu *pMenu)
{
  pMenu->add(&meCalib);
}

void setup(void){
  bool r;
  Serial.begin(115200);
	delay(100);
  Serial.println("\n\nReset:");

	WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#if USE_STATIC_IP
  WiFi.config(staticIP, gateway, subnet);
#endif

	
  // initialize SPI bus

  pinMode(TFT_CS,OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  pinMode(TP_CS, OUTPUT);
  digitalWrite(TP_CS, HIGH);
  
  // SDcard must be initilized first in SPI devices
  
  if(SD.begin(SD_CS)) {
    Serial.printf("initialize SD Card done.\n");
  } else
    Serial.printf("initialise SD Card failed.\n");

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen("BLACK");
  tft.setTextWrap(false);

  // touchscreen calibration 

  ts.begin();
  eeparam.load();
  if(ts.touched() || !ts.loadCalibrationData(&eeparam)){
    do {
      r = ts.calibrate(&tft, c1, bg);
      tft.fillScreen(bg);
      tft.setTextColor(c2);
      tft.setCursor(0,0);
      tft.printf("calibration %s.", r ? "succeeded" : "failed");
      Serial.printf("calibration %s.", r ? "succeeded" : "failed");
      delay(1000);
    } while(!r);
		ts.saveCalibrationData(&eeparam);
    if(eeparam.changed())
      eeparam.save();
  }

  // Wait for connection
#if 1
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.print("IP: "); 
  tft.println(WiFi.localIP());
#endif

#if USE_OTA
	ota_init();
#endif

  makeTopMenu(&topMenu);
  topWin.setMenu(&topMenu);
  root.pushWin(&topWin);
}

void loop(void){
#if USE_OTA
  ArduinoOTA.handle();
#endif
  server.handleClient();
  
  root.update();
}

/*** Local variables: ***/
/*** tab-width:2 ***/
/*** End: ***/
