#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>
#include <Ticker.h>
//================Version Steble or Unstable Conf ========
bool stable = true;

//==============================INISIALISASI=====================================
#define BLYNK_PRINT Serial
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String buildTag = ESCAPEQUOTE(BUILD_TAG);

char blynk_token[] = "eJwSQkMYPxeIIz0PMEZtKLTJc3Aqjejh";
char server[] = "blynk-cloud.com";

int clear = 0;
const int ESP_LED = 2;
int ledPin =  4; //D2     // the number of the LED pin
int button = 14; //D1
WidgetTerminal terminal(V12);
BlynkTimer timer;
void enableUpdateCheck();
Ticker updateCheck(enableUpdateCheck, 30000); // timer for check update with interval 30s
bool doUpdateCheck = true;
bool download = true;
WiFiManager wifiManager;

//=================== PROSEDUR & FUNGSI =====================
void enableUpdateCheck() {
  doUpdateCheck = true;
}

//=================Blynk-Conn=============================
bool isFirstConnect = true;
void conBlynk(){ 
  Blynk.config(blynk_token, server, 8080);
  Blynk.connect();
  if (isFirstConnect) {
    Blynk.syncVirtual(V12, V24, V25);
    isFirstConnect = false;
  }  
}

//==================Wifi-Setting==========================
void resetWifi(){
  wifiManager.resetSettings();
  delay(1000);
  ESP.reset();
  delay(3000);
}

//=====================================Download Firmware=====================================
void DownloadBin(){

  Serial.println("Checking Firmware...");
  terminal.println("Checking Firmware...\n");
  
  if (WiFi.status() == WL_CONNECTED) {
      //==========================downloading firmware.bin with HTTP OTA================
       if (stable == true){
        terminal.println("Stable");
        t_httpUpdate_return ret = ESPhttpUpdate.update("http://ota.firmandev.tech/stable/firmware.php?tag="+ buildTag );
       
        switch(ret) {
         case HTTP_UPDATE_FAILED:
          Serial.printf("UPDATE ERROR (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.printf("\nUPDATE ERROR (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.flush();
          break;

         case HTTP_UPDATE_NO_UPDATES:
          Serial.println(" Already in Current Version");
          terminal.println("Already in Current Version");
          terminal.println(buildTag);
          terminal.flush();
          clear=clear+1;
          break;
          
         case HTTP_UPDATE_OK:
          Serial.println("Updating FIrmware...");
          terminal.println("Updating FIrmware...");
          terminal.flush();
       }
      }else if (stable == false){
        terminal.println("Unstable");
        t_httpUpdate_return ret = ESPhttpUpdate.update("http://ota.firmandev.tech/unstable/firmware.php?tag="+ buildTag );
        switch(ret) {
         case HTTP_UPDATE_FAILED:
          Serial.printf("UPDATE ERROR (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.printf("\nUPDATE ERROR (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.flush();
          break;

         case HTTP_UPDATE_NO_UPDATES:
          Serial.println(" You're in Current Version");
          terminal.println("Already in Current Version");
          terminal.println(buildTag);
          terminal.flush();
          clear=clear+1;
          break;
          
         case HTTP_UPDATE_OK:
          Serial.println("Updating FIrmware...");
          terminal.println("Updating FIrmware...");
          terminal.flush();
          delay(1000);
       }
      }
      
      if (clear==5){
        terminal.clear();
        clear = 0;
      }
  }
}

void setup(){
  terminal.clear();
  //Serial.begin(9600);
  terminal.println("Booting........");
  wifiManager.autoConnect("DevOps");
 /* if (!wifiManager.autoConnect()) {
    resetWifi();
  } */
  conBlynk();
  terminal.println("Current Version: ");
  terminal.println(buildTag);
  updateCheck.start();
  pinMode(ledPin, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(ESP_LED, OUTPUT);
  updateCheck.start(); 
}

int ledState = 0;             // ledState used to set the LED

int temp = 1;
int hold = 0;
int counter = 0;

void loop(){
  updateCheck.update(); 
  Blynk.run();

  //========Bagian Program Utama, sesuaikan alatmu=========
  temp = digitalRead(button);
 // Serial.println(temp);
  if ( temp != hold){
    if ( temp == LOW ){
      counter++;
      Serial.println(counter);
    }
    delay (100);
  }
  hold=temp;
 
  if(counter == 1)
  {
    analogWrite(ledPin, 200); // Update the actual LED nyala redup
  }

  if (counter == 2)
  {
    analogWrite(ledPin, 1024);  // Update the actual LED nyala terang
  }

  if (counter == 3)
  {
    analogWrite(ledPin, ledState);
    counter = 0;    // reset counter
  }
  //======== Batas akhir program utama ===================

  if (doUpdateCheck == true){
    DownloadBin();
    doUpdateCheck = false;
  }
}

BLYNK_WRITE(V25){
    if (param.asInt() == 1) {  
     DownloadBin();
  } 
}
BLYNK_WRITE(V24){
    if (param.asInt() == 1) {  
      resetWifi();
  } 
}