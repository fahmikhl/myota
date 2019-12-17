#include <WiFiClientSecure.h> 
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#define BLYNK_PRINT Serial
// Stringifying the BUILD_TAG parameter
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String buildTag = ESCAPEQUOTE(BUILD_TAG);

char auth[] = "eJwSQkMYPxeIIz0PMEZtKLTJc3Aqjejh";
char ssid[] = "myota";
char pass[] = "myota123";


int clear = 0;
const int ESP_LED = 2;
const int ledBlink = 14;
WidgetTerminal terminal(V12);
BlynkTimer timer;
void enableUpdateCheck();
Ticker updateCheck(enableUpdateCheck, 30000); // timer for check update with interval 60s
bool doUpdateCheck = true;
bool download = true;
void enableUpdateCheck() {
  doUpdateCheck = true;
}
//=================Blynk-Conn=============================
bool isFirstConnect = true;
void conBlynk(){ 
  Blynk.begin(auth, ssid, pass);
  if (isFirstConnect) {
    Blynk.syncVirtual(V12, V24, V25);
    isFirstConnect = false;
  }  
}

//==================Wifi-Setting==========================
void setWifi(){
  const char* ssid = "cidro";
  const char* password = "mbayarsu";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting...");
  }
  if (WiFi.status() == WL_CONNECTED) {
     Serial.println("Connected !!");
     terminal.println("WiFi Connected");
     // digitalWrite(ESP_LED, LOW);
  }
}

//=====================================Download Firmware=====================================
void DownloadBin(){

  Serial.println("Checking Firmware...");
  terminal.println("Checking Firmware...\n");
  if (WiFi.status() == WL_CONNECTED) {
      //==========================downloading firmware.bin with HTTP OTA================
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://ota.firmandev.tech/myota/firmware.php?tag="+ buildTag );

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
          delay(1000);
      }
      if (clear==5){
        terminal.clear();
        clear = 0;
      }
  }
}

void setup(){
  terminal.clear();
  Serial.begin(9600);
  Serial.println("Booting...");
  terminal.println("Booting........");
  setWifi();
  conBlynk();
  pinMode(ESP_LED, OUTPUT);
  Serial.println("Current Version: "+ buildTag);
  terminal.println("Current Version: ");
  terminal.println(buildTag);
  terminal.flush();
  updateCheck.start();
}

void loop(){
  updateCheck.update();  
  Blynk.run();
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